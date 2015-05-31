all: filesender 

filesender: filesender.c
	gcc -D_POSIX_C_SOURCE -g filesender.c -std=c99 -I../lib -L../lib -Wl,-rpath=../lib -o filesender -lbufio -lhelpers

clean:
	rm -f filesender
