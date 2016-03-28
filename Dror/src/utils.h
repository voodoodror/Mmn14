/* utils.h
 * Dror Bletter
 * voodoodror@gmail.com
 */

#ifndef UTILS_H_
#define UTILS_H_

void strip_extra_spaces(char* str);
char *decimalToBinary(int num, int base, char *result, int pad, int bit_len);
char *decimalToBase32 (int num, int pad, char *result);
char* concat(char *s1, char *s2);
char *hasDot(char* str);
char *hasQM(char* str);
int symIsUpper(char* str);



#endif /* UTILS_H_ */
