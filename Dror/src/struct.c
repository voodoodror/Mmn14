/* struct.c
 * Dror Bletter
 * voodoodror@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "struct.h"
#include "second_parsing.h"
#include "utils.h"
#include "main.h"

/* Used to create a new linked list of symbols */
mySymbolList *createSymbolNode (char* str, unsigned int dc, int external, int action) {
	mySymbolList* newSymbol = malloc(sizeof(mySymbolList));
	char *myStr = malloc(sizeof(strlen(str)));
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
/* Used to add an item to linked list of symbols */
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
/* Used to create a new linked list of data */
myDataTable *createDataNode (int dc, int data) {
	myDataTable* newData = malloc(sizeof(myDataTable));
	char *temp = malloc(sizeof(char*));
	char *temp2 = malloc(sizeof(char*));
	char *endp = NULL;
	if (NULL != newData){
		newData->dc = dc;
		newData->data = data;
		/* Converts the decimal number to binary */
		newData->binaryData = decimalToBinary(data,2,temp,PADDING_ENABLED,MAX_DIGIT);
		/* Converts binary back to decimal and then converts it to base32 */
		newData->base32 = decimalToBase32(strtoul(newData->binaryData, &endp, 2),PADDING_ENABLED,temp2);
		newData->next = NULL;
	}
	return newData;
}
/* Used to add an item to linked list of data */
myDataTable *addDataNode (myDataTable* dataTable, int dc, int data) {
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
/* Initialize command table */
void init_command_table() {
	commandTable[0].command = "mov";
	commandTable[0].srcOperations=1;
	commandTable[0].destOperations=1;
	commandTable[1].command = "cmp";
	commandTable[1].srcOperations=1;
	commandTable[1].destOperations=1;
	commandTable[2].command = "add";
	commandTable[2].srcOperations=1;
	commandTable[2].destOperations=1;
	commandTable[3].command = "sub";
	commandTable[3].srcOperations=1;
	commandTable[3].destOperations=1;
	commandTable[4].command = "not";
	commandTable[4].srcOperations=0;
	commandTable[4].destOperations=1;
	commandTable[5].command = "clr";
	commandTable[5].srcOperations=0;
	commandTable[5].destOperations=1;
	commandTable[6].command = "lea";
	commandTable[6].srcOperations=1;
	commandTable[6].destOperations=1;
	commandTable[7].command = "inc";
	commandTable[7].srcOperations=0;
	commandTable[7].destOperations=1;
	commandTable[8].command = "dec";
	commandTable[8].srcOperations=0;
	commandTable[8].destOperations=1;
	commandTable[9].command = "jmp";
	commandTable[9].srcOperations=0;
	commandTable[9].destOperations=1;
	commandTable[10].command = "bne";
	commandTable[10].srcOperations=0;
	commandTable[10].destOperations=1;
	commandTable[11].command = "red";
	commandTable[11].srcOperations=0;
	commandTable[11].destOperations=1;
	commandTable[12].command = "prn";
	commandTable[12].srcOperations=0;
	commandTable[12].destOperations=1;
	commandTable[13].command = "jsr";
	commandTable[13].srcOperations=0;
	commandTable[13].destOperations=1;
	commandTable[14].command = "rts";
	commandTable[14].srcOperations=0;
	commandTable[14].destOperations=0;
	commandTable[15].command = "stop";
	commandTable[15].srcOperations=0;
	commandTable[15].destOperations=0;
}
