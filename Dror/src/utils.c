/* utils.c
 * Dror Bletter
 * voodoodror@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "struct.h"
#include "utils.h"
#include "main.h"

const char *symbolChar = ":";
const char *dotChar = ".";
const char *spaceChar = " ";
const char *qmChar = "\"";
const char *newlineChar = "\n";
const char *commaChar = ","; /* , char */

/* strip_extra_spaces Removes any extra spaces\tabs, keeps commas clean from left and right, removes new line char */
void strip_extra_spaces(char* str) {
  int i,x;

  /* Loop and replace every tab with space first */
  for(i=x=0; str[i]; ++i) {
	  if (str[i+1] == '\t')
	  		  str[i+1] = ' ';
	  /* Skip if the following cases are met */
	  if (isspace(str[i]) && !isalpha(str[i-1]) && (str[i-1] != symbolChar[0]) && (!isdigit(str[i+1]) || !isdigit(str[i-1]) || str[i+1] =='-' || str[i-1] == qmChar[0]))
		  	  i++;
	  if(!isspace(str[i]) || (i>0 && !isspace(str[i-1])))
		  str[x++] = str[i];
  }
  if (isspace(str[x]))
	  str[x] = '\0';
  /* Removes new line char */
  if (strchr(str,(int) newlineChar[0]) != NULL)
	  str[x] = '\0';
  else
	  str[x] = '\0';
  if (isspace(str[strlen((char *)str)-1]))
  	  str[strlen((char *)str)-1] = '\0';
}

/* decimalToBinary
 * Receives decimal number, base, padding, required char bit length and returns binary char * */
