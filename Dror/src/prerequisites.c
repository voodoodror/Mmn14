#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "prerequisites.h"
#include "struct.h"
#include "first_parsing.h"
#include "utils.h"
#include "main.h"

extern myCommandTable commandTable[COMMAND_SIZE];
extern int dotLen,symbolLen, ic;
extern mySymbolList *symbolList;

const char symbolChar = ':';
const char dotChar = '.';
const char spaceChar = ' ';
const char qmChar = '\"';
const char newlineChar = '\n';
const char commaChar = 44; /* , char */

char *parseProjectName(char* project) {
	char *dotPos = strchr(project, dotChar);
	char *projectName = malloc(sizeof(char*));

	int projectNameLen;

	projectNameLen = strlen(project)-strlen(dotPos);

	memcpy(projectName,project,projectNameLen);
	projectName[projectNameLen] = '\0';
	return projectName;
}
void init_command_table() {
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
	commandTable[15].command = "stop",commandTable[15].srcOperations=0,commandTable[15].destOperations=0;
}
void replaceStrAddr() {
	mySymbolList* iter;
	for (iter = symbolList; NULL != iter; iter = iter->next) {
		if(iter->ext==0 && iter->action==0) {
			iter->addr+=ic;
		}
	}
}
int findExistingSym(mySymbolList *symbolList,char *sym, char *type) {
	mySymbolList* iter;
	int totalSym=0,i=0,numberFound=0;
	for (iter = symbolList; NULL != iter; iter = iter->next) {
		if (sym!=NULL) {
			if(strcmp(iter->Sym,sym)==0) {
				if (strcmp(type,"entry") == 0) {
					if (iter->ext==0) {
						return iter->addr;
					} else {
						return -1;
					}
				} else if (strcmp(type,"symbol") == 0) {
					return 1;
				} else if (strcmp(type,"data") == 0) {
					return iter->addr;
				} else {
					return -1;
				}
			}
		}
		totalSym++;
	}
	if (strcmp(type,"random")==0) {
		while (!numberFound) {
			srand(time(NULL));
			int r = ( rand() % totalSym ) + 1;
			iter = symbolList;
			for (i=0; i<r; i++) {
				iter = iter->next;
			}
			if (iter->ext==0) {
				return iter->addr;
			}
		}
	}
	return 0;
}
int findCommand(char *command) {
	int i=0;
	while (i<COMMAND_SIZE) {
		if (strcmp(command,commandTable[i].command)==0) {
			return i;
		}
		i++;
	}
	return -1;
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
	char *myStr = malloc(sizeof(char*));
	memcpy(myStr,str,pos);
	myStr[pos] = '\0';
	return myStr;
}
