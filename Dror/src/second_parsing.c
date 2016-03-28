#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "second_parsing.h"
#include "struct.h"
#include "prerequisites.h"
#include "main.h"
#include "utils.h"

extern int dc, ic, errorFlag,symbolLen,externCounter,entryCounter,icForHashTable,hashTableCounter;
extern mySymbolList *symbolList;
extern myDataTable *dataTable;
extern myHashTable *hashTable;
extern FILE *obj, *ext, *ent;

extern const char symbolChar;
extern const char dotChar;
extern const char spaceChar;

void second_parsing_line (char *line, int count) {

	char *symbolPointer;
	char *dotCommand;
	int symFound;
	char *temp = malloc(sizeof(char*));
	int commandFound=-1;


	strip_extra_spaces(line);
	if (line[0] != ';' && strlen(line) != 0) {
			if (hasSymbol(line) != 0) {
				if (hasDot(line+(symbolLen+sizeof(symbolChar)+sizeof(spaceChar))) == NULL) {
					dotCommand = getNextString(line+(symbolLen+sizeof(symbolChar)+sizeof(spaceChar)));
					commandFound = findCommand(dotCommand);
					extractSym(commandFound,2);
					incHashTable();
				}
			} else if (line[0] == '.'){
				dotCommand = getNextString(line+sizeof(dotChar));
				if (strcmp(dotCommand,"entry") == 0) {
					symbolPointer = getNextString(line+(sizeof(spaceChar)+strlen(dotCommand)+sizeof(spaceChar)));
					symFound = findExistingSym(symbolList,symbolPointer,"entry");
					if (symFound!=-1) {
						fprintf(ent, "%s\t%s\n", symbolPointer, decimalToBase32(symFound,PADDING_DISABLED,temp));
						entryCounter++;
					} else {
						printf("ERROR: Line %d - Entry symbol does not present in symbol list.\n",count);
						errorFlag++;
					}
				}
			} else {
				dotCommand = getNextString(line);
				commandFound = findCommand(dotCommand);
				extractOperands(line+(sizeof(spaceChar)+strlen((char *)dotCommand)),commandFound,2);
				incHashTable();
			}
	}
}
void hashTableToFile() {
	char *temp=malloc(sizeof(char*));
	char *base32ToHash;
	char *stringToHash;
	char *endp = NULL;
	int i=0;
	for (i=0; i<ic-IC_MEM_ALLOCATION; i++) {
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
			hashTable[i].binaryData = stringToHash;
			hashTable[i].base32 = decimalToBase32(strtoul(hashTable[i].binaryData, &endp, 2),PADDING_ENABLED,base32ToHash);
		}
		if (hashTable[i].data!=0 && hashTable[i].era!=0) {
			stringToHash=malloc(sizeof(char*));
			base32ToHash=malloc(sizeof(char*));
			temp = decimalToBinary(hashTable[i].data,2,temp,PADDING_ENABLED,DATA_WIDTH);
			stringToHash = concat(stringToHash,temp);
			temp = decimalToBinary(hashTable[i].era,2,temp,PADDING_ENABLED,ERA_WIDTH);
			stringToHash = concat(stringToHash,temp);
			hashTable[i].binaryData = stringToHash;
			hashTable[i].base32 = decimalToBase32(strtoul(hashTable[i].binaryData, &endp, 2),PADDING_ENABLED,base32ToHash);
		}
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
			hashTable[i].binaryData = stringToHash;
			hashTable[i].base32 = decimalToBase32(strtoul(hashTable[i].binaryData, &endp, 2),PADDING_ENABLED,base32ToHash);
		}
		if (hashTable[i].datanum!=0) {
			stringToHash=malloc(sizeof(char*));
			base32ToHash=malloc(sizeof(char*));
			temp = decimalToBinary(hashTable[i].datanum,2,temp,PADDING_ENABLED,DATA_WIDTH);
			stringToHash = concat(stringToHash,temp);
			temp = decimalToBinary(hashTable[i].era,2,temp,PADDING_ENABLED,ERA_WIDTH);
			stringToHash = concat(stringToHash,temp);
			hashTable[i].binaryData = stringToHash;
			hashTable[i].base32 = decimalToBase32(strtoul(hashTable[i].binaryData, &endp, 2),PADDING_ENABLED,base32ToHash);
		}
		if (hashTable[i].era==1 && hashTable[i].data==0) {
			stringToHash=malloc(sizeof(char*));
			base32ToHash=malloc(sizeof(char*));
			temp = decimalToBinary(hashTable[i].data,2,temp,PADDING_ENABLED,DATA_WIDTH);
			stringToHash = concat(stringToHash,temp);
			temp = decimalToBinary(hashTable[i].era,2,temp,PADDING_ENABLED,ERA_WIDTH);
			stringToHash = concat(stringToHash,temp);
			hashTable[i].binaryData = stringToHash;
			hashTable[i].base32 = decimalToBase32(strtoul(hashTable[i].binaryData, &endp, 2),PADDING_ENABLED,base32ToHash);
		}

	}
	fprintf(obj, "\tBase 32 Address\tBase 32 machine code\n\n");
	base32ToHash=malloc(sizeof(char*));
	fprintf(obj, "\t\t\t\t\t\t\t%s",decimalToBase32(ic-IC_MEM_ALLOCATION,PADDING_DISABLED,base32ToHash));
	base32ToHash=malloc(sizeof(char*));
	fprintf(obj, "\t%s\n",decimalToBase32(dc,PADDING_DISABLED,base32ToHash));

	for (i=0; i<ic-IC_MEM_ALLOCATION; i++) {
		base32ToHash=malloc(sizeof(char*));
		fprintf(obj, "\t\t\t\t\t\t%s\t%s\n",decimalToBase32(hashTable[i].addr,PADDING_ENABLED,base32ToHash),hashTable[i].base32);
	}

	myDataTable* iterd;
	for (iterd = dataTable; NULL != iterd; iterd = iterd->next)
		fprintf(obj, "\t\t\t\t\t\t%s\t%s\n",decimalToBase32(ic+iterd->dc,PADDING_ENABLED,base32ToHash),iterd->base32);
}
void insertToDataTable(int opcode, int srcAddr, int destAddr, char *srcAddrValue, char *destAddrValue) {
	int result=-1;
	char *temp = malloc(sizeof(char*));

	hashTable[hashTableCounter].opcode = opcode;

	if (srcAddr!=-1 && destAddr!=-1) {
		hashTable[hashTableCounter].src_addr = srcAddr;
		hashTable[hashTableCounter].dest_addr = destAddr;
		hashTable[hashTableCounter].group = 2;

		if (srcAddr==21) {
			hashTable[hashTableCounter].rnd = 1;
			hashTable[hashTableCounter].src_addr=2;
			incHashTable();
			srand(time(NULL));
			int r = rand();
			hashTable[hashTableCounter].src_reg = r;
		}
		if (destAddr==21) {
			hashTable[hashTableCounter].rnd = 1;
			hashTable[hashTableCounter].dest_addr=2;
			incHashTable();
			srand(time(NULL));
			int r = rand();
			hashTable[hashTableCounter].dest_reg = r;
		}
		if (srcAddr==22 || destAddr==22) {
			if (srcAddr==22)
				hashTable[hashTableCounter].src_addr=2;
			if (destAddr==22)
				hashTable[hashTableCounter].dest_addr=2;

			hashTable[hashTableCounter].rnd = 2;
			incHashTable();
			srand(time(NULL));
			int r = rand() % 8192;
			hashTable[hashTableCounter].datanum = r;
		}
		if (srcAddr==23 || destAddr==23) {
			if (srcAddr==23)
				hashTable[hashTableCounter].src_addr=2;
			if (destAddr==23)
				hashTable[hashTableCounter].dest_addr=2;

			hashTable[hashTableCounter].rnd = 3;
			incHashTable();
			hashTable[hashTableCounter].era = 2;
			hashTable[hashTableCounter].data = findExistingSym(symbolList,NULL,"random");
		}

	} else if (srcAddr==-1 && destAddr!=-1) {
		hashTable[hashTableCounter].group = 1;
		hashTable[hashTableCounter].dest_addr = destAddr;
	} else if (srcAddr==-1 && destAddr==-1) {

	}
	if (srcAddr==3 && destAddr==3) {
		incHashTable();
	}
	if (srcAddr==3 || srcAddr==21) {
		if (destAddr!=3 && destAddr!=21) {
			incHashTable();
		}
		if (srcAddr==3)
			hashTable[hashTableCounter].src_reg = atoi(srcAddrValue+1);
	}
	if (destAddr==3 || destAddr==21) {
		if (srcAddr!=3 && srcAddr!=21) {
			incHashTable();
		}
		if (destAddr==3)
			hashTable[hashTableCounter].dest_reg = atoi(destAddrValue+1);
	}
	if(srcAddr==1) {
		incHashTable();
		result = findExistingSym(symbolList,srcAddrValue,"data");
		if (result==-1)
		{
			errorFlag++;
			printf("ERROR! Label does not found.\n");
		}
		if (result==0) {
			hashTable[hashTableCounter].era = 1;
			fprintf(ext, "%s\t%s\n", destAddrValue, decimalToBase32(icForHashTable,PADDING_DISABLED,temp));
			externCounter++;
		} else {
			hashTable[hashTableCounter].era = 2;
		}
		hashTable[hashTableCounter].data = result;
	}
	if(destAddr==1) {
		incHashTable();
		result = findExistingSym(symbolList,destAddrValue,"data");
		if (result==-1)
		{
			errorFlag++;
			printf("ERROR! Label does not found.\n");
		}
		if (result==0) {
			hashTable[hashTableCounter].era = 1;
			fprintf(ext, "%s\t%s\n", destAddrValue, decimalToBase32(icForHashTable,PADDING_DISABLED,temp));
			externCounter++;
		} else {
			hashTable[hashTableCounter].era = 2;
		}
		hashTable[hashTableCounter].data = result;
	}
	if(srcAddr==0) {
		incHashTable();
		hashTable[hashTableCounter].datanum = atoi(srcAddrValue+1);
	}
	if(destAddr==0) {
		incHashTable();
		hashTable[hashTableCounter].datanum = atoi(destAddrValue+1);
	}
}
