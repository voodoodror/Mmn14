/* second_parsing.c
 * Dror Bletter
 * voodoodror@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "struct.h"
#include "first_parsing.h"
#include "second_parsing.h"
#include "main.h"
#include "utils.h"

void second_parsing_line (char *line, int count) {

	char *symbolPointer;
	char *dotCommand;
	int symFound;
	char *temp = malloc(sizeof(char*));
	int commandFound=-1;

	/* Strips extra spaces, tabs */
	strip_extra_spaces(line);
	/* Checking whether line is not empty and has no comment char */
	if (line[0] != ';' && strlen(line) != 0) {
			/* Check if it has symbol ':' char in string */
			if (hasSymbol(line) != 0) {
				/* If it has no dot, it's a command */
				if (hasDot(line+(symbolLen+strlen(symbolChar)+strlen(spaceChar))) == NULL) {
					/* Get the command */
					dotCommand = getNextString(line+(symbolLen+strlen(symbolChar)+strlen(spaceChar)));
					/* Find it in table */
					commandFound = findCommand(dotCommand);
					/* extract opcode and operands */
					extractSym(commandFound,2);
					/* Increase hash table counter by 1 after assignment to hash table is done */
					incHashTable();
				}
				/* Line has dot */
			} else if (line[0] == '.'){
				/* What's coming after dot? */
				dotCommand = getNextString(line+strlen(dotChar));
				/* If it's entry */
				if (strcmp(dotCommand,"entry") == 0) {
					/* Gets the symbol after entry */
					symbolPointer = getNextString(line+(strlen(spaceChar)+strlen(dotCommand)+strlen(spaceChar)));
					/* Checks that entry is in the list */
					symFound = findExistingSym(symbolList,symbolPointer,"entry");
					if (symFound!=-1) {
						/* Write entry to  ent file in BASE32 */
						fprintf(ent, "%s\t%s\n", symbolPointer, decimalToBase32(symFound,PADDING_DISABLED,temp));
						entryCounter++;
					} else {
						printf("ERROR: Line %d - Entry symbol does not present in symbol list.\n",count);
						errorFlag++;
					}
				}
			} else {
				/* Get the command */
				dotCommand = getNextString(line);
				/* Find it's opcode */
				commandFound = findCommand(dotCommand);
				/* Extract the operands and add it to the table */
				extractOperands(line+(strlen(spaceChar)+strlen((char *)dotCommand)),commandFound,2);
				/* Increase hash table counter by 1 after assignment to hash table is done */
				incHashTable();
			}
	}
}
/* Extracts all data from hash table to file */
void hashTableToFile() {
	char *temp=malloc(sizeof(char*));
	char *base32ToHash;
	char *stringToHash;
	char *endp = NULL;
	int i=0;
	/* Run for IC times */
	for (i=0; i<ic-IC_MEM_ALLOCATION; i++) {
		/* If it's an action statement */
		if (hashTable[i].era==0 && (hashTable[i].src_addr!=0 || hashTable[i].dest_addr!=0 || hashTable[i].opcode!=0)) {
			stringToHash=malloc(sizeof(char*));
			base32ToHash=malloc(sizeof(char*));
			temp = decimalToBinary(hashTable[i].not_in_use,2,temp,PADDING_ENABLED,NIU_WIDTH);
			stringToHash = concat(stringToHash,temp);
			temp = decimalToBinary(hashTable[i].rnd,2,temp,PADDING_ENABLED,RND_WIDTH);
			stringToHash = concat(stringToHash,temp);
			temp = decimalToBinary(hashTable[i].group,2,temp,PADDING_ENABLED,GROUP_WIDTH);
			stringToHash = concat(stringToHash,temp);
			temp = decimalToBinary(hashTable[i].opcode,2,temp,PADDING_ENABLED,OPCODE_WIDTH);
			stringToHash = concat(stringToHash,temp);
			temp = decimalToBinary(hashTable[i].src_addr,2,temp,PADDING_ENABLED,SADDR_WIDTH);
			stringToHash = concat(stringToHash,temp);
			temp = decimalToBinary(hashTable[i].dest_addr,2,temp,PADDING_ENABLED,DADDR_WIDTH);
			stringToHash = concat(stringToHash,temp);
			temp = decimalToBinary(hashTable[i].era,2,temp,PADDING_ENABLED,ERA_WIDTH);
			stringToHash = concat(stringToHash,temp);
			/* Gathered all binary string to binaryData */
			hashTable[i].binaryData = stringToHash;
			/* Converts binaryData to decimal and then to Base32 */
			hashTable[i].base32 = decimalToBase32(strtoul(hashTable[i].binaryData, &endp, 2),PADDING_ENABLED,base32ToHash);
		}
		/* For negative data numbers */
		if (hashTable[i].data!=0 && hashTable[i].era!=0) {
			stringToHash=malloc(sizeof(char*));
			base32ToHash=malloc(sizeof(char*));
			temp = decimalToBinary(hashTable[i].data,2,temp,PADDING_ENABLED,DATA_WIDTH);
			stringToHash = concat(stringToHash,temp);
			temp = decimalToBinary(hashTable[i].era,2,temp,PADDING_ENABLED,ERA_WIDTH);
			stringToHash = concat(stringToHash,temp);
			/* Gathered all binary string to binaryData */
			hashTable[i].binaryData = stringToHash;
			/* Converts binaryData to decimal and then to Base32 */
			hashTable[i].base32 = decimalToBase32(strtoul(hashTable[i].binaryData, &endp, 2),PADDING_ENABLED,base32ToHash);
		}
		/* It's action statement that's meant for registers */
		if (hashTable[i].src_reg!=0 || hashTable[i].dest_reg!=0) {
			stringToHash=malloc(sizeof(char*));
			base32ToHash=malloc(sizeof(char*));
			temp = decimalToBinary(hashTable[i].not_in_use,2,temp,PADDING_ENABLED,NIU_WIDTH);
			stringToHash = concat(stringToHash,temp);
			temp = decimalToBinary(hashTable[i].src_reg,2,temp,PADDING_ENABLED,SREG_WIDTH);
			stringToHash = concat(stringToHash,temp);
			temp = decimalToBinary(hashTable[i].dest_reg,2,temp,PADDING_ENABLED,DREG_WIDTH);
			stringToHash = concat(stringToHash,temp);
			temp = decimalToBinary(hashTable[i].era,2,temp,PADDING_ENABLED,ERA_WIDTH);
			stringToHash = concat(stringToHash,temp);
			/* Gathered all binary string to binaryData */
			hashTable[i].binaryData = stringToHash;
			/* Converts binaryData to decimal and then to Base32 */
			hashTable[i].base32 = decimalToBase32(strtoul(hashTable[i].binaryData, &endp, 2),PADDING_ENABLED,base32ToHash);
		}
		/* For positive data numbers */
		if (hashTable[i].datanum!=0) {
			stringToHash=malloc(sizeof(char*));
			base32ToHash=malloc(sizeof(char*));
			temp = decimalToBinary(hashTable[i].datanum,2,temp,PADDING_ENABLED,DATA_WIDTH);
			stringToHash = concat(stringToHash,temp);
			temp = decimalToBinary(hashTable[i].era,2,temp,PADDING_ENABLED,ERA_WIDTH);
			stringToHash = concat(stringToHash,temp);
			/* Gathered all binary string to binaryData */
			hashTable[i].binaryData = stringToHash;
			/* Converts binaryData to decimal and then to Base32 */
			hashTable[i].base32 = decimalToBase32(strtoul(hashTable[i].binaryData, &endp, 2),PADDING_ENABLED,base32ToHash);
		}
		/* For external references */
		if (hashTable[i].era==1 && hashTable[i].data==0) {
			stringToHash=malloc(sizeof(char*));
			base32ToHash=malloc(sizeof(char*));
			temp = decimalToBinary(hashTable[i].data,2,temp,PADDING_ENABLED,DATA_WIDTH);
			stringToHash = concat(stringToHash,temp);
			temp = decimalToBinary(hashTable[i].era,2,temp,PADDING_ENABLED,ERA_WIDTH);
			stringToHash = concat(stringToHash,temp);
			/* Gathered all binary string to binaryData */
			hashTable[i].binaryData = stringToHash;
			/* Converts binaryData to decimal and then to Base32 */
			hashTable[i].base32 = decimalToBase32(strtoul(hashTable[i].binaryData, &endp, 2),PADDING_ENABLED,base32ToHash);
		}

	}
	/* Prints headline */
	fprintf(obj, "\tBase 32 Address\tBase 32 machine code\n\n");
	base32ToHash=malloc(sizeof(char*));
	/* Prints IC Counter & DC */
	fprintf(obj, "\t\t\t\t\t\t\t%s",decimalToBase32(ic-IC_MEM_ALLOCATION,PADDING_DISABLED,base32ToHash));
	base32ToHash=malloc(sizeof(char*));
	fprintf(obj, "\t%s\n",decimalToBase32(dc,PADDING_DISABLED,base32ToHash));

	/* Prints Base32 IC Address and Base32 */
	for (i=0; i<ic-IC_MEM_ALLOCATION; i++) {
		base32ToHash=malloc(sizeof(char*));
		fprintf(obj, "\t\t\t\t\t\t%s\t%s\n",decimalToBase32(hashTable[i].addr,PADDING_ENABLED,base32ToHash),hashTable[i].base32);
	}

	/* Adds DataTable at the bottom of the program */
	myDataTable* iterd;
	for (iterd = dataTable; NULL != iterd; iterd = iterd->next)
		fprintf(obj, "\t\t\t\t\t\t%s\t%s\n",decimalToBase32(ic+iterd->dc,PADDING_ENABLED,base32ToHash),iterd->base32);
}
/* Fills Data Table accordingly to each scenario */
void insertToDataTable(int opcode, int srcAddr, int destAddr, char *srcAddrValue, char *destAddrValue) {
	int result=-1;
	char *temp = malloc(sizeof(char*));

	/* Set opcode */
	hashTable[hashTableCounter].opcode = opcode;

	/* Source operand and destination operand found. Assigns both and sets group. */
	if (srcAddr!=-1 && destAddr!=-1) {
		hashTable[hashTableCounter].src_addr = srcAddr;
		hashTable[hashTableCounter].dest_addr = destAddr;
		hashTable[hashTableCounter].group = 2;

		/* Source operand is random register */
		if (srcAddr==21) {
			hashTable[hashTableCounter].rnd = 1;
			hashTable[hashTableCounter].src_addr=2;
			incHashTable();
			srand(time(NULL));
			/* Select random number from 0 to 7 */
			int r = rand() % 8;
			hashTable[hashTableCounter].src_reg = r;
		}
		/* Destination operand is random register */
		if (destAddr==21) {
			hashTable[hashTableCounter].rnd = 1;
			hashTable[hashTableCounter].dest_addr=2;
			incHashTable();
			srand(time(NULL));
			/* Select random number from 0 to 7 */
			int r = rand() % 8;
			hashTable[hashTableCounter].dest_reg = r;
		}
		/* Source OR destination operand is a random number */
		if (srcAddr==22 || destAddr==22) {
			/* Source operand is a random number */
			if (srcAddr==22)
				hashTable[hashTableCounter].src_addr=2;
			/* Destination operand is a random number */
			if (destAddr==22)
				hashTable[hashTableCounter].dest_addr=2;

			hashTable[hashTableCounter].rnd = 2;
			incHashTable();
			srand(time(NULL));
			/* 8192 is the maximum number that's possible to represent in 13 bits */
			int r = rand() % 8192;
			hashTable[hashTableCounter].datanum = r;
		}
		/* Source or Destination operand is random symbol (not external)*/
		if (srcAddr==23 || destAddr==23) {
			/* Source operand is random symbol*/
			if (srcAddr==23)
				hashTable[hashTableCounter].src_addr=2;
			/* Destination operand is random symbol*/
			if (destAddr==23)
				hashTable[hashTableCounter].dest_addr=2;
			/* Sets rnd */
			hashTable[hashTableCounter].rnd = 3;
			/* Increase table counter with 1 */
			incHashTable();
			/* Sets era */
			hashTable[hashTableCounter].era = 2;
			/* Look for random symbol */
			hashTable[hashTableCounter].data = findExistingSym(symbolList,NULL,"random");
		}
		/* Only destination operand is present */
	} else if (srcAddr==-1 && destAddr!=-1) {
		hashTable[hashTableCounter].group = 1;
		hashTable[hashTableCounter].dest_addr = destAddr;
		/* No operands at all, do nothing */
	} else if (srcAddr==-1 && destAddr==-1) {

	}
	/* Source or destination operand is register */
	if (srcAddr==3 && destAddr==3) {
		/* Increase hashtable counter once */
		incHashTable();
	}
	/* Source is register OR source is a random register */
	if (srcAddr==3 || srcAddr==21) {
		/* If destination isn't register AND destination isn't not random register */
		if (destAddr!=3 && destAddr!=21) {
			incHashTable();
			/* Increase hashtable counter once */
		}
		/* Set source register with srcAddrValue */
		if (srcAddr==3)
			hashTable[hashTableCounter].src_reg = atoi(srcAddrValue+1);
	}
	/* Destination is register OR destination is a random register */
	if (destAddr==3 || destAddr==21) {
		/* If source isn't register AND source isn't not random register */
		if (srcAddr!=3 && srcAddr!=21) {
			/* Increase hashtable counter once */
			incHashTable();
		}
		/* Set destination register with destAddrValue */
		if (destAddr==3)
			hashTable[hashTableCounter].dest_reg = atoi(destAddrValue+1);
	}
	/* If source operand is Symbol */
	if(srcAddr==1) {
		/* Increase hashtable counter once */
		incHashTable();
		/* Find the specified source operand value in table */
		result = findExistingSym(symbolList,srcAddrValue,"data");
		if (result==-1)
		{
			errorFlag++;
			printf("ERROR! Label does not found.\n");
		}
		/* 0 means external */
		if (result==0) {
			/* Set era */
			hashTable[hashTableCounter].era = 1;
			/* Write external to ext file */
			fprintf(ext, "%s\t%s\n", destAddrValue, decimalToBase32(icForHashTable,PADDING_DISABLED,temp));
			/* ext found */
			externCounter++;
		} else {
			/* Set era */
			hashTable[hashTableCounter].era = 2;
		}
		/* set data by result */
		hashTable[hashTableCounter].data = result;
	}
	/* If destination operand is Symbol */
	if(destAddr==1) {
		incHashTable();
		result = findExistingSym(symbolList,destAddrValue,"data");
		if (result==-1)
		{
			errorFlag++;
			printf("ERROR! Label does not found.\n");
		}
		/* 0 means external */
		if (result==0) {
			/* Set era */
			hashTable[hashTableCounter].era = 1;
			/* Write external to ext file */
			fprintf(ext, "%s\t%s\n", destAddrValue, decimalToBase32(icForHashTable,PADDING_DISABLED,temp));
			/* ext found */
			externCounter++;
		} else {
			/* Set era */
			hashTable[hashTableCounter].era = 2;
		}
		/* set data by result */
		hashTable[hashTableCounter].data = result;
	}
	/* source operand is a number */
	if(srcAddr==0) {
		/* Increase hashtable counter once */
		incHashTable();
		/* convert number to integer from char and assign to datanum*/
		hashTable[hashTableCounter].datanum = atoi(srcAddrValue+1);
	}
	/* destination operand is a number */
	if(destAddr==0) {
		/* Increase hashtable counter once */
		incHashTable();
		/* convert number to integer from char and assign to datanum */
		hashTable[hashTableCounter].datanum = atoi(destAddrValue+1);
	}
}
