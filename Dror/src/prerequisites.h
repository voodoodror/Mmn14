/* prerequisites.h
 * Dror Bletter
 * voodoodror@gmail.com
 */

#ifndef PREREQUISITES_H_
#define PREREQUISITES_H_

#include "struct.h"

char *parseProjectName(char* project);
void init_command_table();
void replaceStrAddr();
int findExistingSym(mySymbolList *symbolList,char *sym, char *type);
int findCommand(char *command);
int hasSymbol(char* str);
char *getNextString(char* str);
char *getSymbol(char* str, int pos);

#endif /* PREREQUISITES_H_ */
