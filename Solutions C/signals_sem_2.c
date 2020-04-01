#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <limits.h>
#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>

volatile int cnt = 0;

void
handler(int sig)
{
	printf("-1\n");
	_exit(0);
}

int
main(int argc, char *argv[])
{
	int cur, t = strtol(argv[1], NULL, 10);
	signal(SIGALRM, handler);
	alarm(t);
	scanf("%d", &cur);
	while (cur) {
		printf("%d ", cur * cur);
		scanf("%d", &cur);
	}

	return 0;
}
