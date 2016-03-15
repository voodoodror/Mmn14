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
const char newlineChar = '\n';
const char commaChar = 44; /* , char */
int symbolLen = 0;
int dotLen = 0;
int symbolCounter = 0;
int errorFlag = 0;
int dc=0, ic=0;
int count;
mySymbolList *symbolList;
myHashTable *hashTable[MAX_ARRAY_SIZE];
myCommandTable commandTable[COMMAND_SIZE];
myDataTable *dataTable;
int addressingTable[82] = { 1000, 1001, 10021, 10022, 10023, 1003, 1011, 10121, 10122, 10123, 10123, 1013, 1100, 1101, 11011, 110121, 110122, 110123, 11013, 11021, 11022, 11023, 1103, 1110, 1111, 11111, 111121, 111122, 111123, 11113, 11121, 11122, 11123, 1113, 11210, 11211, 112121, 112122, 112123, 11213, 11311, 113123, 1200, 1201, 12021, 12022, 12023, 1203, 1211, 12121, 12122, 12123, 12123, 1213, 1300, 1301, 13021, 13022, 13023, 1303, 1311, 13121, 13122, 13123, 1313, 1411, 1413, 1511, 1513, 1601, 16023, 1611, 1613, 1711, 1713, 1811, 1813, 1911, 19121, 19122, 19123, 1913 };

