all: bufcat 

bufcat: bufcat.c
	gcc bufcat.c -std=c99 -I../lib -L../lib -o bufcat -lbufio

clean:
	rm -f bufcat