char *decimalToBinary(int num, int base, char *result, int pad, int bit_len)
{
	int negativeNumber=0,oneFound=0;
	int index = 0, i,j;
	char tmp[bit_len + 1];
	for (i = 0; i < bit_len; i++) /* reset all the cells */
	{
		result[i] = '0';
		tmp[i] = '0';
	}
	if (num<0) {
		num = abs(num);
		negativeNumber=1;
	}
	while (num != 0)
	{
		tmp[index] = (num % base) + '0';
		num /= base;
		index++;
	}

	for (i = 0; i < bit_len; i++)
	{
		result[i] = tmp[bit_len - 1 - i];
	}
    result[i] = '\0';

    /* If decimal is negative number, use two complement notation */
    if(negativeNumber) {
    	i=0;
    	while (!oneFound) {
    		if (result[bit_len-i]=='1') {
    			i=bit_len-i;
    			oneFound=1;
    		} else {
    			i++;
    		}
		}
    	for (j = 0; j < i; j++)
		{
			if (result[j]=='1')
				result[j]='0';
			else if (result[j]=='0')
				result[j]='1';
		}
    }

    if (pad)
    {
        return result;
    }
    else
    {
        return result + bit_len - index;
    }
}
/* decimalToBase32 converts decimal number to Base32. The functions gets number, pad (boolean) and returns result as char * */
char *decimalToBase32 (int num, int pad, char *result) {
	int quotient;
	int i=0,j,temp;
	char hexadecimalNumber[100] = {0};
	quotient = num;
	/* If number is already zero, convert it anyway with zeroes */
	if (!quotient) {
		while (pad && i!=MAX_BASE32_DIGIT) {
			hexadecimalNumber[i++] = '0';
		}
	}
	while(quotient!=0) {
		temp = quotient % 32;
		/* To convert integer into character */
		if( temp < 10)
			temp =temp + 48;
		else
			temp = temp + 55;
		hexadecimalNumber[i++]= temp;
		quotient = quotient / 32;
		/* If padding is enabled */
		while (pad && quotient==0 && i!=MAX_BASE32_DIGIT) {
			hexadecimalNumber[i++] = '0';
		}
	}
	for (j = i ;j> 0;j--)
	  result[i-j] = hexadecimalNumber[j-1];
	return result;
}
/* concat Merges between two char * */
char* concat(char *s1, char *s2)
{
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    char *result = malloc(len1+len2+1); /* +1 for the zero-terminator */
    /* in real code you would check for errors in malloc here */
    memcpy(result, s1, len1);
    memcpy(result+len1, s2, len2+1);/* +1 to copy the null-terminator */
    return result;
}
/* hasDot checks for existence of dot in a string. It returns the char * at the point where dot is. */
char *hasDot(char* str) {
	char *tmpStr;
	tmpStr = strchr(str,(int) dotChar[0]);

	return tmpStr;
}
/* hasQM checks for existence of quotation mark in a string. It returns the char * at the point where QM is. */
char *hasQM(char* str) {
	char *tmpStr;
	tmpStr = strchr(str,(int) qmChar[0]);

	return tmpStr;
}
/* symIsUpper checks if a string is UPPERCASE */
int symIsUpper(char* str) {
	char *myStr = malloc(sizeof(char*));
	memcpy(myStr,str,(size_t) sizeof(str));
	while (myStr[0] != '\0') {
		if (myStr[0]<65 || myStr[0]>90)
			return 0;
		myStr=myStr+1;
	}
	return 1;
}
/* Strips the extension (AS) of the file and returns the string without it */
char *parseProjectName(char* project) {
	char *dotPos = strchr(project, (int) dotChar[0]);
	char *projectName = malloc(sizeof(char*));

	int projectNameLen;

	projectNameLen = strlen(project)-strlen(dotPos);

	memcpy(projectName,project,projectNameLen);
	projectName[projectNameLen] = '\0';
	return projectName;
}
/* Replace DC with IC+DC */
void replaceStrAddr() {
	mySymbolList* iter;
	for (iter = symbolList; NULL != iter; iter = iter->next) {
		if(iter->ext==0 && iter->action==0) {
			iter->addr+=ic;
		}
	}
}
/* Lookup for existing symbol in list */
int findExistingSym(mySymbolList *symbolList,char *sym, char *type) {
	mySymbolList* iter;
	int totalSym=0,i=0,numberFound=0;
	for (iter = symbolList; NULL != iter; iter = iter->next) {
		if (sym!=NULL) {
			if(strcmp(iter->Sym,sym)==0) {
				if (strcmp(type,"entry") == 0) {
					if (iter->ext==0) {
						return iter->addr;
					} else {
						return -1;
					}
				} else if (strcmp(type,"symbol") == 0) {
					return 1;
				} else if (strcmp(type,"data") == 0) {
					return iter->addr;
				} else {
					return -1;
				}
			}
		}
		totalSym++;
	}
	/* findExistingSym is also used a random symbol when it gets "random" type */
	if (strcmp(type,"random")==0) {
		while (!numberFound) {
			srand(time(NULL));
			/* Selects a random number out of total symbols count */
			int r = ( rand() % totalSym ) + 1;
			iter = symbolList;
			for (i=0; i<r; i++) {
				iter = iter->next;
			}
			/* Retry if symbol is external */
			if (iter->ext==0) {
				return iter->addr;
			}
		}
	}
	return 0;
}
/* Searching for command in command table */
int findCommand(char *command) {
	int i=0;
	while (i<COMMAND_SIZE) {
		if (strcmp(command,commandTable[i].command)==0) {
			return i;
		}
		i++;
	}
	return -1;
}
/* hasSymbol checks for existence of ':' in a string. It returns the char * at the point where ':' is. */
int hasSymbol(char* str) {
	char *symbolPos = strchr(str,(int) symbolChar[0]);
	int tmpLen;

	if (symbolPos == NULL)
		return 0;
	tmpLen = strlen(str)-strlen(symbolPos);
	if (!tmpLen)
		tmpLen+=1;

	symbolLen=tmpLen;
	return tmpLen;
}
/* getNextString checks for existence of SPACE in a string. It returns the char * at the point where SPACE is. */
char *getNextString(char* str) {
	char *dotPos = strchr(str,(int) spaceChar[0]);
	char *tmp = malloc(sizeof(str));
	int tmpLen;

	if (dotPos == NULL)
		return str;
	tmpLen = strlen(str)-strlen(dotPos);
	if (!tmpLen)
		tmpLen+=1;

	dotLen=tmpLen;
	memcpy(tmp,str,tmpLen+1);
	tmp[dotLen] = '\0';
	return tmp;
}
/* getSymbol returns 'str' in 'pos' position */
char *getSymbol(char* str, int pos) {
	char *myStr = malloc(sizeof(char*));
	memcpy(myStr,str,pos);
	myStr[pos] = '\0';
	return myStr;
}
