all: simplesh 

simplesh: simplesh.c
	gcc simplesh.c -std=c99 -I../lib -L../lib -o simplesh -lbufio -lhelpers

clean:
	rm -f simplesh
