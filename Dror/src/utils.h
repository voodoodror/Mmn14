/* utils.h
 * Dror Bletter
 * voodoodror@gmail.com
 */

#ifndef UTILS_H_
#define UTILS_H_

extern const char *symbolChar;
extern const char *dotChar;
extern const char *qmChar;
extern const char *spaceChar;
extern const char *commaChar;
extern const char *newLineChar;

void strip_extra_spaces(char* str);
char *decimalToBinary(int num, int base, char *result, int pad, int bit_len);
char *decimalToBase32 (int num, int pad, char *result);
char* concat(char *s1, char *s2);
char *hasDot(char* str);
char *hasQM(char* str);
int symIsUpper(char* str);
char *parseProjectName(char* project);
void init_command_table();
void replaceStrAddr();
int findExistingSym(mySymbolList *symbolList,char *sym, char *type);
int findCommand(char *command);
int hasSymbol(char* str);
char *getNextString(char* str);
char *getSymbol(char* str, int pos);



#endif /* UTILS_H_ */
