all: lib.so

lib.so: helpers.o
	gcc -shared helpers.o -o libhelpers.so
	gcc -shared bufio.o -o libbufio.so

helpers.o:
	gcc -c -fPIC -std=c99 helpers.c	-o helpers.o
	gcc -c -fPIC -std=c99 bufio.c	-o bufio.o

clean:
	rm -f helpers.o
	rm -f libhelpers.so
	rm -f bufio.o
	rm -f libbufio.so 
