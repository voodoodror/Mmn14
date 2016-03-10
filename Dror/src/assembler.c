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
    const char commaChar = 44;
    int symbolLen = 0;
    int dotLen = 0;
    int symbolCounter = 0;
    int errorFlag = 0;
    int dc=0, ic=0;
    mySymbolList* symbolList;
    myHashTable *hashTable[1000];
    myCommandTable commandTable[16];
    myDataTable *dataTable[1000];

int main(int argc, char **argv)
{
    int i;
    FILE *fp;
    char file_name[20];
    char *lineToProcess = malloc(sizeof(char) * BUF_SIZE);
    init_command_table();

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
        printf("\n\nSymbol Table:\n");
		mySymbolList* iter;
		for (iter = symbolList; NULL != iter; iter = iter->next)
						printf("NAME: \"%s\"\tADDR: \"%d\"\tEXTERNAL: \"%d\"\tACTION: \"%d\"\n",iter->Sym,iter->addr,iter->ext,iter->action);
    }
    return 0;
}
void init_hash_table(myHashTable *il)
{
	il->addr = 0;
    il->era = 0;
    il->dest_reg = 0;
    il->dest_addr = 0;
    il->src_reg = 0;
    il->src_addr = 0;
    il->opcode = 0;
    il->group = 0;
    il->rnd = 0;
    il->not_in_use = 0;
}
void init_command_table()
{
	commandTable[0].command = "mov",commandTable[0].srcOperations=1,commandTable[0].destOperations=1;
	commandTable[1].command = "cmp",commandTable[1].srcOperations=1,commandTable[1].destOperations=1;
	commandTable[2].command = "add",commandTable[2].srcOperations=1,commandTable[2].destOperations=1;
	commandTable[3].command = "sub",commandTable[3].srcOperations=1,commandTable[3].destOperations=1;
	commandTable[4].command = "not",commandTable[4].srcOperations=0,commandTable[4].destOperations=1;
	commandTable[5].command = "clr",commandTable[5].srcOperations=0,commandTable[5].destOperations=1;
	commandTable[6].command = "lea",commandTable[6].srcOperations=1,commandTable[6].destOperations=1;
	commandTable[7].command = "inc",commandTable[7].srcOperations=0,commandTable[7].destOperations=1;
	commandTable[8].command = "dec",commandTable[8].srcOperations=0,commandTable[8].destOperations=1;
	commandTable[9].command = "jmp",commandTable[9].srcOperations=0,commandTable[9].destOperations=1;
	commandTable[10].command = "bne",commandTable[10].srcOperations=0,commandTable[10].destOperations=1;
	commandTable[11].command = "red",commandTable[11].srcOperations=0,commandTable[11].destOperations=1;
	commandTable[12].command = "prn",commandTable[12].srcOperations=0,commandTable[12].destOperations=1;
	commandTable[13].command = "jsr",commandTable[13].srcOperations=0,commandTable[13].destOperations=1;
	commandTable[14].command = "rts",commandTable[14].srcOperations=0,commandTable[14].destOperations=0;
	commandTable[15].command = "top",commandTable[15].srcOperations=0,commandTable[15].destOperations=0;
}
void first_parsing_line (char *line, int *count) {
	int i=0, dupSymbol=0, symbolFound=0, commandFound=0;

	char *symbolPointer;
	char *stringFound;
	char *dotCommand;
	char tmp[80];
	char *temp;
	void *test;

	strip_extra_spaces(line);
	if (line[0] == ';') {
		printf("%d: %s (comment line, ignoring)\n",*count,line);
	} else {
		if (!isalpha(line[0]) && line[0] != '.') {
				printf("%d: (must begin with a letter OR a DOT!)\n",*count);
		} else {
			if (hasSymbol(line) != 0) {
				if (hasSymbol(line+(symbolLen+sizeof(symbolChar))) != 0) {
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
						if (hasDot(line+(symbolLen+sizeof(symbolChar)+sizeof(spaceChar))) != NULL) {
							dotCommand = getNextString(line+(symbolLen+sizeof(symbolChar)+sizeof(spaceChar)+sizeof(dotChar)));
							if (strcmp(dotCommand,"string") == 0) {
								/*line = line+(symbolLen+sizeof(symbolChar)+sizeof(spaceChar)+sizeof(dotChar)+sizeof(dotCommand)+sizeof(spaceChar));
								extractData(line,dotCommand);
								while (tmp != NULL) {
									printf("%s\n",tmp);
									temp = strtok(NULL,qmChar);
								}
								if (line[symbolLen+3+dotLen+1] == '\"' && hasQM(line+(symbolLen+3+dotLen+1)) != NULL) {
									stringFound = extractString(line+(symbolLen+3+dotLen+1));
									printf("%d: %s (string found, it is: \"%s\")\n",*count,line,stringFound);
									} else {
										printf("%d: %s (string found but does not end properly)\n",*count,line);
									}*/
								printf("%d: %s (string found)\n",*count,line);
							} else if (strcmp(dotCommand,"data") == 0) {
								extractData(line,"data");
								printf("%d: %s (data found)\n",*count,line);
							} else {
								printf("%d: %s (unknown instruction line, ignoring)\n",*count,line);
							}
						} else {
							printf("%d: %s (NEW symbol found)\n",*count,line);
							symbolPointer = getSymbol(line,hasSymbol(line));
							if (symbolCounter == 0)
								symbolList = createSymbolNode(symbolPointer,100,0,0);
							else
								symbolList = addSymbolNode(symbolList,symbolPointer,100,0,0);
							symbolCounter++;
							dotCommand = getNextString(line+(symbolLen+sizeof(symbolChar)+sizeof(spaceChar)));
							for (i=0; i<15; i++) {
								if (strcmp(getNextString(line+(symbolLen+sizeof(symbolChar)+sizeof(spaceChar))),commandTable[i].command)) {
									commandFound=1;
								}
							}
							if (commandFound) {
								printf("%d: %s (\"%s\" command WITH symbol found)\n",*count,line,getNextString(line+(symbolLen+2)));
							} else {
								printf("%d: %s (command not found)\n",*count,line);
							}
						}
					}
				}
			} else if (line[0] == '.'){
				symbolFound=0;
				dotCommand = getNextString(line+sizeof(dotChar));
				if (strcmp(dotCommand,"entry") == 0) {
					printf("%d: %s (entry found)\n",*count,line);
				} else if (strcmp(dotCommand,"extern") == 0) {
					symbolPointer = getNextString(line+(sizeof(spaceChar)+strlen(dotCommand)+sizeof(spaceChar)));
					if (symbolCounter == 0)
						symbolList = createSymbolNode(symbolPointer,0,1,0);
					else
						symbolList = addSymbolNode(symbolList,symbolPointer,0,1,0);
					symbolCounter++;
					printf("%d: %s (extern found)\n",*count,line);
				}
				else {
					printf("%d: %s (unknown instruction line, ignoring)\n",*count,line);
				}
			} else {
				for (i=0; i<15; i++) {
					if (strcmp(getNextString(line),commandTable[i].command)) {
						commandFound=1;
					}
				}
				if (commandFound) {
					printf("%d: %s (\"%s\" command found)\n",*count,line,getNextString(line));
				} else {
					printf("%d: %s (command not found)\n",*count,line);
				}
			}
		}
	}
}

