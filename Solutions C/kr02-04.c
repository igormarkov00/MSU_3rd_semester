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
    if (argc < 3) {
        return 0;
    }
    int prev = argc - 1;
    for (int i = argc - 1; i > 0; --i) {
        if (!strcmp(argv[1], argv[i])) {
            int pid;
            if (!(pid = fork())) {
                char **new_argv = calloc(prev - i + 1, sizeof(char*));
                for (int j = i + 1; j <= prev; ++j) {
                    new_argv[j - i - 1] = malloc(sizeof(argv[j]));
                    strcpy(new_argv[j - i - 1], argv[j]);
                }
                new_argv[prev - i] = NULL;
                execvp(argv[i + 1], new_argv);
                _exit(1);
            }
            int st;
            waitpid(pid, &st, 0);
            if (WIFSIGNALED(st) || (WIFEXITED(st) && WEXITSTATUS(st))) {
                return 1;
            }
            prev = i - 1;
        }
    }

    return 0;
}
