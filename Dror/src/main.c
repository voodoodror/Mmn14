#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#include "main.h"
#include "first_parsing.h"
#include "second_parsing.h"
#include "utils.h"
#include "prerequisites.h"
#include "struct.h"

char *lineToProcess;
char file_name[20];
int symbolLen, dotLen;
int symbolCounter;
int errorFlag;
int dc, ic;
int count;
int externCounter,entryCounter;
int hashTableCounter;
int icForHashTable;
FILE *obj, *ext, *ent, *fp;
mySymbolList *symbolList;
myDataTable *dataTable;
myHashTable *hashTable;
myCommandTable commandTable[COMMAND_SIZE];
int addressingTable[82] = { 1000, 1001, 10021, 10022, 10023, 1003, 1011, 10121, 10122, 10123, 10123, 1013, 1100, 1101, 11011, 110121, 110122, 110123, 11013, 11021, 11022, 11023, 1103, 1110, 1111, 11111, 111121, 111122, 111123, 11113, 11121, 11122, 11123, 1113, 11210, 11211, 112121, 112122, 112123, 11213, 11311, 113123, 1200, 1201, 12021, 12022, 12023, 1203, 1211, 12121, 12122, 12123, 12123, 1213, 1300, 1301, 13021, 13022, 13023, 1303, 1311, 13121, 13122, 13123, 1313, 1411, 1413, 1511, 1513, 1601, 16023, 1611, 1613, 1711, 1713, 1811, 1813, 1911, 19121, 19122, 19123, 1913 };

int main(int argc, char **argv)
{

	int i=0, j=0;
    char *lineToProcess = malloc(sizeof(char) * BUF_SIZE);

    init_command_table();

    for (i = 1; i < argc; i++)
    {
    	count=0;
    	symbolLen = 0;
    	dotLen = 0;
    	symbolCounter = 0;
    	errorFlag = 0;
    	dc=0, ic=IC_MEM_ALLOCATION;
    	icForHashTable=IC_MEM_ALLOCATION;
    	externCounter=0, entryCounter=0;
    	hashTableCounter=0;
    	symbolList = NULL;
		dataTable = NULL;
		hashTable = NULL;

    	char *project_name = parseProjectName(argv[i]);

        sprintf(file_name, "%s", argv[i]);
        fp = fopen(file_name, "r");

        sprintf(file_name, "%s.ent", project_name);
		ent = fopen(file_name, "w");

		sprintf(file_name, "%s.ext", project_name);
		ext = fopen(file_name, "w");

		sprintf(file_name, "%s.obj", project_name);
		obj = fopen(file_name, "w");


		 /*print header */

        if (fp == NULL)
        {
            printf("ERROR: Cannot find file %s\n", argv[i]);
            errorFlag++;
        }

        count=0;
        if (!errorFlag) {
			while (fgets(lineToProcess, MAX_LINE, fp))
			{
				first_parsing_line(lineToProcess, ++count);
			}
        }

        count=0;
        if (!errorFlag) {
			rewind(fp);
			replaceStrAddr();
			hashTable = malloc(sizeof(myHashTable)*(ic-IC_MEM_ALLOCATION));
        }

        if (!errorFlag) {
			while (fgets(lineToProcess, MAX_LINE, fp))
			{
				if (!errorFlag) {
				second_parsing_line(lineToProcess, ++count);
				}
			}
        }
        if (!errorFlag) {
        	hashTableToFile();
        }

        fclose(ent);
        fclose(fp);
        fclose(obj);
		fclose(ext);

        printf("\n\n*DEBUG* %s\n",project_name);
        printf("Total Errors in program: %d\n",errorFlag);
        printf("IC: %d\tDC: %d\n\n",ic,dc);
        printf("\n\nSymbol Table:\n");
		mySymbolList* iter;
		for (iter = symbolList; NULL != iter; iter = iter->next)
			printf("NAME: \"%s\"\tADDR: \"%d\"\tEXTERNAL: \"%d\"\tACTION: \"%d\"\n",iter->Sym,iter->addr,iter->ext,iter->action);

		printf("\n\nData Table:\n");
		myDataTable* iterd;
		for (iterd = dataTable; NULL != iterd; iterd = iterd->next)
			printf("DC: \"%d\"\tDATA: \"%d\"\tBINARY DATA: \"%s\"\tBASE32 DATA: \"%s\"\n",iterd->dc,iterd->data,iterd->binaryData,iterd->base32);

		printf("\n\nHash Table:\n");
		printf("IC\tERA\tDEST_REG\tDEST_ADDR\tSRC_REG\t\tSRC_ADDR\tOPCODE\tGROUP\tRND\tDATA\tDATA_NUM\tNOT_IN_USE\tbinaryData\t\tbase32\n");
		if (hashTable!=NULL)
			for (j=0; j<ic-IC_MEM_ALLOCATION; j++)
				printf("%d\t%d\t\t%d\t\t%d\t%d\t\t%d\t\t%d\t%d\t%d\t%d\t%d\t\t%d\t\t%s\t\t%s\n",hashTable[j].addr,hashTable[j].era,hashTable[j].dest_reg,hashTable[j].dest_addr,hashTable[j].src_reg,hashTable[j].src_addr,hashTable[j].opcode,hashTable[j].group,hashTable[j].rnd,hashTable[j].data,hashTable[j].datanum,hashTable[j].not_in_use,hashTable[j].binaryData,hashTable[j].base32);

	    if (errorFlag!=0)
	    {
	        sprintf(file_name, "%s.obj", project_name);
	        remove(file_name);
	        sprintf(file_name, "%s.ext", project_name);
	        remove(file_name);
	        sprintf(file_name, "%s.ent", project_name);
	        remove(file_name);
	    } else if (!externCounter) {
	        sprintf(file_name, "%s.ext", project_name);
	        remove(file_name);
	    } else if (!entryCounter){
	        sprintf(file_name, "%s.ent", project_name);
	        remove(file_name);
	    }
    }
    return 0;
}
