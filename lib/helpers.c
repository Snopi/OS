#include "helpers.h"
#include <unistd.h>

ssize_t read_(int fd, void *buf, size_t count) {
    size_t offset = 0;
    ssize_t ret_value;
    while (1) {
	ret_value = read(fd, buf + offset, count - offset);
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
    while (1) {
	ret_value = write(fd, buf + offset, count - offset);
	if (ret_value < 0) {
	    return ret_value;
	}
	offset += ret_value;
	if (offset >= count) {
	    return offset;
	}
    }
}


