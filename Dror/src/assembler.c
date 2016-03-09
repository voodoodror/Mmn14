#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "assembler.h"

    char *lineToProcess;
    const char symbolChar = ':';
    const char dotChar = '.';
    const char spaceChar = ' ';
    const char qmChar = '\"';
    int symbolLen = 0;
    int dotLen = 0;
    int symbolCounter = 0;
    int errorFlag = 0;
    int dc=0, ic=0;
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

        while (fgets(lineToProcess, MAX_LINE, fp))
        {
        	first_parsing_line(lineToProcess, &count);
        	count++;
        }
        fclose(fp);
    }
    return 0;
}

void first_parsing_line (char *line, int *count) {
	int i=0, dupSymbol=0, symbolFound=0;

	char *symbolPointer;
	char *stringFound;

	strip_extra_spaces(line);
	if (line[0] == ';') {
		printf("%d: %s (comment line, ignoring)\n",*count,line);
	} else {
		if (!isalpha(line[0]) && line[0] != '.') {
				printf("%d: (must begin with a letter OR a DOT!)\n",*count,line);
		} else {
			if (hasSymbol(line) != 0) {
				if (hasSymbol(line+(symbolLen+1)) != 0) {
					printf("%d: %s (multiple symbols found, ignoring)\n",*count,line);
				} else {
					if (symbolCounter!=0) {
						symbolPointer = getSymbol(line,hasSymbol(line));
						mySymbolList* iter;
						for (iter = symbolList; NULL != iter; iter = iter->next) {
								if(strcmp(iter->Sym,symbolPointer) == 0) {
										dupSymbol = 1;
										printf("%d: %s (duplicate symbol found, ignoring... addr? %u action? %i ext? %i)\n",*count,line,iter->addr,iter->action,iter->ext);
								}
						}
					}
					if (!dupSymbol) {
						symbolFound=1;
						if (hasDot(line+(symbolLen+2)) != NULL) {
							if (strcmp(getDotInstruction(line+(symbolLen+3)),"string") == 0) {
								/*tmp = strtok(line+(symbolLen+3+dotLen+1),qmChar);
								while (tmp != NULL) {
									printf("%s\n",tmp);
									tmp = strtok(NULL,qmChar);
								}
								if (line[symbolLen+3+dotLen+1] == '\"' && hasQM(line+(symbolLen+3+dotLen+1)) != NULL) {
									stringFound = extractString(line+(symbolLen+3+dotLen+1));
									printf("%d: %s (string found, it is: \"%s\")\n",*count,line,stringFound);
									} else {
										printf("%d: %s (string found but does not end properly)\n",*count,line);
									}*/
								printf("%d: %s (string found)\n",*count,line);
							} else if (strcmp(getDotInstruction(line+(symbolLen+3)),"data") == 0) {
								printf("%d: %s (data found)\n",*count,line);
							} else {
								printf("%d: %s (unknown instruction line, ignoring)\n",*count,line);
							}
						}
					printf("%d: %s (NEW symbol found)\n",*count,line);
					symbolPointer = getSymbol(line,hasSymbol(line));
					if (symbolCounter == 0)
						symbolList = createSymbolNode(symbolPointer,100,0,0);
					else
						symbolList = addSymbolNode(symbolList,symbolPointer,100,0,0);
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
char *hasQM(char* str) {
	char *tmpStr;
	tmpStr = strchr(str,qmChar);

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
	char *symbolPos = strchr(str, qmChar);
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
	char *myStr;
	memcpy(myStr,str,pos);
	myStr[pos] = '\0';
	return myStr;
}
mySymbolList *createSymbolNode (char* str, unsigned int dc, int external, int action) {
	mySymbolList* newSymbol = malloc(sizeof(mySymbolList));
	char * myStr = malloc(sizeof(strlen(str)));
	memcpy(myStr,str,strlen(str));

	if (NULL != newSymbol){
		newSymbol->Sym = myStr;
		newSymbol->addr = dc;
		newSymbol->ext = external;
		newSymbol->action = action;
		newSymbol->next = NULL;
	}
	return newSymbol;
}
mySymbolList *addSymbolNode (mySymbolList* symbolList, char* str, unsigned int dc, int external, int action) {
	mySymbolList* newSymbol = createSymbolNode(str,dc,external,action);
	mySymbolList *firstpos = symbolList;
	if (newSymbol != NULL) {
			while (symbolList->next != NULL) {
				symbolList = symbolList->next;
			}
			symbolList->next = newSymbol;
	}
	return firstpos;
}
