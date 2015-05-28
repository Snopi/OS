#ifndef HELPERS_H
#define HELPERS_H

#include <unistd.h>

ssize_t read_(int fd, void *buf, size_t count);

ssize_t write_(int fd, const void *buf, size_t count);

ssize_t read_until(int fd, void *buf, size_t count, char delimiter);

int spawn(const char * file, char * const argv []);

struct execargs {
    char **program_arguments; //null terminated arguments
};

typedef struct execargs execargs_t;
int exec(execargs_t* args);
int runpiped(execargs_t ** programs, size_t n);
char * mstrdup(char const * str);
execargs_t * args_new(char const * prog, char const ** args, int n); 
void args_free(execargs_t * args); 
#endif
