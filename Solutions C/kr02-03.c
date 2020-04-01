#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>

int
main(int argc, char *argv[])
{
    int pid, fd[2];
    if (pipe(fd)) {
        return 1;
    }
    pid = fork();
    if (pid < 0) {
        close(fd[1]);
        close(fd[0]);
        return 1;
    }
    if (!pid) {
        close(fd[1]);
        pid = fork();
        if (pid < 0) {
            close(fd[0]);
            _exit(1);
        }
        if (!pid) {
            int cur;
            long long sum = 0;
            while (read(fd[0], &cur, sizeof(cur)) == sizeof(cur)) {
                sum += cur;
            }
            close(fd[0]);
            printf("%lld\n", sum);
            return 0;
        }
        wait(NULL);
        close(fd[0]);
        _exit(0);
    }

    close(fd[0]);
    int val;
    while (scanf("%d", &val) == 1) {
        write(fd[1], &val, sizeof(val));
    }
    close(fd[1]);
    while (wait(NULL) > 0) {}

    return 0;
}
