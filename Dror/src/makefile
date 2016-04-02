main: main.o utils.o struct.o first_parsing.o second_parsing.o
	gcc -g -ansi -Wall main.o utils.o struct.o first_parsing.o second_parsing.o -o main

main.o: main.c main.h struct.h utils.h first_parsing.h second_parsing.h
	gcc -c main.c -ansi -Wall -o main.o

struct.o: struct.c struct.h
	gcc -c struct.c -ansi -Wall -o struct.o

utils.o: utils.c utils.h struct.h
	gcc -c utils.c -ansi -Wall -o utils.o

first_parsing.o: first_parsing.c struct.h main.h utils.h first_parsing.h second_parsing.h
	gcc -c first_parsing.c -ansi -Wall -o first_parsing.o

second_parsing.o: second_parsing.c struct.h first_parsing.h second_parsing.h main.h utils.h
	gcc -c second_parsing.c -ansi -Wall -o second_parsing.o