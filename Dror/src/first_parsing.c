/* first_parsing.c
 * Dror Bletter
 * voodoodror@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "struct.h"
#include "main.h"
#include "utils.h"
#include "first_parsing.h"
#include "second_parsing.h"

void first_parsing_line (char *line, int count) {
	int dupSymbol=0, commandFound=-1, tmp=0;

	char *symbolPointer;
	char *dotCommand;
	int extractResult;

	/* Strips extra spaces, tabs */
	strip_extra_spaces(line);
	/* Checking whether line is not empty and has no comment char */
	if (line[0] != ';' && strlen(line) != 0) {
		/* Line should start with either dot or an alpha */
		if (!isalpha(line[0]) && line[0] != '.') {
			printf("ERROR: Line %d - line MUST begin with a letter or a dot.\n",count);
			errorFlag++;
		} else {
			/* Check if it has symbol ':' char in string */
			if (hasSymbol(line) != 0) {
				/* Looking for more than ':' chars in string*/
				if (hasSymbol(line+(symbolLen+strlen(symbolChar))) != 0) {
					printf("ERROR: Line %d - More than 1 symbol sign has been found.\n",count);
					errorFlag++;
					/* Symbol must be consist of UPPERCASE only */
				} else if (!symIsUpper(getSymbol(line,hasSymbol(line)))) {
					printf("ERROR: Line %d - Symbol has other chars than UPPERCASE.\n",count);
					errorFlag++;
				} else {
					/* Checking if other symbols were already added to list, if so - then it checks for duplication */
					if (symbolCounter!=0) {
						symbolPointer = getSymbol(line,hasSymbol(line));
						dupSymbol = findExistingSym(symbolList,symbolPointer,"symbol");
					}
					if (!dupSymbol) {
						/* Checking if there's dot after symbol */
						if (hasDot(line+(symbolLen+strlen(symbolChar)+strlen(spaceChar))) != NULL) {
							symbolPointer = getSymbol(line,hasSymbol(line));
							/* Isolating the instruction line, to check whether it's string\data or other invalid data */
							dotCommand = getNextString(line+(symbolLen+strlen(symbolChar)+strlen(spaceChar)+strlen(dotChar)));
							if (strcmp(dotCommand,"string") == 0) {
								/* Save DC's value before incremental */
								tmp = dc;
								/* Send the data to Macro extractSymData as string and checks if the values are valid */
								extractResult = extractSymData("string");
							} else if (strcmp(dotCommand,"data") == 0) {
								/* Save DC's value before incremental */
								tmp = dc;
								/* Send the data to Macro extractSymData as string and checks if the values are valid */
								extractResult = extractSymData("data");
							} else {
								printf("ERROR: Line %d - Instruction line doesn't exist. Make you write everything in lowercase.\n",count);
								errorFlag++;
							}
						} else {
							/* Symbol has been found, but no ".string" or ".data" present. Checking what's the command that comes after... */
							dotCommand = getNextString(line+(symbolLen+strlen(symbolChar)+strlen(spaceChar)));
							/* Take the command and verify it's existence in Command Table. If it's valid, it returns it's opcode number in decimal. */
							commandFound = findCommand(dotCommand);
							if (commandFound!=-1) {
								/* Sends the opcode number in decimal to extractSym for further processing. 1 means first parsing phase. */
								extractResult = extractSym(commandFound,1);
								if (extractResult) {
									/* Returns the amount of ic needed for this line */
									ic+=extractResult;
									symbolPointer = getSymbol(line,hasSymbol(line));
									/* Checks whether it's first symbol or not */
									if (symbolCounter == 0)
										symbolList = createSymbolNode(symbolPointer,ic-extractResult,0,1);
									else
										symbolList = addSymbolNode(symbolList,symbolPointer,ic-extractResult,0,1);
									symbolCounter++;
									/* DEBUG:
									 * printf("%d: %s (\"%s\")\n",count,line,dotCommand);*/
								}
							} else {
								printf("ERROR: Line %d - Opcode doesn't exist.\n",count);
								errorFlag++;
							}
						}
					} else {
						printf("ERROR: Line %d - Duplicate Symbol Detected.\n",count);
						errorFlag++;
					}
				}
				/* Check whether it has a dot but it belongs to extern or entry */
			} else if (line[0] == '.'){
				dotCommand = getNextString(line+strlen(dotChar));
				if (strcmp(dotCommand,"entry") == 0) {
					/* DEBUG
					printf("%d: %s (entry found, ignoring in first parsing)\n",count,line);*/
				} else if (strcmp(dotCommand,"extern") == 0) {
					symbolPointer = getNextString(line+(strlen(spaceChar)+strlen(dotCommand)+strlen(spaceChar)));
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
				/* If it doesn't have symbol AND dot, then it must be just a usual command */
				dotCommand = getNextString(line);
				/* Looking for command in command table. Returns opcode number in decimal if valid. */
				commandFound = findCommand(dotCommand);
				if (commandFound!=-1) {
					/* Parsing the opcode number along with it's values */
					extractResult = extractOperands(line+(strlen(spaceChar)+strlen(dotCommand)),commandFound,1);
					if (extractResult)
						/* DEBUG
						printf("%d: %s command found: (\"%s\")\n",count,line,dotCommand); */
						/* Returns the amount of ic needed for this line */
						ic+=extractResult;
				} else {
					printf("ERROR: Line %d - Opcode doesn't exist.\n",count);
					errorFlag++;
				}
			}
		}
	}
}
/* extractData receives string and instruction line type (string\data), validates that they're valid and has no other undesirable\extra chars */
int extractData(char *str, char *type) {

	/* Initializes char array with buffer size, it will be used with strcpy */
	char rwString[BUF_SIZE];
	/* rwPointer Will be used as string to delimit to token */
	char *rwPointer;
	/* Copies string to rwString, will be used later to re-copy string to str */
	strcpy(rwString,str);
	char *token;
	int num;

	/* *************
	 * VERIFICATIONS
	 * *************/

	rwPointer = rwString;
	/* If data is empty */
	if (str == NULL) {
		printf("ERROR: Line %d - No data has been found in string.\n",count);
		errorFlag++;
		/* return 0 => no ic will be increased */
		return 0;
	}
	/* Data type */
	if (strcmp(type,"data") == 0) {

		/* Extracts token from string and checks if it has no data */
		token = strsep(&rwPointer,commaChar);
		if (token == NULL) {
			printf("ERROR: Line %d - Error occurred while trying to parse data field.\n1. Please make sure you don\'t have comma at the beginning or end of the string.\n2. Don't put 2 commas side by side.\n",count);
			errorFlag++;
			return 0;
		}

		/* While loop to validate that there aren't any other type than numbers. Also checks for empty data. */
		while (token != NULL) {
			if ((strlen(token)<=1) && (!isdigit(token[0]))) {
				printf("ERROR: Line %d - Empty or invalid char found.\n",count);
				errorFlag++;
				return 0;
			}
			/* Check next operand after , char */
			token = strsep(&rwPointer,commaChar);
		}

		/* *************
		 * EXTRACT DATA
		 * *************/

		/* Restores original string back to rwString for parsing */
		strcpy(rwString,str);
		rwPointer = rwString;
		token = strsep(&rwPointer,commaChar);

		/* Loop until no more tokens are found */
		while (token != NULL) {
			/* Convert number from char to integer */
			num = atoi(token);
			/* Insert to Data Table */
			if (dc == 0) {
				dataTable = createDataNode(dc++,num);
			} else {
				dataTable = addDataNode(dataTable,dc++,num);
			}

			token = strsep(&rwPointer,commaChar);
		}
		/* Return successfully done parsing */
		return 1;

		/* String type */
		} else if (strcmp(type,"string") == 0) {
			/* Will be used to count how many Quotation marks (QM) has been found */
			int qmFound=0;
			token = str;
			/* Verifies that first letter is QM */
			if (str[0] != qmChar[0]) {
				printf("ERROR: Line %d - String must begin with quotation mark!\n",count);
				errorFlag++;
				return 0;
			}
			/* Counts the number of QM in string (should be 2 exactly) */
			while (token != NULL) {
				token = hasQM(token+1);
				qmFound++;
			}
			/* If exactly 2 QM are found */
			if (qmFound==2) {
				/* Strip first QM */
				str = str+1;

				/* Strip last QM */
				str[strlen(str)-1] = '\0';

				/* Adds the char to Data Table */
				while (str[0]!='\0') {
					if (dc == 0) {
						dataTable = createDataNode(dc++,(int)str[0]);
						str++;
					} else {
						dataTable = addDataNode(dataTable,dc++,(int)str[0]);
						str++;
					}
				}
				/* Adds null char to the end of string */
				dataTable = addDataNode(dataTable,dc++,0);

				/* Successfully added the Data instruction line! */
				return 1;
			/* More than 2 QM has been found... Not good! */
			} else if (qmFound<=2) {
				printf("ERROR: Line %d - No quotation marks found for string!\n",count);
				errorFlag++;
				return 0;
			/* Less than 2 QM has been found... Not good! */
			} else if (qmFound>2) {
				printf("ERROR: Line %d - More than 1 quotation mark has been found!\n",count);
				errorFlag++;
				return 0;
			}
		}
	return 0;
}

