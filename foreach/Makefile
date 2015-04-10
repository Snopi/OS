all: foreach 

foreach: foreach.c
	gcc foreach.c -g -std=c99 -I../lib -L../lib -o foreach -lbufio -lhelpers

clean:
	rm -f foreach
