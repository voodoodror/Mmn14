#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"

	mySymbolList *symbolList;
    char *lineToProcess;
    char symbolChar = ':';

int main(int argc, char **argv)
{
    int i;
    FILE *fp;
    char file_name[20];
    char *lineToProcess = malloc(sizeof(char) * BUF_SIZE);

    for (i = 1; i < argc; i++)
    {
        int count = 0, buf_count = BUF_SIZE;

        sprintf(file_name, "%s", argv[i]);
        fp = fopen(file_name, "r");

        if (fp == NULL)
        {
            printf("Error: can't find file %s\n", file_name);
            return -1;
        }

        while (fgets(lineToProcess, MAX_LINE, fp))
        {
        	first_parsing_line(lineToProcess, &count);
        	count++;
        }
        fclose(fp);
    }
    return 0;
}

void first_parsing_line (char *line, int *count) {
	int i=0, ic=0, dc=0;

	mySymbolList *symbolList = malloc(sizeof(mySymbolList));


	strip_extra_spaces(line);
	if (line[0] == ';') {
		printf("%d: %s (ignoring)\n",*count,line);
	} else {
		if (hasSymbol(line) != NULL) {
				if (hasSymbol(hasSymbol(line)+1) != NULL) {
					printf("%d: %s (multiple symbols, ignoring)\n",*count,line);
				} else {
					/*if (symbolList != NULL) {
						mySymbolList *tmp = realloc(symbolList,sizeof(symbolList)+sizeof(mySymbolList));
						if (tmp == NULL) {
							printf("No memory left for new symbol.");
							exit(0);
						}
						else {
							symbolList = tmp;*/
							symbolList[0].Sym = getSymbol(line);
						/*}
						}*/
				}
		} else {
			printf("%d: %s\n",*count,line);
		}
	}
}

void strip_extra_spaces(char* str) {
  int i,x;
  for(i=x=0; str[i]; ++i)
    if(!isspace(str[i]) || (i>0 && !isspace(str[i-1])))
      str[x++] = str[i];
  char newlineChar = '\n';
  if (strchr(str,newlineChar) != NULL) {
	  str[x-1] = '\0';
  }
  str[x] = '\0';
}

char *hasSymbol(char* str) {
	char *symbolPos = strchr(str, symbolChar);
	return symbolPos;
}
char *getSymbol(char* str) {
	char *tmpSymbol;
	int symbolLen = strlen(str)-strlen(strchr(str,symbolChar)-1);
	strncpy(tmpSymbol, str, symbolLen);
	return tmpSymbol;
}
