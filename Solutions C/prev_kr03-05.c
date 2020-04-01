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
#include <string.h>

int
main(int argc, char *argv[])
{
    int cnt = 0, prev = 1;
    for (int i = 1; i <= argc; ++i) {
        if (i == argc || argv[i][0] == 's') {
            int *son = calloc(i - prev, sizeof(int));
            for (int j = prev; j < i; ++j) {
                son[j - prev] = fork();
                if (!son[j - prev]) {
                    char *name = calloc(strlen(argv[j]), sizeof(argv[j][0]));
                    strcpy(name, argv[j] + 1);
                    execlp(name, name, NULL);
                    _exit(1);
                }
            }
            for (int j = 0; j < i - prev; ++j) {
                int st;
                if (son[j] > 0) {
                    waitpid(son[j], &st, 0);
                    cnt += (WIFEXITED(st) && !(WEXITSTATUS(st)));
                }
            }
            if (i != argc) {
                int pid = fork(), st;
                if (!pid) {
                    char *name = calloc(strlen(argv[i]), sizeof(argv[i][0]));
                    strcpy(name, argv[i] + 1);
                    execlp(name, name, NULL);
                    _exit(1);
                }
                if (pid > 0) {
                    waitpid(pid, &st, 0);
                    cnt += (WIFEXITED(st) && !(WEXITSTATUS(st)));
                }
                prev = i + 1;
            }
        }
    }
    printf("%d\n", cnt);

    return 0;
}