/* extractOperands receives string with the operands, opcode as decimal number and phase (1st parsing or 2nd parsing).
 * If the function proceed successfully - it returns the number of IC to increase. If not, it returns 0. */
int extractOperands(char *str, int opcode, int phase) {

	/* Initializes char array with buffer size, it will be used with strcpy */
	char rwString[BUF_SIZE];
	/* rwPointer Will be used as string to delimit to token */
	char *rwPointer;
	/* Copies string to rwString, will be used later to re-copy string to str */
	strcpy(rwString,str);

	/* token will be used to extract operands using comma delimiter
	 * srcAddrValue means the value that's passed from source. i.e. mov A,B => A is the srcAddr value.
	 * destAddrValue means the value that's passed to destination. i.e. mov A,B => B is the destAddr value.
	 */
	char *token, *srcAddrValue, *destAddrValue;

	 /* srcAddr is the addressing type of the source operand. Can be 0/1/2/23/3. If opcode doesn't have srcAddr to be assigned, it will remain -1.
	 *  destAddr is the addressing type of the destination operand. Can be 0/1/2/23/3.
	 *  noOperands is a boolean which will be used in case that the opcode has no operands at all.
	 *  singleOperand is a boolean which will be used in case that the opcode has only destination operand.
	 *  validateSuccess is IC number received by validOperOpcode in case that it passed the required tests.
	 */
	int i=0,singleOperand=0,noOperands=0,srcAddr=-1,destAddr=-1,validateSuccess=0;
	rwPointer = rwString;

	/* *************
	 * VERIFICATIONS
	 * *************/

	/* Extracts the 1st operand using comma delimiter */
	token = strchr(str,(int) commaChar[0]);
	/* The opcode is like one of the below... (no operands at all) */
	if (opcode==14 || opcode==15) {
		/* If operand is found anyway... Error! */
		if (token!=NULL) {
			printf("ERROR: Line %d - You can't use operands with this command!\n",count);
			errorFlag++;
			return 0;
		}
		/* noOperands flag is active */
		noOperands=1;
	}

	/* Only 1 operand is found AND noOperands is false */
	if (token == NULL && !noOperands) {
		/* Checks that string wasn't empty before the extraction. Also checks that the opcode has only destination addressing. */
		if (str != NULL && commandTable[opcode].srcOperations==0 && commandTable[opcode].destOperations==1) {
			/* singleOperand flag is active */
			singleOperand=1;
		} else {
			printf("ERROR: Line %d - Invalid addressing for this operand.\n",count);
			errorFlag++;
			return 0;
		}
	}
	/* Checks for 2nd Operands */
	token = strsep(&rwPointer,commaChar);

	/* Checks for overall comma count, increases i each time comma is present*/
	while (token != NULL && !singleOperand && !noOperands) {
		/* Checks for empty content after comma */
		if (strcmp(token,"\0")==0) {
			printf("ERROR: Line %d - Empty or invalid operand has been found.\n",count);
			errorFlag++;
			return 0;
		}
		token = strsep(&rwPointer,commaChar);
		i++;
	}

	/* *************
	 * EXTRACT DATA
	 * *************/

	/* PROCEED IF => 2 operands found exactly (1 comma), neither single operand or no operands at all. */
	if (i==2 && !singleOperand && !noOperands) {
		/* Re-copies string back to rwString after successful validation */
		strcpy(rwString,str);
		rwPointer = rwString;

		/* Extract 1st operand and place it's value in srcAddrValue */
		token = strsep(&rwPointer,commaChar);
		srcAddrValue = token;
		/* Sends the value for recognition */
		srcAddr = recognizeOperand(token);

		/* Extract 2nd operand and place it's value in destAddrValue */
		token = strsep(&rwPointer,commaChar);
		destAddrValue = token;
		/* Sends the value for recognition */
		destAddr = recognizeOperand(token);

		/* If both operands are recognized, proceed to check if the addressing is validate for this specific opcode and operands */
		if (srcAddr!=-1 && destAddr!=-1) {
			/* For phase 2, inserts the following variables to Data Table */
			if (phase==2) {
				insertToDataTable(opcode,srcAddr,destAddr,srcAddrValue,destAddrValue);
				return 2;
			} else {
				/* Check if the addressing is validate for this specific opcode and operands. Result is saved to validateSuccess. */
				validateSuccess=validOperOpcode(opcode,srcAddr,destAddr);
			}
			/* In case that addressing has passed, it returns the number of IC needed*/
			if(validateSuccess!=0) {
				return validateSuccess;
			} else {
				printf("ERROR: Line %d - Invalid operands received for this command.\n",count);
				errorFlag++;
				return validateSuccess;
			}
		}
		return 1;

	/* Dealing with singleOperand */
	} else if (singleOperand) {
		/* Value is being saved to destAddrValue */
		destAddrValue = token;
		/* Recognizes the operand type */
		destAddr = recognizeOperand(token);
		if (destAddr!=-1) {
			/* For phase 2, inserts the following variables to Data Table */
			if (phase==2) {
				insertToDataTable(opcode,-1,destAddr,NULL,destAddrValue);
				return 2;
			} else {
				/* Check if the addressing is validate for this specific opcode and operands. Result is saved to validateSuccess. */
				validateSuccess=validOperOpcode(opcode,-1,destAddr);
			}
			/* In case that addressing has passed, it returns the number of IC needed*/
			if (validateSuccess!=0) {
				return validateSuccess;
			} else {
				printf("ERROR: Line %d - Invalid operands received for this command.\n",count);
				errorFlag++;
				return 0;
			}
		}
		return 0;
		/* Dealing with no operands */
	} else if (noOperands) {
		/* For phase 2, inserts the following variables to Data Table */
		if (phase==2) {
			insertToDataTable(opcode,-1,-1,NULL,NULL);
		}
		/* 1 IC is returned */
		return 1;
	} else {
		printf("ERROR: Line %d - Do not place 2 commas side by side\n2. Please make sure you do not exceed 2 operands limit.\n",count);
		errorFlag++;
		return 0;
	}
}
/* recognizeOperand receives a string and parses to type. It returns 0/1/21/22/23/3 **OR** -1 if invalid operand value found */
int recognizeOperand(char *str) {
	unsigned int tempNum;
	int invalidResult=-1;

	/* If it's register */
	if (str[0] == 'r') {
		/* Has 2 chars exactly as it should... */
		if (strlen(str)==2) {
			str=str+1;
			/* Converts char to integer */
			tempNum = atoi(str);
			/* Register number is between 0-7 */
			if (tempNum>=REGISTER_MIN && tempNum<=REGISTER_MAX) {
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
	/* Starts with #, means an integer... */
	} else if (str[0] == '#') {
		/* If it's negative number */
		if (str[1] == '-') {
			/* Skip # and - */
			str = str+2;
			tempNum = atoi(str);
			/* Lowest negative number that can be achieved using two's complement negative notation */
			if (tempNum<=16384 && tempNum>0) {
				return 0;
			} else {
				printf("ERROR: Line %d - Invalid number entered.\n",count);
				errorFlag++;
				return invalidResult;
			}
		} else {
			/* Skip # and - */
			str = str+2;
			tempNum = atoi(str);
			/* Highest number that can be achieved using two's complement negative notation */
			if (tempNum<=16383 && tempNum>0) {
				return 0;
			} else {
				printf("ERROR: Line %d - Invalid number entered.\n",count);
				errorFlag++;
				return invalidResult;
			}
		}
		/* Random register */
	} else if (strcmp(str,"*")==0) {
		return 21;
		/* Random number */
	} else if (strcmp(str,"**")==0) {
		return 22;
		/* Random symbol (not external) */
	} else if (strcmp(str,"***")==0) {
		return 23;
	} else {
		/* Checks that symbol has UPPERCASE chars only */
		while (strlen(str)!=0) {
			if (str[0]>=UPPERCASE_A && str[0]<=UPPERCASE_Z)
				return 1;
			str=str+1;
		}
		printf("ERROR: Line %d - Invalid data operand entered.\n",count);
		errorFlag++;
		return invalidResult;
	}
}
/* General Background:
 * -------------------
 *
 * Addressing Table uses the following method to validate opcode's addressing rules according to the table in page 29:
 * 1 - Always present (in order the preserve 0 digits at the beginning)
 * 0-15 - Opcode
 * 0/1 - Source\Destination
 * 0/1/2/23/3 (2 with 3 asterisk) - Addressing
 *
 * Any addressing that's not in table is considered invalid.
 */
int validOperOpcode(int opcode, int srcAddr, int destAddr) {
	int i=0, j=0, x=0, numSrc=0, numDest=0, foundSrc=0, foundDest=0;

	/* Calculates the size of addressingTable (15) */
	int addressingTableLen = sizeof(addressingTable)/sizeof(addressingTable[0]);
	char addressingSrc[6] = { 0 };
	char addressingDest[6] = { 0 };
	int digit;
	int tmp=0;

	/* First digit in every addressTable entry is always 1 */
	addressingSrc[i++]=1, addressingDest[j++]=1;

	/* If opcode consists from 2 digits, split it */
	if (opcode>9) {
		/* Splits digits */
		for (x=0; x<2; x++) {
			digit = opcode % 10;
			opcode /=10;
			addressingSrc[i++] = digit;
			addressingDest[j++] = digit;
		}
		/* Macro that replaces 2 digits with each other */
		reverseDigits();
	} else {
		/* opcode is 1 digit */
		addressingSrc[i++] = opcode;
		addressingDest[j++] = opcode;
	}
	/* Add 0 for source and 1 for destination */
	addressingSrc[i++]=0, addressingDest[j++]=1;

	/* Two operands are received */
	if (srcAddr!=-1 && destAddr!=-1) {
		/* If source operand is bigger than 9 => Can be 23 (random with 3 asterisks) */
		if (srcAddr>9) {
			/* Splits digits */
			for (x=0; x<2; x++) {
				digit = srcAddr % 10;
				srcAddr /=10;
				addressingSrc[i++] = digit;
			}
			/* Macro that replaces 2 digits with each other */
			reverseDigits();
		} else {
			/* srcAddr is 1 digit exactly */
			addressingSrc[i++] = srcAddr;
		}
		/* If destination operand is bigger than 9 => Can be 23 (random with 3 asterisks) */
		if (destAddr>9) {
			for (x=0; x<2; x++) {
				digit = destAddr % 10;
				destAddr /=10;
				addressingDest[j++] = digit;
			}
			reverseDigits();
		} else {
			/* destAddr is 1 digit exactly */
			addressingDest[j++] = destAddr;
		}

		/* Sums back to decimal number all the results and searches in addressingTable */
		for (x = 0; x < i; x++)
			numSrc = 10 * numSrc + addressingSrc[x];
		for (x = 0; x < j; x++)
			numDest = 10 * numDest + addressingDest[x];

		/* Looking both source and destination operands in addressTable */
		for (i=0; i<=addressingTableLen; i++) {
			if (addressingTable[i]==numSrc)
				foundSrc=1;
			if (addressingTable[i]==numDest)
				foundDest=1;
			/* Number has been found in addressingTable! Returns success... */
			if (foundSrc && foundDest) {
				if (srcAddr==3 && destAddr==3) {
					return 2;
				}
				return 3;
			}
		}
	/* If there's only 1 operand */
	} else if (srcAddr==-1 && destAddr!=-1) {
		/* If destination operand is bigger than 9 => Can be 23 (random with 3 asterisks) */
		if (destAddr>9) {
			for (x=0; x<2; x++) {
				digit = destAddr % 10;
				destAddr /=10;
				addressingDest[j++] = digit;
			}
			/* Macro that replaces 2 digits with each other */
			reverseDigits();
		} else {
			/* destAddr is 1 digit exactly */
			addressingDest[j++] = destAddr;
		}
		/* Sums back to decimal number all the results and searches in addressingTable */
		for (x = 0; x < j; x++)
			numDest = 10 * numDest + addressingDest[x];
		/* Looking for destination operand in addressTable */
		for (i=0; i<=addressingTableLen; i++) {
			if (addressingTable[i]==numDest)
				foundDest=1;
			/* Number has been found in addressingTable! Returns success... */
			if (foundDest)
				return 2;
		}
	}
	return 0;
}
