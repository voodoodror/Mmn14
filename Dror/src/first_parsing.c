#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "struct.h"
#include "prerequisites.h"
#include "utils.h"
#include "first_parsing.h"
#include "main.h"

extern int dc, ic, errorFlag,symbolLen,symbolCounter,count;
extern mySymbolList *symbolList;
extern myDataTable *dataTable;
extern myCommandTable commandTable[COMMAND_SIZE];
extern int addressingTable[82];
extern const char symbolChar;
extern const char dotChar;
extern const char spaceChar;
extern const char commaChar;

void first_parsing_line (char *line, int count) {
	int dupSymbol=0, commandFound=-1, tmp=0;

	char *symbolPointer;
	char *dotCommand;
	int extractResult;

	strip_extra_spaces(line);
	/* Checking whether line is not empty and has no comment char */
	if (line[0] != ';' && strlen(line) != 0) {
		/* Line should start with either dot or an alpha */
		if (!isalpha(line[0]) && line[0] != '.') {
			printf("ERROR: Line %d - line MUST begin with a letter or a dot.\n",count);
			errorFlag++;
		} else {
			/* If it has symbol ':' char in string */
			if (hasSymbol(line) != 0) {
				if (hasSymbol(line+(symbolLen+sizeof(symbolChar))) != 0) {
					printf("ERROR: Line %d - More than 1 symbol sign has been found.\n",count);
					errorFlag++;
				} else if (!symIsUpper(getSymbol(line,hasSymbol(line)))) {
					printf("ERROR: Line %d - Symbol has other chars than UPPERCASE.\n",count);
					errorFlag++;
				} else {
					if (symbolCounter!=0) {
						symbolPointer = getSymbol(line,hasSymbol(line));
						dupSymbol = findExistingSym(symbolList,symbolPointer,"symbol");
					}
					if (!dupSymbol) {
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
								printf("ERROR: Line %d - Instruction line doesn't exist. Make you write everything in lowercase.\n",count);
								errorFlag++;
							}
						} else {
							dotCommand = getNextString(line+(symbolLen+sizeof(symbolChar)+sizeof(spaceChar)));
							commandFound = findCommand(dotCommand);
							if (commandFound!=-1) {
								extractResult = extractSym(commandFound,1);
								if (extractResult) {
									ic+=extractResult;
									symbolPointer = getSymbol(line,hasSymbol(line));
									if (symbolCounter == 0)
										symbolList = createSymbolNode(symbolPointer,ic-extractResult,0,1);
									else
										symbolList = addSymbolNode(symbolList,symbolPointer,ic-extractResult,0,1);
									symbolCounter++;
									/* DEBUG:
									 * printf("%d: %s (\"%s\")\n",count,line,dotCommand);*/
								}
							} else {
								printf("ERROR: Line %d - Command doesn't exist.\n",count);
								errorFlag++;
							}
						}
					} else {
						printf("ERROR: Line %d - Duplicate Symbol Detected.\n",count);
						errorFlag++;
					}
				}
			} else if (line[0] == '.'){
				dotCommand = getNextString(line+sizeof(dotChar));
				if (strcmp(dotCommand,"entry") == 0) {
					/* DEBUG
					printf("%d: %s (entry found, ignoring in first parsing)\n",count,line);*/
				} else if (strcmp(dotCommand,"extern") == 0) {
					symbolPointer = getNextString(line+(sizeof(spaceChar)+strlen(dotCommand)+sizeof(spaceChar)));
					dupSymbol = findExistingSym(symbolList,symbolPointer,"symbol");
					if (!dupSymbol) {
						if (symbolCounter == 0)
							symbolList = createSymbolNode(symbolPointer,0,1,0);
						else
							symbolList = addSymbolNode(symbolList,symbolPointer,0,1,0);
						symbolCounter++;
						/* DEBUG
						printf("%d: %s (extern found)\n",count,line);*/
					} else {
						printf("ERROR: Line %d - Duplicate Symbol Detected.\n",count);
						errorFlag++;
					}
				} else {
					printf("ERROR: Line %d - Instruction line doesn't exist.\n",count);
					errorFlag++;
				}
			} else {
				dotCommand = getNextString(line);
				commandFound = findCommand(dotCommand);
				if (commandFound!=-1) {
					extractResult = extractOperands(line+(sizeof(spaceChar)+strlen((char *)dotCommand)),commandFound,1);
					if (extractResult)
						/* DEBUG
						printf("%d: %s command found: (\"%s\")\n",count,line,dotCommand); */
						ic+=extractResult;
				} else {
					printf("ERROR: Line %d - Command doesn't exist.\n",count);
					errorFlag++;
				}
			}
		}
	}
}
int extractData(char *str, char *type) {

	char rwString[BUF_SIZE];
	char *rwPointer;
	strcpy(rwString,str);
	char *token;
	int num;

	rwPointer = rwString;
	if (str == NULL) {
		printf("ERROR: Line %d - No data has been found in string.\n",count);
		errorFlag++;
		return 0;
	}
	if (strcmp(type,"data") == 0) {

		token = strsep(&rwPointer,",");
		if (token == NULL) {
			printf("ERROR: Line %d - Error occurred while trying to parse data field.\n1. Please make sure you don\'t have comma at the beginning or end of the string.\n2. Don't put 2 commas side by side.\n",count);
			errorFlag++;
			return 0;
		}
		while (token != NULL) {
			if ((strlen(token)<=1) && (!isdigit(token[0]))) {
				printf("ERROR: Line %d - Empty or invalid char found.\n",count);
				errorFlag++;
				return 0;
			}
			token = strsep(&rwPointer,",");
		}
		strcpy(rwString,str);
		rwPointer = rwString;
		token = strsep(&rwPointer,",");

		while (token != NULL) {
			num = atoi(token);
			if (dc == 0) {
				dataTable = createDataNode(dc++,num);
			} else {
				dataTable = addDataNode(dataTable,dc++,num);
			}

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

				str[strlen(str)-1] = '\0';

				while (str[0]!='\0') {
					if (dc == 0) {
						dataTable = createDataNode(dc++,(int)str[0]);
						str++;
					} else {
						dataTable = addDataNode(dataTable,dc++,(int)str[0]);
						str++;
					}
				}
				dataTable = addDataNode(dataTable,dc++,0);
				return 1;
			} else if (qmFound<=2) {
				printf("ERROR: Line %d - No quotation marks found for string!\n",count);
				errorFlag++;
				return 0;
			} else if (qmFound>2) {
				printf("ERROR: Line %d - More than 1 quotation mark has been found!\n",count);
				errorFlag++;
				return 0;
			}
		}
	return 0;
}
int extractOperands(char *str, int opcode, int phase) {
	char rwString[BUF_SIZE];
	char *rwPointer;
	strcpy(rwString,str);
	char *token, *srcAddrValue, *destAddrValue;
	int i=0,singleOperand=0,noOperands=0,srcAddr=-1,destAddr=-1,validateSuccess=0;
	rwPointer = rwString;

	token = strchr(str,commaChar);
	if (opcode==14 || opcode==15) {
		if (token!=NULL) {
			printf("ERROR: Line %d - You can't use operands with this command!\n",count);
			errorFlag++;
			return 0;
		}
		noOperands=1;
	}
	if (token == NULL && !noOperands) {
		if (str != NULL && commandTable[opcode].srcOperations==0 && commandTable[opcode].destOperations==1) {
			singleOperand=1;
		} else {
			printf("ERROR: Line %d - Invalid addressing for this operand.\n",count);
			errorFlag++;
			return 0;
		}
	}

	token = strsep(&rwPointer,",");

	while (token != NULL && !singleOperand && !noOperands) {
		if (strcmp(token,"\0")==0) {
			printf("ERROR: Line %d - Empty or invalid operand has been found.\n",count);
			errorFlag++;
			return 0;
		}
		token = strsep(&rwPointer,",");
		i++;
	}
	if (i==2 && !singleOperand && !noOperands) {
		strcpy(rwString,str);
		rwPointer = rwString;

		token = strsep(&rwPointer,",");
		srcAddrValue = token;
		srcAddr = recognizeOperand(token);

		token = strsep(&rwPointer,",");
		destAddrValue = token;
		destAddr = recognizeOperand(token);

		if (srcAddr!=-1 && destAddr!=-1) {
			if (phase==2) {
				insertToDataTable(opcode,srcAddr,destAddr,srcAddrValue,destAddrValue);
				return 2;
			} else {
				validateSuccess=validOperOpcode(opcode,srcAddr,destAddr);
			}
			if(validateSuccess!=0) {
				return validateSuccess;
			} else {
				printf("ERROR: Line %d - Invalid operands received for this command.\n",count);
				errorFlag++;
				return validateSuccess;
			}
		}
		return 1;

	} else if (singleOperand) {
		destAddrValue = token;
		destAddr = recognizeOperand(token);
		if (destAddr!=-1) {
			if (phase==2) {
				insertToDataTable(opcode,-1,destAddr,NULL,destAddrValue);
				return 2;
			} else {
				validateSuccess=validOperOpcode(opcode,-1,destAddr);
			}
			if (validateSuccess!=0) {
				return validateSuccess;
			} else {
				printf("ERROR: Line %d - Invalid operands received for this command.\n",count);
				errorFlag++;
				return 0;
			}
		}
		return 0;
	} else if (noOperands) {
		if (phase==2) {
			insertToDataTable(opcode,-1,-1,NULL,NULL);
		}
		return 1;
	} else {
		printf("ERROR: Line %d - Do not place 2 commas side by side\n2. Please make sure you do not exceed 2 operands limit.\n",count);
		errorFlag++;
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
				printf("ERROR: Line %d - Invalid register number in addressing.\n",count);
				errorFlag++;
				return invalidResult;
			}
		} else {
			printf("ERROR: Line %d - Too LESS or MUCH chars for register operand.\n",count);
			errorFlag++;
			return invalidResult;
		}
	} else if (str[0] == '#') {
		if (str[1] == '-') {
			str = str+2;
			tempNum = atoi(str);
			if (tempNum<=16384 && tempNum>0) {
				return 0;
			} else {
				printf("ERROR: Line %d - Invalid number entered.\n",count);
				errorFlag++;
				return invalidResult;
			}
		} else {
			str = str+2;
			tempNum = atoi(str);
			if (tempNum<=16383 && tempNum>0) {
				return 0;
			} else {
				printf("ERROR: Line %d - Invalid number entered.\n",count);
				errorFlag++;
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
		while (strlen(str)!=0) {
			if (str[0]>=65 && str[0]<=90)
				return 1;
			str=str+1;
		}
		printf("ERROR: Line %d - Invalid data operand entered.\n",count);
		errorFlag++;
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
			if (foundSrc && foundDest) {
				if (srcAddr==3 && destAddr==3) {
					return 2;
				}
				return 3;
			}
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
				return 2;
		}
	}
	return 0;
}
