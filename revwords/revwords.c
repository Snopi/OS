#include <helpers.h>
#include <stdio.h>

#define BUFFER_SIZE 4 * 1024 + 10

//reverse array of chars [i, j];
void rev(char *arr, int i, int j) {
	char tmp;
	while (i < j) {
		tmp = arr[i];
		arr[i++] = arr[j];
		arr[j--] = tmp;
	}
}

int main() {
	char buf[BUFFER_SIZE];
	char delim = ' ';
	int res;
	while (1) {
		res = read_until(STDIN_FILENO, buf, BUFFER_SIZE, delim);
		if (res < 0) {
			perror("The following error occured, while reading");
			return 1;
		}
		if (res == 0) {
			return 0;
		}
		rev(buf, 0, buf[res - 1] == delim ? res - 2 : res - 1);
		res = write_(STDOUT_FILENO, buf, res);
		if (res < 0) {
			perror("The following error occured, while writing");
			return 2;
		}
	}
	return 0;
}