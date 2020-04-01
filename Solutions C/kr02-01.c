#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>

void
prec(int ver, int cur_h, int h)
{
    if (cur_h == h) {
        printf("%d\n", ver - 1);
        fflush(stdout);
        _exit(0);
    }
    if (!fork()) {
        prec(2 * ver, cur_h + 1, h);
    }
    while (wait(NULL) > 0) {}
    printf("%d\n", ver - 1);
    fflush(stdout);
    if (!fork()) {
        prec(2 * ver + 1, cur_h + 1, h);
    }
    while (wait(NULL) > 0) {}
    _exit(0);
}

int
main(int argc, char *argv[])
{
    int n = strtol(argv[1], NULL, 10);
    prec(1, 1, n + 1);

    return 0;
}
