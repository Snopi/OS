#include <helpers.h>
#include <stdio.h>
#include <string.h>

#define MAX_LENGTH 4096 

int substr(char* str, char* pattern) {
	while(*str != 0 && *pattern !=0) {
		if (*str != *pattern) {
			return 0;
		}
		str++;
		pattern++;
	}
	if (*pattern == 0) {
		return 1;
	}
	return 2;
}

int main(int argc, char** argv) {
	char buf[MAX_LENGTH];
	if (argc < 2) {
		write_(STDOUT_FILENO, "few arguments", 13);
		return 1;
	}
	int res;
	char* pattern = argv[1];
	int offset = 0;
	int len = strlen(argv[1]);
	int i;
	while(1) {
		res = read_(STDIN_FILENO, buf + offset, MAX_LENGTH - 1 - offset);
		if (res < 0) {
			return 1;
		}
		
		if (res + offset == 0) {
			return 0;
		}
		if (res == 0 && offset) {
			write_(STDOUT_FILENO, buf, offset);
			return 0;
		}
		buf[res + offset] = 0;
		offset = 0;
		int res_substr;
		for (i = 0; buf[i]; ) {
			res_substr = substr(buf + i, pattern);
			if (res_substr == 0) {
				write_(STDOUT_FILENO, buf + i, 1);
				i++;
				continue;
			}
			if (res_substr == 1) {
				i += len;
				continue;
			}
			if (res_substr == 2) {
				while(buf[i]) {
					buf[offset++] = buf[i++];
				}
				break;
			}
		}
	}	
}
