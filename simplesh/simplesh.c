#include <helpers.h>
#include <bufio.h>
#include <stdio.h>
#include <string.h>
#define BUF_SIZE 4096
#define COMMANDS_LIMIT 128
#define TOKENS_PER_COMMAND_LIMIT 1024

int main() {
    struct buf_t* b = buf_new(BUF_SIZE);
    char line[3*BUF_SIZE];
    char* commands[COMMANDS_LIMIT];
    char* command_tokens[COMMANDS_LIMIT][TOKENS_PER_COMMAND_LIMIT];
    int token_count[COMMANDS_LIMIT];
    
    write_(STDIN_FILENO, "$ ", 2);
    while (buf_getline(STDIN_FILENO, b, line) > 0) {
        char *p = strtok(line, "|");
        int com_count = 0;
        while (p) {
            commands[com_count++] = p;
            printf("com %d\t%s\n", com_count - 1, p);
            p = strtok(0, "|");
        }
        for (int i = 0; i < com_count; i++) {
            p = strtok(commands[i], " ");
            int t_c = 0;
            while (p) {
                command_tokens[i][t_c++] = p;
                printf("com %d\t tok:%d\t %s\n", i, t_c - 1, p);
                p = strtok(0, " ");
            }
        }
        write_(STDIN_FILENO, "$ ", 2);
    };

    /*
    char *pname = "ls";
    char const* arr[10] = {"--color=auto", NULL};
    execargs_t * lal = args_new(pname, (char const **) arr, 1);
    char *grp = "grep";
    char const* arr2[10] = {"sim", NULL};
    execargs_t * lal2 = args_new(grp, (char const **) arr2, 1);
    execargs_t * progs[5] = {lal, lal2, NULL};
    runpiped(progs, 2);
    */
    return 0;
}
