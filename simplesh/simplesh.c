#include <helpers.h>
#include <bufio.h>
#include <stdio.h>

int main() {
    char *pname = "ls";
    char const* arr[10] = {"--color=auto", NULL};
    execargs_t * lal = args_new(pname, (char const **) arr, 1);
    exec(lal);
    return 0;
}
