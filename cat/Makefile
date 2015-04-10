all: cat

cat: cat.c
	gcc cat.c -std=c99 -I../lib -L../lib -o cat -lhelpers

clean:
	rm -f cat
