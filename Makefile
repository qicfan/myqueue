main:main.c link.o main.o
	gcc -o main link.o main.o -levent
link.o:link.h link.c
	gcc -c link.c
main.o:link.h
	gcc -c main.c -levent

clean:
	rm -rf link.o main.o
