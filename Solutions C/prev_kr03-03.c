#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>

volatile int mode, cnt = 1;

void
handler1(int sig)
{
    if (cnt == 5) {
        _exit(0);
    }
    cnt++;
    mode ^= 1;
}

void
handler2(int sig)
{
    unsigned ip, ans = 0, add = 1;
    scanf("%o", &ip);
    if (!(ip & (1U << 31)) || (ip & (1U << 30))) {
        printf("0\n");
        return;
    }
    if (!mode){
        for (int i = 16; i < 30; ++i) {
            if ((1 << i) & ip) {
                ans += add;
            }
            add <<= 1;
        }
    } else {
        for (int i = 0; i < 16; ++i) {
            if ((1 << i) & ip) {
                ans += add;
            }
            add <<= 1;
        }
    }
    printf("%d\n", ans);
    fflush(stdout);
}

int
main(int argc, char *argv[])
{
    sigaction(SIGUSR1, &(struct sigaction) {.sa_handler = handler1, .sa_flags = SA_RESTART}, NULL);
    sigaction(SIGUSR2, &(struct sigaction) {.sa_handler = handler2, .sa_flags = SA_RESTART}, NULL);
    printf("%d\n", getpid());
    for (;;) {
        pause();
    }

    return 0;
}

