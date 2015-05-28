#include "helpers.h"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h> //pipes

char * mstrdup(const char * str) {
    int n = strlen(str) + 1;
    char * l = (char *) malloc(n);
    if (l) {
        strcpy(l, str);
    }
    return l;
}

int exec(execargs_t* args) {
   int status = spawn(args->program_arguments[0], args->program_arguments); 
   if (status != 0) {
       return -1;
   }
   return 0;
}

execargs_t * args_new(char const * prog, char const ** args, int n) {
    execargs_t* e_args = (execargs_t*) malloc(sizeof(execargs_t));
    e_args->program_arguments = (char **) calloc(sizeof(char *), n + 2);
    e_args->program_arguments[0] =  mstrdup(prog);
    for (int i = 1; i <= n; i++) {
        e_args->program_arguments[i] = mstrdup(args[i - 1]);
    }
    return e_args;
}

void args_free(execargs_t * args) {
    for (int i = 0; args->program_arguments[i]; i++) {
        free(args->program_arguments[i]);
    }
    free(args->program_arguments);
}

int runpiped(execargs_t ** programs, size_t n) {
    if (!n) {
        //lal input; You want nothing...
        return 0;
    }
    int pipes[n - 1][2]; //lots of pipes;
    int progs_pids[n];
}


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
    return WEXITSTATUS(status);
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
