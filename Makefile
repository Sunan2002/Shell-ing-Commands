all: main.o parser.o exec.o
	gcc -o shell.out main.o parser.o exec.o

main.o: main.c parser.h exec.h
	gcc -c main.c

parser.o: parser.c parser.h
	gcc -c parser.h parser.c

exec.o: exec.c exec.h
	gcc -c exec.c exec.h

run:
	./shell.out