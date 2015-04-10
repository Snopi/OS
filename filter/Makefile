all: filter

filter: filter.c
	gcc filter.c -std=c99 -I../lib -L../lib -o filter -lhelpers

clean:
	rm -f filter
