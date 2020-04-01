#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <fcntl.h>

int
main(int argc, char *argv[])
{
    argc--;
    int semid = semget(IPC_PRIVATE, argc, 0666);
    int shmid = shmget(IPC_PRIVATE, (argc) * sizeof(int), 0666);
    int *mas = shmat(shmid, NULL, 0), bank_size;
    struct stat sb;
    stat(argv[1], &sb);
    bank_size = sb.st_size;
    for (int i = 0; i < argc;  ++i) {
        if (!fork()) {
            int fd = open(argv[i + 1], O_RDONLY);
            if (fd < 0) {
                printf("ERROR\n");
            }
            while (!semop(semid, &(struct sembuf) {i, -1, 0}, 1)) {
                char res = -1;
                mas[i] /= argc;
                if (mas[i] < bank_size) {
                    lseek(fd, mas[i], SEEK_SET);
                    read(fd, &res, 1);
                }
                mas[i] = res;
                semop(semid, &(struct sembuf) {i, -1, 0}, 1);
            }
            shmdt(mas);
            close(fd);
            return 0;
        }
    }
    int addr;
    while (scanf("%d", &addr) == 1) {
        int bank_num = addr % argc;
        mas[bank_num] = addr;
        semop(semid, &(struct sembuf) {bank_num, 2, 0}, 1);
        semop(semid, &(struct sembuf) {bank_num, 0, 0}, 1);
        printf("%d\n", mas[bank_num]);
    }
    shmdt(mas);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID, NULL);
    while (wait(NULL) > 0);

    return 0;
}
