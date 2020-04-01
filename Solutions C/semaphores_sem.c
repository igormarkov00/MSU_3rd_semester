#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>



int
main(int argc, char *argv[])
{
    int key;
    scanf("%d", &key);
    int shmid = shmget(key, 256, IPC_CREAT | 0666);
    int semid = semget(key, 2, IPC_CREAT | 0666);
    char *str = shmat(shmid, NULL, 0);
    struct sembuf up = {0, 1, SEM_UNDO}, down = {0, -1, SEM_UNDO};
    if (!fork()) {
        down.sem_num = 1;
        while (fgets(str, 256, stdin)) {
            semop(semid, &up, 1);
            semop(semid, &down, 1);
        }
        semctl(semid, 2, IPC_RMID, 0);
        shmdt(str);
        return 0;
    }
    if (!fork()) {
        up.sem_num = 1;
        while (!semop(semid, &down, 1)) {
            printf("%s", str);
            semop(semid, &up, 1);
        }
        return 0;
    }
    wait(NULL);
    wait(NULL);
    shmdt(str);
    shmctl(shmid, IPC_RMID, 0);

    return 0;
}
