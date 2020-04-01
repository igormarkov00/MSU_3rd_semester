#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <math.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
    double x0 = strtod(argv[1], NULL), dx = strtod(argv[3], NULL);
    int n = strtol(argv[2], NULL, 10);
    int fd12[2], fd21[2];
    pipe(fd12);
    pipe(fd21);
    if (!fork()) {
        if (!fork()) {
            double res = 0;
            for (int i = 0; i < n + 1; ++i) {
                double cur = sin(x0);
                write(fd12[1], &cur, sizeof(cur));
                read(fd21[0], &cur, sizeof(cur));
                res += cur * cur;
                x0 += dx;
            }
            printf("1 %.10lf\n", res);
        }
        wait(NULL);
        return 0;
    }
    if (!fork()) {
        if (!fork()) {
            double res = 0;
            for (int i = 0; i < n + 1; ++i) {
                double cur;
                read(fd12[0], &cur, sizeof(cur));
                if (cur < 0) {
                    cur = -cur;
                }
                res += cur;
                cur = cos(x0);
                write(fd21[1], &cur, sizeof(cur));
                x0 += dx;
            }
            printf("2 %.10lf\n", res);
        }
        wait(NULL);
        return 0;
    }
    while (wait(NULL) > 0) {}
    printf("0 0\n");

    return 0;
}
