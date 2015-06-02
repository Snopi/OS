all: forking 

forking: forking.c
	gcc -D_POSIX_C_SOURCE -g forking.c -std=c99 -I../lib -L../lib -Wl,-rpath=../lib -o forking -lbufio -lhelpers

clean:
	rm -f forking
