#include <helpers.h>
#include <stdio.h>
#include <string.h>

#define MAX_LEN 4096

int main(int argc, char **argv) {
    char buf[MAX_LEN];
    char* arguments[argc + 1];
    arguments[argc] = NULL;
    arguments[argc - 1] = buf;
    int i;
    for (i = 0; i < argc - 1; i++) {
        arguments[i] = argv[i + 1];
    }
    int read_res;
    int write_res;
    int spawn_res;
    while (1) {
        read_res = read_until(STDIN_FILENO, buf, MAX_LEN - 1, '\n');
        if (read_res < 0) {
            perror("some error occured, while reading:");
            return 1;
        }
        if (read_res == 0) {
            return 0;
        }
        read_res -= (buf[read_res - 1] == '\n' ? 1 : 0);
        buf[read_res] = 0;
        spawn_res = spawn(argv[1], arguments);
        if (!spawn_res) {
            buf[read_res] = '\n';
            write_res = write_(STDOUT_FILENO, buf, read_res + 1);
            if (write_res < 0) {
                perror("some error occured, while writing:");
                return 2;
            }
        }
    }
}
