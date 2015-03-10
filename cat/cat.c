#include <helpers.h>
#include <stdio.h>

const size_t BUF_SIZE = 4 * 1024;

int main() {
    char buf[BUF_SIZE];
    while (1) {
	ssize_t read_count = read_(STDIN_FILENO, buf, BUF_SIZE);
	if (read_count < 0) {
	    return 1;
	}
	if (read_count == 0) {
	    return 0;
	}
	ssize_t write_count = write_(STDOUT_FILENO, buf, read_count);
	if (write_count < 0) {
	    return 2;
	}
    }
}
