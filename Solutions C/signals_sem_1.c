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
foo(int sig)
{
	signal(SIGINT, foo);
	cnt++;
	if (cnt == 4) {
		signal(SIGINT, SIG_DFL);
	}
}

int
main(void)
{
	signal(SIGINT, foo);
	for(;;) {
		pause();
	}

	return 0;
}
