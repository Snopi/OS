#define _GNU_SOURCE 
//man said that
#include "helpers.h"
#include <fcntl.h> //pipes
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

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

//is it a solution? 
//sig handler with some variables in LIBRARY

int prog_count;
int* kill_progs;
void sigint_ha(int s) {
    for (int i = 0; i < prog_count; i++) {
        kill(kill_progs[i], SIGKILL);
    }
    prog_count = 0;
}


int runpiped(execargs_t ** programs, size_t n) {
    if (!n) {
        //lal input; You want nothing...
        return 0;
    }
    int pipes[n - 1][2]; //lots of pipes;
    // throw from here <<<<< [0] <<<<<< [1] <<<<< write here
    int progs_pids[n];
    for (int i = 0; i < n - 1; i++) {
        pipe2(pipes[i], O_CLOEXEC); //autoclose all this pipes on exec
        //only dupped will survive
    }  
    for (int i = 0; i < n; i++) {
        if ((progs_pids[i] = fork()) == 0) { //i'm child
            if (i) { //I need new STDIN
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            if (i != n - 1) { // I need new STDOUT
                dup2(pipes[i][1], STDOUT_FILENO); 
            }
            execvp(programs[i]->program_arguments[0], programs[i]->program_arguments);
            _exit(-1); //Something has gone wrong...
        }
        if (progs_pids[i] < 0) { //someone fails
            return -1;
        }
    }
    //parent doesn't need pipes
    for (int i = 0; i < n - 1; i++) 
        close(pipes[i][1]), close(pipes[i][0]);
    
    prog_count = n;
    kill_progs = progs_pids;
    
    struct sigaction sa;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = &sigint_ha;
    if (sigaction(SIGINT, &sa, NULL) < 0)
        return -1;
      
    int res = 0;
    int child_res;
    for (int i = 0; i < n; i++) {
        waitpid(progs_pids[i], &child_res, 0); //progs_pids.foreach(Thread::join)
        res |= child_res;
    }

    return res; 
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
