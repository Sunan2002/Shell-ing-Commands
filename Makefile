all: main.o
	gcc -o shell.out main.o

main.o: main.c
	gcc -c main.c

run:
	./shell.out