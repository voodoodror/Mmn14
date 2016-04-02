/* main.h
 * Dror Bletter
 * voodoodror@gmail.com
 */

#ifndef MAIN_H_
#define MAIN_H_

extern mySymbolList *symbolList;
extern myDataTable *dataTable;
extern myHashTable *hashTable;
extern FILE *obj, *ext, *ent;
extern myCommandTable commandTable[16];
extern int addressingTable[82];
extern int symbolLen, dotLen;
extern int symbolCounter;
extern int errorFlag;
extern int dc, ic;
extern int count;
extern int externCounter,entryCounter;
extern int hashTableCounter;
extern int icForHashTable;

#endif /* MAIN_H_ */