void strip_extra_spaces(char* str) {
  int i,x;
  char newlineChar = '\n';
  for(i=x=0; str[i]; ++i) {
	  /*if (isspace(str[i]) && (str[i+1] == 44 || str[i-1] == 44)) {
		  	  i++;
	  	  }
	  else*/ if(!isspace(str[i]) || (i>0 && !isspace(str[i-1])))
		  str[x++] = str[i];
	  else if (str[i+1] == '\t')
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
void extractData(char *str, char *type) {
	/*char *dataPos;
	int tmpLen;
	char *tmp = malloc(sizeof(str));

	if (strcmp(type,"data") == 0) {
		dataPos = strchr(str, commaChar);

		while (dataPos != NULL) {
			dataPos = strchr(str,commaChar);
			tmpLen = strlen(str)-strlen(dataPos);
			memcpy(tmp,str,tmpLen+1);
			tmp[dotLen] = '\0';
		}
	} else if (strcmp(type,"string") == 0) {
	}*/
}
char *getNextString(char* str) {
	char *dotPos = strchr(str, spaceChar);
	char *tmp = malloc(sizeof(str));
	int tmpLen;

	if (dotPos == NULL)
		return str;
	tmpLen = strlen(str)-strlen(dotPos);
	if (!tmpLen)
		tmpLen+=1;

	dotLen=tmpLen;
	memcpy(tmp,str,tmpLen+1);
	tmp[dotLen] = '\0';
	return tmp;
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