int main(int argc, char **argv)
{

    int i;

    FILE *fp;
    char file_name[20];
    char *lineToProcess = malloc(sizeof(char) * BUF_SIZE);

    init_command_table();

    for (i = 1; i < argc; i++)
    {
        sprintf(file_name, "%s", argv[i]);
        fp = fopen(file_name, "r");

        if (fp == NULL)
        {
            printf("Error: can't find file %s\n", file_name);
            return -1;
        }

        count=0;

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

		printf("\n\nData Table:\n");
		myDataTable* iterd;
		for (iterd = dataTable; NULL != iterd; iterd = iterd->next)
			printf("DC: \"%d\"\tDATA: \"%u\"\n",iterd->dc,iterd->data);
    }
    return 0;
}
void init_hash_table(myHashTable *il)
{
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
	commandTable[15].command = "stop",commandTable[15].srcOperations=0,commandTable[15].destOperations=0;
}
void first_parsing_line (char *line, int *count) {
	int dupSymbol=0, symbolFound=0, commandFound=0, tmp=0;

	char *symbolPointer;
	char *dotCommand;
	char extractResult;

	strip_extra_spaces(line);
	if (line[0] == ';') {
		printf("%d: %s (comment line, ignoring)\n",*count,line);
	} else if (strlen(line) == 0){
		printf("%d: Empty line found, ignoring\n",*count);
	} else {
		if (!isalpha(line[0]) && line[0] != '.') {
			printf("%d: (line MUST begin with a letter OR a DOT!)\n",*count);
		} else {
			if (hasSymbol(line) != 0) {
				if (hasSymbol(line+(symbolLen+sizeof(symbolChar))) != 0) {
					printf("%d: %s (Syntax Error! multiple symbol signs found!)\n",*count,line);
				} else {
					if (symbolCounter!=0) {
						symbolPointer = getSymbol(line,hasSymbol(line));
						dupSymbol = findDuplicateSym(symbolList,symbolPointer);
					}
					if (!dupSymbol) {
						symbolFound=1;
						if (hasDot(line+(symbolLen+sizeof(symbolChar)+sizeof(spaceChar))) != NULL) {
							symbolPointer = getSymbol(line,hasSymbol(line));
							dotCommand = getNextString(line+(symbolLen+sizeof(symbolChar)+sizeof(spaceChar)+sizeof(dotChar)));
							if (strcmp(dotCommand,"string") == 0) {
								tmp = dc;
								extractResult = extractSymData("string");
							} else if (strcmp(dotCommand,"data") == 0) {
								tmp = dc;
								extractResult = extractSymData("data");
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
							commandFound = findCommand(dotCommand);
							if (commandFound!=-1) {
								extractResult = extractSym(commandFound);
								if (extractResult)
									printf("%d: %s (\"%s\")\n",*count,line,dotCommand);
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
				} else {
					printf("%d: %s (unknown instruction line, ignoring)\n",*count,line);
				}
			} else {
				symbolFound=0;
				dotCommand = getNextString(line);
				commandFound = findCommand(dotCommand);
				if (commandFound!=-1) {
					extractResult = extractOperands(line+(sizeof(spaceChar)+strlen((char *)dotCommand)),commandFound);
					if (extractResult)
						printf("%d: %s command found: (\"%s\")\n",*count,line,dotCommand);
				} else {
					printf("%d: %s (command not found)\n",*count,line);
				}
			}
		}
	}
}
int findDuplicateSym(mySymbolList *symbolList,char *sym) {
	mySymbolList* iter;
	for (iter = symbolList; NULL != iter; iter = iter->next) {
		if(strcmp(iter->Sym,sym) == 0) {
			printf("Duplicate symbol found!\n");
			return 1;
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
void strip_extra_spaces(char* str) {
  int i,x;

  for(i=x=0; str[i]; ++i) {
	  if (str[i+1] == '\t')
	  		  str[i+1] = ' ';
	  if (isspace(str[i]) && !isalpha(str[i-1]) && (str[i-1] != symbolChar) && (!isdigit(str[i+1]) || !isdigit(str[i-1]) || str[i+1] =='-' || str[i-1] == qmChar))
		  	  i++;
	  if(!isspace(str[i]) || (i>0 && !isspace(str[i-1])))
		  str[x++] = str[i];
  }
  if (isspace(str[x]))
	  str[x] = '\0';
  if (strchr(str,newlineChar) != NULL)
	  str[x] = '\0';
  else
	  str[x] = '\0';
  if (isspace(str[strlen((char *)str)-1]))
  	  str[strlen((char *)str)-1] = '\0';
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
int extractData(char *str, char *type) {

	char rwString[BUF_SIZE];
	char *rwPointer;
	strcpy(rwString,str);
	char *token;

	rwPointer = rwString;
	if (str == NULL) {
		printf("No data found in string");
		return 0;
	}
	if (strcmp(type,"data") == 0) {

		token = strsep(&rwPointer,",");
		if (token == NULL) {
			printf("%d: Error occurred while trying to parse data field.\n1. Please make sure you don\'t have comma at the beginning or end of the string.\n2. Don't put 2 commas side by side.",count);
			return 0;
		}
		while (token != NULL) {
			if ((strlen(token)<=1) && (!isdigit(token[0]))) {
				printf("%d: Syntax Error: Empty or invalid char found.\n",count);
				return 0;
			}
			token = strsep(&rwPointer,",");
		}
		strcpy(rwString,str);
		rwPointer = rwString;
		token = strsep(&rwPointer,",");

		while (token != NULL) {
			if (dc == 0) {
				dataTable = createDataNode(dc++,(int)*token);
			} else {
				dataTable = addDataNode(dataTable,dc++,(int)*token);
			}
			/*printf("TOKEN: %s\n", token);*/

			token = strsep(&rwPointer,",");
		}
		return 1;

		} else if (strcmp(type,"string") == 0) {
			int qmFound=0;
			token = str;
			while (token != NULL) {
				token = hasQM(token+1);
				qmFound++;
			}
			if (qmFound==2) {
				str = str+1;

				str[strlen(str)] = '\0';

				while (str[0]!='\0') {
					if (dc == 0) {
						dataTable = createDataNode(dc++,(int)str[0]);
						str++;
					} else {
						dataTable = addDataNode(dataTable,dc++,(int)str[0]);
						str++;
					}
				}
				return 1;
			} else if (qmFound<=2) {
				printf("%d: Syntax Error: No quotation marks found for string!\n",count);
				return 0;
			} else if (qmFound>2) {
				printf("%d: Syntax Error: Too much quotation marks has been found!\n",count);
				return 0;
			}
		}
	return 0;
}
int extractOperands(char *str, int opcode) {
	char rwString[BUF_SIZE];
	char *rwPointer;
	strcpy(rwString,str);
	char *token;
	int i=0,singleOperand=0,noOperands=0,srcAddr=-1,destAddr=-1;
	rwPointer = rwString;

	token = strchr(str,commaChar);
	if (opcode==14 || opcode==15) {
		if (token!=NULL) {
			printf("Syntax Error! You can't use operands in this command!\n");
			return 0;
		}
		noOperands=1;
	}
	if (token == NULL && !noOperands) {
		if (str != NULL && commandTable[opcode].srcOperations==0 && commandTable[opcode].destOperations==1) {
			singleOperand=1;
		} else {
			printf("%d: Invalid operand for %s\n",count,commandTable[opcode].command);
			return 0;
		}
	}

	token = strsep(&rwPointer,",");

	while (token != NULL && !singleOperand && !noOperands) {
		if (strcmp(token,"\0")==0) {
			printf("%d: Syntax Error: Empty or invalid operand found.\n",count);
			return 0;
		}
		token = strsep(&rwPointer,",");
		i++;
	}
	if (i==2 && !singleOperand && !noOperands) {
		strcpy(rwString,str);
		rwPointer = rwString;

		token = strsep(&rwPointer,",");
		srcAddr = recognizeOperand(token);

		token = strsep(&rwPointer,",");
		destAddr = recognizeOperand(token);

		if (srcAddr!=-1 && destAddr!=-1) {
			if(validOperOpcode(opcode,srcAddr,destAddr)) {
				printf("GOOD TO GO!\n");
				return 1;
			} else {
				printf("Invalid operands received for this command.\n");
				return 0;
			}
		}



		return 1;
	} else if (singleOperand) {
		destAddr = recognizeOperand(token);
		if (destAddr!=-1) {
			printf("%d: destAddr: %d\n",count,destAddr);
			if (validOperOpcode(opcode,-1,destAddr)) {
				return 1;
			} else {
				printf("Invalid operands received for this command.\n");
				return 0;
			}
		}
		return 0;
	} else if (noOperands) {
		printf("%d: STOP or RTS command found!\n",count);
		return 1;
	} else {
		printf("SYNTAX ERROR:\n1. Do not place 2 commas side by side\n2. Please make sure you do not exceed 2 operands limit.\n\n");
		return 0;
	}
}
int recognizeOperand(char *str) {
	unsigned int tempNum;
	int invalidResult=-1;

	if (str[0] == 'r') {
		if (strlen(str)==2) {
			str=str+1;
			tempNum = atoi(str);
			if (tempNum>=0 && tempNum<=7) {
				return 3;
			} else {
				printf("Invalid register number.\n");
				return invalidResult;
			}
		} else {
			printf("Too LESS or MUCH chars for register operand.\n");
			return invalidResult;
		}
	} else if (str[0] == '#') {
		if (str[1] == '-') {
			str = str+2;
			tempNum = atoi(str);
			if (tempNum<=16385 && tempNum>0) {
				return 0;
			} else {
				printf("Invalid number entered.\n");
				return invalidResult;
			}
		} else {
			str = str+2;
			tempNum = atoi(str);
			if (tempNum<=16383 && tempNum>0) {
				return 0;
			} else {
				printf("Invalid number entered.\n");
				return invalidResult;
			}
		}
	} else if (strcmp(str,"*")==0) {
		return 21;
	} else if (strcmp(str,"**")==0) {
		return 22;
	} else if (strcmp(str,"***")==0) {
		return 23;
	} else {
		while (str!=NULL) {
			if (str[0]>=65 && str[0]<=90)
				return 1;
			str=str+1;
		}
		printf("Invalid operand data entered.\n");
		return invalidResult;
	}
}
int validOperOpcode(int opcode, int srcAddr, int destAddr) {
	int i=0, j=0, x=0, numSrc=0, numDest=0, foundSrc=0, foundDest=0;
	int addressingTableLen = sizeof(addressingTable)/sizeof(addressingTable[0]);
	char addressingSrc[6] = { 0 };
	char addressingDest[6] = { 0 };
	int digit;
	int tmp=0;

	addressingSrc[i++]=1, addressingDest[j++]=1;

	if (opcode>9) {
		for (x=0; x<2; x++) {
			digit = opcode % 10;
			opcode /=10;
			addressingSrc[i++] = digit;
			addressingDest[j++] = digit;
		}
		reverseDigits();
	} else {
		addressingSrc[i++] = opcode;
		addressingDest[j++] = opcode;
	}

	addressingSrc[i++]=0, addressingDest[j++]=1;

	if (srcAddr!=-1 && destAddr!=-1) {
		if (srcAddr>9) {
			for (x=0; x<2; x++) {
				digit = srcAddr % 10;
				srcAddr /=10;
				addressingSrc[i++] = digit;
			}
			reverseDigits();
		} else {
			addressingSrc[i++] = srcAddr;
		}
		if (destAddr>9) {
			for (x=0; x<2; x++) {
				digit = destAddr % 10;
				destAddr /=10;
				addressingDest[j++] = digit;
			}
			reverseDigits();
		} else {
			addressingDest[j++] = destAddr;
		}

		for (x = 0; x < i; x++)
			numSrc = 10 * numSrc + addressingSrc[x];
		for (x = 0; x < j; x++)
			numDest = 10 * numDest + addressingDest[x];

		for (i=0; i<=addressingTableLen; i++) {
			if (addressingTable[i]==numSrc)
				foundSrc=1;
			if (addressingTable[i]==numDest)
				foundDest=1;
			if (foundSrc && foundDest)
				return 1;
		}
	} else if (srcAddr==-1 && destAddr!=-1) {
		if (destAddr>9) {
			for (x=0; x<2; x++) {
				digit = destAddr % 10;
				destAddr /=10;
				addressingDest[j++] = digit;
			}
			reverseDigits();
		} else {
			addressingDest[j++] = destAddr;
		}
		for (x = 0; x < j; x++)
			numDest = 10 * numDest + addressingDest[x];
		for (i=0; i<=addressingTableLen; i++) {
			if (addressingTable[i]==numDest)
				foundDest=1;
			if (foundDest)
				return 1;
		}
	}
	return 0;
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
myDataTable *createDataNode (int dc, unsigned int data) {
	myDataTable* newData = malloc(sizeof(myDataTable));

	if (NULL != newData){
		newData->dc = dc;
		newData->data = data;
		newData->next = NULL;
	}
	return newData;
}
myDataTable *addDataNode (myDataTable* dataTable, int dc, unsigned int data) {
	myDataTable* newData = createDataNode(dc,data);
	myDataTable *firstpos = dataTable;
	if (newData != NULL) {
			while (dataTable->next != NULL) {
				dataTable = dataTable->next;
			}
			dataTable->next = newData;
	}
	return firstpos;
}
