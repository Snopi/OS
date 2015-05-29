all: simplesh 

simplesh: simplesh.c
	gcc -D_POSIX_C_SOURCE  -g simplesh.c -std=c99 -I../lib -L../lib -Wl,-rpath=../lib -o simplesh -lbufio -lhelpers

clean:
	rm -f simplesh
