#include "bufio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct buf_t *buf_new(size_t capacity) {
    struct buf_t * new_buf = (struct buf_t *) malloc(sizeof(struct buf_t));
    if (!new_buf) {
        return NULL;
    }
    new_buf->capacity = capacity;
    new_buf->size = 0;
    new_buf->buf = (char *) malloc(capacity + 1); // Some extra char for '\0' in the end for some purposes...
    if (!new_buf->buf) {
        free(new_buf);
        return NULL;
    }
    return new_buf;
}

ssize_t buf_getline(fd_t fd, struct buf_t *buf, char *dest) {
    buf->buf[buf->size] = '\0';
    char * occurence;
    int offset = 0;
    int fill_res;
    while (!(occurence = strchr(buf->buf, '\n'))) {
        memmove(dest + offset, buf->buf, buf->size);
        offset += buf->size;
        buf->size = 0;
        buf_fill(fd, buf, 1);
        buf->buf[buf->size] = '\0';
        if (!buf->size)
            return -1; //no lines anymore
    }
    *occurence = '\0';
    int cnt = occurence - buf->buf + 1;
    memcpy(dest + offset, buf->buf, cnt); //with 0
    memmove(buf->buf, occurence + 1, buf->size - cnt);
    buf->size -= cnt;
    return cnt + offset;
}

void buf_free(struct buf_t * b) {
    #ifdef DEBUG
    if (!b) {
        abort();
    }
    #endif

    free(b->buf);
    free(b);
}

size_t buf_capacity(struct buf_t * b) {
    #ifdef DEBUG
    if (!b) {
        abort();
    }
    #endif

    return b->capacity;
}

size_t buf_size(struct buf_t * b) {
    #ifdef DEBUG
    if (!b) {
        abort();
    }
    #endif

   return b->size;
}

ssize_t buf_fill(fd_t fd, struct buf_t *buf, size_t required) {
    #ifdef DEBUG
    if (required > buf_capacity(buf)) {
        abort();
    }
    #endif

    ssize_t read_r;
    while (buf_size(buf) < required) {
        read_r = read(fd, buf->buf + buf_size(buf), buf_capacity(buf) - buf_size(buf));
        if (!read_r) {
            break; // eof, or buffer end.
        }
        if (read_r < 0) {
            return -1;
        }
        buf->size += read_r;
    }
    return buf_size(buf);
}

ssize_t buf_flush(fd_t fd, struct buf_t *buf, size_t required) {
    #ifdef DEBUG
    if (!buf) {
        abort();
    }
    #endif

    ssize_t write_res;
    size_t flushed = 0;

    while (flushed < required && flushed < buf->size
         &&  (write_res = write(fd, buf->buf + flushed, buf->size - flushed)) >= 0) {
        flushed += write_res;
    }
    buf->size -= flushed;
    for (int i = 0; i < buf->size; i++) {
        buf->buf[i] = buf->buf[i + flushed];
    }
    if (write_res < 0) {
        return -1;
    }
    return flushed;
}
