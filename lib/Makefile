all: libhelpers.so libbufio.so 

libhelpers.so: helpers.o 
	gcc -shared helpers.o -o libhelpers.so

libbufio.so: bufio.o
	gcc -shared bufio.o -o libbufio.so


helpers.o: helpers.c
	gcc -c -fPIC -std=c99 helpers.c	-o helpers.o

bufio.o: bufio.c 
	gcc -c -fPIC -std=c99 bufio.c	-o bufio.o


clean:
	rm -f helpers.o
	rm -f libhelpers.so
	rm -f bufio.o
	rm -f libbufio.so 
