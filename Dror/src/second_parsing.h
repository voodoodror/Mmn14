/* second_parsing.h
 * Dror Bletter
 * voodoodror@gmail.com
 */

#ifndef SECOND_PARSING_H_
#define SECOND_PARSING_H_

#include "first_parsing.h"

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

#define incHashTable() \
		hashTable[hashTableCounter++].addr = icForHashTable++;

#endif /* SECOND_PARSING_H_ */
