/*
 * first_parsing.h
 *
 *  Created on: 26 ���� 2016
 *      Author: Dror
 */

#ifndef SECOND_PARSING_H_
#define SECOND_PARSING_H_

void second_parsing_line (char *line, int count);
void hashTableToFile();
void insertToDataTable(int opcode, int srcAddr, int destAddr, char *srcAddrValue, char *destAddrValue);

extern const char symbolChar;
extern const char dotChar;
extern const char spaceChar;
extern const char qmChar;
extern const char newlineChar;
extern const char commaChar;

#include "struct.h"
#include "first_parsing.h"

#define extractSym(X,Y) extractOperands(line+(symbolLen+sizeof(symbolChar)+sizeof(spaceChar)+sizeof(dotChar)+strlen(dotCommand)),X,Y);
#define extractSymData(X) \
		extractData(line+(symbolLen+sizeof(symbolChar)+sizeof(spaceChar)+sizeof(dotChar)+strlen(dotCommand)+sizeof(spaceChar)),X);\
		if (extractResult) {\
			if (symbolCounter == 0)\
				symbolList = createSymbolNode(symbolPointer,tmp,0,0);\
			else\
				symbolList = addSymbolNode(symbolList,symbolPointer,tmp,0,0);\
			symbolCounter++;\
		}
#define reverseDigits() \
		memcpy(&tmp,&addressingSrc[i-2],sizeof(int));\
		memcpy(&addressingSrc[i-2],&addressingSrc[i-1],sizeof(int));\
		memcpy(&addressingSrc[i-1],&tmp,sizeof(int));\
		memcpy(&tmp,&addressingDest[j-2],sizeof(int));\
		memcpy(&addressingDest[j-2],&addressingDest[j-1],sizeof(int));\
		memcpy(&addressingDest[j-1],&tmp,sizeof(int));
#define incHashTable() \
		hashTable[hashTableCounter++].addr = icForHashTable++;

#endif /* SECOND_PARSING_H_ */
