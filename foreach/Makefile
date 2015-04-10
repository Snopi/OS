all: foreach 

foreach:
	gcc foreach.c -std=c99 -I../lib -L../lib -o foreach -lbufio -lhelpers

clean:
	rm -f foreach
