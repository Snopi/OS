#include <helpers.h>
#include <bufio.c>
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
    struct buf_t * in_buf = buf_new(1024);
    if (!in_buf) {
        return 0;
    }
    while (1) {
        read_res = buf_getline(STDIN_FILENO, in_buf, buf);
        if (read_res == 1) {
            continue;
        }
        if (read_res == 0) {
            break;
        }
        read_res--;
        buf[read_res] = 0;
        if (read_res & 1) {
            continue;
        }
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
    buf_free(in_buf);
}
