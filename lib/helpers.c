#include "helpers.h"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int spawn(const char * file, char * const argv []) {
    pid_t pid = fork();
    if (pid < 0) {
        return -2;
    }
    int status;
    if (!pid) {
        execvp(file, argv);
    }
    pid_t wait_res = waitpid(pid, &status, 0);
    if (wait_res != pid) {
        return -1;
    }
    return status;
}


ssize_t read_(int fd, void *buf, size_t count) {
    size_t offset = 0;
    ssize_t ret_value;
    char *char_buf = (char *) buf;
    while (1) {
		ret_value = read(fd, char_buf + offset, count - offset);
		if (ret_value < 0) {
	  		return ret_value;
		}
		if (ret_value == 0) {
		    return offset;
		}
		offset += ret_value;
    }
}

ssize_t write_(int fd, const void *buf, size_t count) {
    size_t offset = 0;
    ssize_t ret_value;
    char *char_buf = (char *) buf;
    while (1) {
		ret_value = write(fd, char_buf + offset, count - offset);
		if (ret_value < 0) {
			return ret_value;
		}
		offset += ret_value;
		if (offset >= count) {
		  	return offset;
		}
    }
}

ssize_t read_until(int fd, void *buf, size_t count, char delimiter) {
	size_t offset = 0;
	ssize_t ret_value;
	char *char_buf = (char *) buf;
	while (1) {
		ret_value = read(fd, char_buf + offset, 1);
		if (ret_value < 0) {
			return ret_value;
		}
		offset += ret_value;
		if (ret_value == 0 || offset == count || char_buf[offset - 1] == delimiter) {
			return offset;
		}
	}
}
