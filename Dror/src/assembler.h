/*
 * assembler.h
 *
 *  Created on: Mar 3, 2016
 *      Author: dbletter
 */

#define MAX_LINE 80
#define BUF_SIZE 100

typedef struct symbolLists {
	char *Sym;
	unsigned int addr;
	int ext;
	int action;
	struct symbolLists* next;
} mySymbolList;


void first_parsing_line (char *line, int *count);
void strip_extra_spaces (char* str);
int hasSymbol(char* str);
char *hasDot(char* str);
int extractData(char* str);
int extractString(char* str);
char *getSymbol(char* str, int pos);
char *getDotInstruction(char* str);
char *hasQM(char* str);

mySymbolList *createSymbolNode (char* str, unsigned int dc, int external, int action);
	mySymbolList *addSymbolNode (mySymbolList* symbolList, char* str, unsigned int dc, int external, int action);
