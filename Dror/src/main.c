/* main.c
 * Dror Bletter
 * voodoodror@gmail.com
 */
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
char *project_name;
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

/* Addressing Table uses the following method to validate opcode's addressing rules according to the table in page 29:
 * 1 - Always present (in order the preserve 0 digits at the beginning)
 * 0-15 - Opcode
 * 0/1 - Source\Destination
 * 0/1/2/23/3 (2 with 3 asterisk) - Addressing
 *
 * Any addressing that's not in table is considered invalid.
 */
int addressingTable[82] = { 1000, 1001, 10021, 10022, 10023, 1003, 1011, 10121, 10122, 10123, 10123, 1013, 1100, 1101, 11011, 110121, 110122, 110123, 11013, 11021, 11022, 11023, 1103, 1110, 1111, 11111, 111121, 111122, 111123, 11113, 11121, 11122, 11123, 1113, 11210, 11211, 112121, 112122, 112123, 11213, 11311, 113123, 1200, 1201, 12021, 12022, 12023, 1203, 1211, 12121, 12122, 12123, 12123, 1213, 1300, 1301, 13021, 13022, 13023, 1303, 1311, 13121, 13122, 13123, 1313, 1411, 1413, 1511, 1513, 1601, 16023, 1611, 1613, 1711, 1713, 1811, 1813, 1911, 19121, 19122, 19123, 1913 };

int main(int argc, char **argv)
{

	int i=0;
	/* int j=0; FOR DEBUG */
    char *lineToProcess = malloc(sizeof(char) * BUF_SIZE);

    /* Initialization of command table */
    init_command_table();

    /* Running on all arguments */
    for (i = 1; i < argc; i++)
    {
    	/* Resets all variables for the next AS file */
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

		/* Cuts the extension from the argument and uses the name for the rest of the files */
    	project_name = parseProjectName(argv[i]);

    	/* Opens AS file */
        sprintf(file_name, "%s", argv[i]);
        fp = fopen(file_name, "r");

        /* Entry file */
        sprintf(file_name, "%s.ent", project_name);
		ent = fopen(file_name, "w");

		/* Extern File */
		sprintf(file_name, "%s.ext", project_name);
		ext = fopen(file_name, "w");

		/* Obj File */
		sprintf(file_name, "%s.obj", project_name);
		obj = fopen(file_name, "w");

		/* File is not found */
        if (fp == NULL)
        {
            printf("ERROR: Cannot find file %s\n", argv[i]);
            errorFlag++;
        }

        /* If not errors present, proceed to first parse */
        if (!errorFlag) {
        	/* Reads the current line and stores it in lineToProcess*/
			while (fgets(lineToProcess, MAX_LINE, fp))
			{
				/* Sends the line to parsing, along with the line number (will be used for errors */
				first_parsing_line(lineToProcess, ++count);
			}
        }
        /* Resets the line count back to 0 for the 2nd parsing */
        count=0;
        /* If no errors present by far, prepare program for 2nd parsing */
        if (!errorFlag) {
        	/* Places the cursor back to the beginning of the file, for 2nd parsing */
			rewind(fp);
			/* Replaces symbol list from DC to IC. */
			replaceStrAddr();
			/* Creates hashTable array with size of total ic count taken from 1st parse */
			hashTable = malloc(sizeof(myHashTable)*(ic-IC_MEM_ALLOCATION));
        }

        /* If no errors present by far, start 2nd parsing */
        if (!errorFlag) {
			while (fgets(lineToProcess, MAX_LINE, fp))
			{
				/* Continue as long as no errors are found */
				if (!errorFlag) {
				second_parsing_line(lineToProcess, ++count);
				}
			}
        }
        /* This is the final step, print all the tables to files */
        if (!errorFlag) {
        	hashTableToFile();
        }

        /* Close files */
        fclose(ent);
        fclose(fp);
        fclose(obj);
		fclose(ext);

        /*printf("\n\n*DEBUG* %s\n",project_name);
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
				printf("%d\t%d\t\t%d\t\t%d\t%d\t\t%d\t\t%d\t%d\t%d\t%d\t%d\t\t%d\t\t%s\t\t%s\n",hashTable[j].addr,hashTable[j].era,hashTable[j].dest_reg,hashTable[j].dest_addr,hashTable[j].src_reg,hashTable[j].src_addr,hashTable[j].opcode,hashTable[j].group,hashTable[j].rnd,hashTable[j].data,hashTable[j].datanum,hashTable[j].not_in_use,hashTable[j].binaryData,hashTable[j].base32);*/

		/* In case that error flag is bigger than 0, remove obj, ext and ent files */
	    if (errorFlag!=0)
	    {
	        sprintf(file_name, "%s.obj", project_name);
	        remove(file_name);
	        sprintf(file_name, "%s.ext", project_name);
	        remove(file_name);
	        sprintf(file_name, "%s.ent", project_name);
	        remove(file_name);
	    /* In case that there are no extern lines, delete ext files */
	    } else if (!externCounter) {
	        sprintf(file_name, "%s.ext", project_name);
	        remove(file_name);
		/* In case that there are no entry lines, delete ent files */
	    } else if (!entryCounter){
	        sprintf(file_name, "%s.ent", project_name);
	        remove(file_name);
	    }
    }
    return 0;
}
