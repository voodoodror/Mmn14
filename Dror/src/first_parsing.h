/* first_parsing.h
 * Dror Bletter
 * voodoodror@gmail.com
 */

#ifndef FIRST_PARSING_H_
#define FIRST_PARSING_H_
#define REGISTER_MIN 0
#define REGISTER_MAX 7
#define UPPERCASE_A 65
#define UPPERCASE_Z 90

void first_parsing_line (char *line, int count);
int extractData(char *str, char *type);
int extractOperands(char *str, int opcode, int phase);
int recognizeOperand(char *str);
int validOperOpcode(int opcode, int srcAddr, int destAddr);

/* extractSym Macro
 * ----------------
 * Receives the opcode number in decimal and the phase of the parsing (1st parse or 2nd parse).
 * Send both along with opcode values to extractOperands for further parsing and validation.
 */
#define extractSym(X,Y) extractOperands(line+(symbolLen+strlen(symbolChar)+strlen(spaceChar)+strlen(dotChar)+strlen(dotCommand)),X,Y);

/* extractSymData Macro
 * --------------------
 * Receives the type of instruction line (string\data) and sends it to extractData for validation, along with it's values.
 * If extractData returns 1, then the data is considered valid and being added to Symbol Table. It also checks if other symbols were added until then.
 *  */
#define extractSymData(X) \
		extractData(line+(symbolLen+strlen(symbolChar)+strlen(spaceChar)+strlen(dotChar)+strlen(dotCommand)+strlen(spaceChar)),X);\
		if (extractResult) {\
			if (symbolCounter == 0)\
				symbolList = createSymbolNode(symbolPointer,tmp,0,0);\
			else\
				symbolList = addSymbolNode(symbolList,symbolPointer,tmp,0,0);\
			symbolCounter++;\
		}

/* reverseDigits used in validOperOpcode to replace the place of the digits with each other */
#define reverseDigits() \
		memcpy(&tmp,&addressingSrc[i-2],sizeof(int));\
		memcpy(&addressingSrc[i-2],&addressingSrc[i-1],sizeof(int));\
		memcpy(&addressingSrc[i-1],&tmp,sizeof(int));\
		memcpy(&tmp,&addressingDest[j-2],sizeof(int));\
		memcpy(&addressingDest[j-2],&addressingDest[j-1],sizeof(int));\
		memcpy(&addressingDest[j-1],&tmp,sizeof(int));

#endif /* FIRST_PARSING_H_ */
