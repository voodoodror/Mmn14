#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "struct.h"
#include "second_parsing.h"
#include "main.h"
#include "prerequisites.h"
#include "utils.h"


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
myDataTable *createDataNode (int dc, int data) {
	myDataTable* newData = malloc(sizeof(myDataTable));
	char *temp = malloc(sizeof(char*));
	char *temp2 = malloc(sizeof(char*));
	char *endp = NULL;
	if (NULL != newData){
		newData->dc = dc;
		newData->data = data;
		newData->binaryData = decimalToBinary(data,2,temp,PADDING_ENABLED,MAX_DIGIT);
		newData->base32 = decimalToBase32(strtoul(newData->binaryData, &endp, 2),PADDING_ENABLED,temp2);
		newData->next = NULL;
	}
	return newData;
}
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
