#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "assembler.h"

    char *lineToProcess;
    char symbolChar = ':';
    char dotChar = '.';
    char spaceChar = ' ';
    int symbolLen = 0;
    int dotLen = 0;
    int symbolCounter = 0;
    mySymbolList* symbolList;

int main(int argc, char **argv)
{
    int i;
    FILE *fp;
    char file_name[20];
    char *lineToProcess = malloc(sizeof(char) * BUF_SIZE);

    for (i = 1; i < argc; i++)
    {
        int count = 0;

        sprintf(file_name, "%s", argv[i]);
        fp = fopen(file_name, "r");

        if (fp == NULL)
        {
            printf("Error: can't find file %s\n", file_name);
            return -1;
        }

        mySymbolList* symbolList = createSymbolNode("test");

        while (fgets(lineToProcess, MAX_LINE, fp))
        {
        	first_parsing_line(lineToProcess, &count);
        	count++;
        }
        fclose(fp);
        mySymbolList* iter;
		for (iter = symbolList; NULL != iter; iter = iter->next) {
				printf("%s ", iter->Sym);
		}
    }
    return 0;
}

void first_parsing_line (char *line, int *count) {
	int i=0, dupSymbol=0;

	char *symbolPointer;
	char *tmp;

	strip_extra_spaces(line);
	if (line[0] == ';') {
		printf("%d: %s (comment, ignoring)\n",*count,line);
	} else {
		if (!isalpha(line[0]) && line[0] != '.') {
			printf("%d: %s (must begin with a letter OR a DOT!)\n",*count,line);
		} else {
			if (hasSymbol(line) != 0) {
				if (hasSymbol(line+(symbolLen+1)) != 0) {
					printf("%d: %s (multiple symbols, ignoring)\n",*count,line);
				} else if (hasSymbol(line+(symbolLen))) {
					if (!dupSymbol) {
						if (hasDot(line+(symbolLen+2)) != NULL) {
							if (strcmp(getDotInstruction(line+(symbolLen+3)),"string") == 0) {
								printf("%d: %s (string found)\n",*count,line);
							} else if (strcmp(getDotInstruction(line+(symbolLen+3)),"data") == 0) {
								printf("%d: %s (data found)\n",*count,line);
							} else {
								printf("%d: %s (unknown instruction line, ignoring)\n",*count,line);
							}
						}
					printf("%d: %s (1 valid symbol found)\n",*count,line);

					symbolPointer = getSymbol(line,hasSymbol(line));
					symbolList = addSymbolNode(symbolList, symbolPointer);
					symbolCounter++;
					}
				}
			} else if (line[0] == '.'){
				if (strcmp(getDotInstruction(line+1),"entry") == 0) {
					printf("%d: %s (entry found)\n",*count,line);
				} else if (strcmp(getDotInstruction(line+1),"extern") == 0) {
					printf("%d: %s (extern found)\n",*count,line);
				} else {
					printf("%d: %s (unknown instruction line, ignoring)\n",*count,line);
				}
			} else {
					printf("%d: %s (no symbols found)\n",*count,line);
			}
		}
	}
}

void strip_extra_spaces(char* str) {
  int i,x;
  char newlineChar = '\n';
  for(i=x=0; str[i]; ++i) {
    if(!isspace(str[i]) || (i>0 && !isspace(str[i-1])))
    	str[x++] = str[i];
    if (str[i+1] == '\t')
    	str[i+1] = ' ';
  }

  if (strchr(str,newlineChar) != NULL) {
	  str[x-1] = '\0';
  } else {
	  str[x] = '\0';
  }
}

int hasSymbol(char* str) {
	char *symbolPos = strchr(str, symbolChar);
	int tmpLen;

	if (symbolPos == NULL)
		return 0;
	tmpLen = strlen(str)-strlen(symbolPos);
	if (!tmpLen)
		tmpLen+=1;

	symbolLen=tmpLen;
	return tmpLen;
}
char *hasDot(char* str) {
	char *tmpStr;
	tmpStr = strchr(str,dotChar);

	return tmpStr;
}
int extractData(char* str) {
	char *symbolPos = strchr(str, symbolChar);
	int tmpLen;

	if (symbolPos == NULL)
		return 0;
	tmpLen = strlen(str)-strlen(symbolPos);
	if (!tmpLen)
		tmpLen+=1;

	symbolLen=tmpLen;
	return tmpLen;
}
char *getDotInstruction(char* str) {
	char *dotPos = strchr(str, spaceChar);
	char *tmp = malloc(sizeof(str));
	int tmpLen;

	if (dotPos == NULL)
		return "";
	tmpLen = strlen(str)-strlen(dotPos);
	if (!tmpLen)
		tmpLen+=1;

	dotLen=tmpLen;
	memcpy(tmp,str,tmpLen+1);
	tmp[dotLen] = '\0';
	return tmp;
}
int extractString(char* str) {
	char *symbolPos = strchr(str, symbolChar);
	int tmpLen;

	if (symbolPos == NULL)
		return 0;
	tmpLen = strlen(str)-strlen(symbolPos);
	if (!tmpLen)
		tmpLen+=1;

	symbolLen=tmpLen;
	return tmpLen;
}
char *getSymbol(char* str, int pos) {
	str[pos] = '\0';
	return str;
}
mySymbolList *createSymbolNode (char* str) {
	mySymbolList* newSymbol = malloc(sizeof(mySymbolList));
	if (NULL != newSymbol){
		newSymbol->Sym = str;
		newSymbol->next = NULL;
	}
	return newSymbol;
}
mySymbolList *addSymbolNode (mySymbolList* symbols, char* str) {
	mySymbolList* newSymbol = createSymbolNode(str);
	if (NULL != newSymbol) {
			newSymbol->next = symbols;
	}
	return newSymbol;
}
