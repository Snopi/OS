#include <stdio.h>
#include <bufio.h>

#define BUF_MAX_CAPACITY 4096 

int main() {
    struct buf_t * b = buf_new(BUF_MAX_CAPACITY);
    int b_fill;
    while(1) {
        b_fill = buf_fill(STDIN_FILENO, b, buf_capacity(b));
//        printf("bfill = %d\n", b_fill);
        if (b_fill < 0) {
            if (buf_flush(STDOUT_FILENO, b, buf_size(b)) < 0) {
                return 2;
            }
            return 1;
        }
        if (!b_fill) {
            return 0;
        }
        if (buf_flush(STDOUT_FILENO, b, buf_size(b)) < 0) {
            return 3;
        }
    }
    return 0;
}
