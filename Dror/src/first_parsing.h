/*
 * first_parsing.h
 *
 *  Created on: 26 במרץ 2016
 *      Author: Dror
 */

#ifndef FIRST_PARSING_H_
#define FIRST_PARSING_H_

void first_parsing_line (char *line, int count);
int extractData(char *str, char *type);
int extractOperands(char *str, int opcode, int phase);
int recognizeOperand(char *str);
int validOperOpcode(int opcode, int srcAddr, int destAddr);

#include "struct.h"

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

#endif /* FIRST_PARSING_H_ */
