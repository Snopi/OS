all: forking polling

forking: forking.c
	gcc -D_POSIX_C_SOURCE -g forking.c -std=c99 -I../lib -L../lib -Wl,-rpath=../lib -o forking -lbufio -lhelpers

polling: polling.c
	gcc -D_POSIX_C_SOURCE -g polling.c -std=c99 -I../lib -L../lib -Wl,-rpath=../lib -o polling -lbufio -lhelpers

clean:
	rm -f forking
