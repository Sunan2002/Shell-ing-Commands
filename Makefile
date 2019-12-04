all: main.o parser.o
	gcc -o shell.out main.o parser.o

main.o: main.c parser.h
	gcc -c main.c

parser.o: parser.c parser.h
	gcc -c parser.h parser.c

run:
	./shell.out