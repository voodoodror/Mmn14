/*
 * assembler.h
 *
 *  Created on: Mar 3, 2016
 *      Author: dbletter
 */

#define MAX_LINE 80
#define BUF_SIZE 100

void first_parsing_line (char *line, int *count);
char *hasSymbol(char* str);
char *getSymbol(char* str);

typedef struct symbolList {
	char *Sym;
	unsigned int addrSym;
	int extSym;
	int actionSym;
};
