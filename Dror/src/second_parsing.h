/* second_parsing.h
 * Dror Bletter
 * voodoodror@gmail.com
 */

#ifndef SECOND_PARSING_H_
#define SECOND_PARSING_H_

void second_parsing_line (char *line, int count);
void hashTableToFile();
void insertToDataTable(int opcode, int srcAddr, int destAddr, char *srcAddrValue, char *destAddrValue);

#define incHashTable() \
		hashTable[hashTableCounter++].addr = icForHashTable++;

#endif /* SECOND_PARSING_H_ */
