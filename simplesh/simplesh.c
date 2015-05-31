#include <helpers.h>
#include <bufio.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <strings.h> // bzero is here
#define BUF_SIZE 4096
#define COMMANDS_LIMIT 128
#define TOKENS_PER_COMMAND_LIMIT 512

void sigint_ha(int sig) {} // ignore it

int main() {
    struct sigaction sa;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = &sigint_ha;
    //sa.sa_flags |= SA_RESTART;
    if (sigaction(SIGINT, &sa, NULL) < 0)
        return -1;
    
    struct buf_t* b = buf_new(BUF_SIZE);
    char line[2 * BUF_SIZE];
    char* commands[COMMANDS_LIMIT];
    char* command_tokens[COMMANDS_LIMIT][TOKENS_PER_COMMAND_LIMIT];
    execargs_t exargs[COMMANDS_LIMIT];
    execargs_t* arr_of_ptrs[COMMANDS_LIMIT];
    for (int i = 0; i < COMMANDS_LIMIT; i++) {
        arr_of_ptrs[i] = &exargs[i];
    }

    write_(STDOUT_FILENO, "$ ", 2);
    int b_r;
    while (1) {
        b_r = buf_getline(STDIN_FILENO, b, line);
        if (b_r == 0) {
            return 0;
        }
        if (b_r < 0) {
            if (errno == EINTR) {
                write_(STDOUT_FILENO, "\n$ ", 3);
                continue;
            }
            return -1;
        }
        char *p = strtok(line, "|");
        int com_count = 0;
        while (p) {
            commands[com_count++] = p;
            p = strtok(0, "|");
        }
        for (int i = 0; i < com_count; i++) {
            p = strtok(commands[i], " ");
            int t_c = 0;
            while (p) {
                command_tokens[i][t_c++] = p;
                p = strtok(0, " ");
            }
            command_tokens[i][t_c] = NULL;
            exargs[i].program_arguments = (char **) &command_tokens[i];
        }
        runpiped(arr_of_ptrs, com_count); //stay alive
        write_(STDOUT_FILENO, "$ ", 2);
    };
    return 0;
}
