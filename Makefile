all: main
main: main.c alloc.c
	gcc  -o main -ansi -pedantic -Wall -Werror main.c alloc.c
