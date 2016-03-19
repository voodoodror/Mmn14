#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include "assembler.h"

char *lineToProcess;
char file_name[20];
const char symbolChar = ':';
const char dotChar = '.';
const char spaceChar = ' ';
const char qmChar = '\"';
const char newlineChar = '\n';
const char commaChar = 44; /* , char */
int symbolLen;
int dotLen;
int symbolCounter;
int errorFlag;
int dc, ic;
int count;
int externCounter,entryCounter;
int hashTableCounter;
int icForHashTable;
FILE *obj, *ext, *ent, *fp;
mySymbolList *symbolList;
myCommandTable commandTable[COMMAND_SIZE];
myDataTable *dataTable;
myHashTable *hashTable;
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

        sprintf(file_name, "%s", argv[i]);
        fp = fopen(file_name, "r");

        sprintf(file_name, "%s.ent", argv[i]);
		ent = fopen(file_name, "w");

		sprintf(file_name, "%s.ext", argv[i]);
		ext = fopen(file_name, "w");

		sprintf(file_name, "%s.obj", argv[i]);
		obj = fopen(file_name, "w");


		 /*print header */

        if (fp == NULL)
        {
            printf("ERROR: Cannot find file %s\n", file_name);
            return -1;
        }

        count=0;

        while (fgets(lineToProcess, MAX_LINE, fp))
        {
        	first_parsing_line(lineToProcess, count+1);
        	count++;
        }

        count=0;
        if (!errorFlag) {
			rewind(fp);
			replaceStrAddr();
			hashTable = malloc(sizeof(myHashTable)*(ic-IC_MEM_ALLOCATION));
        }

		while (fgets(lineToProcess, MAX_LINE, fp))
		{
			if (!errorFlag) {
			second_parsing_line(lineToProcess, count+1);
			count++;
			}
		}
        if (!errorFlag) {
        	hashTableToFile();
        }

        fclose(ent);
        fclose(fp);
        fclose(obj);
		fclose(ext);

        printf("\n\n*DEBUG*\n");
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
		printf("IC\tERA\tDEST_REG\tDEST_ADDR\tSRC_REG\t\tSRC_ADDR\tOPCODE\tGROUP\tRND\tDATA\tDATA_NUM\tNOT_IN_USE\n");
		for (j=0; j<ic-IC_MEM_ALLOCATION; j++)
			printf("%d\t%d\t\t%d\t\t%d\t%d\t\t%d\t\t%d\t%d\t%d\t%d\t%d\t\t%d\t\n",hashTable[j].addr,hashTable[j].era,hashTable[j].dest_reg,hashTable[j].dest_addr,hashTable[j].src_reg,hashTable[j].src_addr,hashTable[j].opcode,hashTable[j].group,hashTable[j].rnd,hashTable[j].data,hashTable[j].datanum,hashTable[j].not_in_use);

		/* close all the open files



		/*for (i = 0; i < HASHSIZE; i++)
		{
			data_symtab[i] = NULL;
			inst_symtab[i] = NULL;
			exttab[i] = NULL;
		}
	    if (errorFlag == 1)
	    {
	    	printf("Output files were not created due to found errors at the input file\n\n");
	        sprintf(file_name, "%s.obj", module_name);
	        remove(file_name);
	        sprintf(file_name, "%s.ext", module_name);
	        remove(file_name);
	        sprintf(file_name, "%s.ent", module_name);
	        remove(file_name);
	    }
	    else if (extern_counter == 0)
	    {
	        sprintf(file_name, "%s.ext", module_name);
	        remove(file_name);
	    }
	    else if (entry_counter == 0)
	    {
	        sprintf(file_name, "%s.ent", module_name);
	        remove(file_name);
	    }
	    return 0;*/
    }
    return 0;
}
void init_hash_table(myHashTable *il)
{
    il->era = 0;
    il->dest_reg = 0;
    il->dest_addr = 0;
    il->src_reg = 0;
    il->src_addr = 0;
    il->opcode = 0;
    il->group = 0;
    il->rnd = 0;
    il->not_in_use = 0;
}
void init_command_table()
{
	commandTable[0].command = "mov",commandTable[0].srcOperations=1,commandTable[0].destOperations=1;
	commandTable[1].command = "cmp",commandTable[1].srcOperations=1,commandTable[1].destOperations=1;
	commandTable[2].command = "add",commandTable[2].srcOperations=1,commandTable[2].destOperations=1;
	commandTable[3].command = "sub",commandTable[3].srcOperations=1,commandTable[3].destOperations=1;
	commandTable[4].command = "not",commandTable[4].srcOperations=0,commandTable[4].destOperations=1;
	commandTable[5].command = "clr",commandTable[5].srcOperations=0,commandTable[5].destOperations=1;
	commandTable[6].command = "lea",commandTable[6].srcOperations=1,commandTable[6].destOperations=1;
	commandTable[7].command = "inc",commandTable[7].srcOperations=0,commandTable[7].destOperations=1;
	commandTable[8].command = "dec",commandTable[8].srcOperations=0,commandTable[8].destOperations=1;
	commandTable[9].command = "jmp",commandTable[9].srcOperations=0,commandTable[9].destOperations=1;
	commandTable[10].command = "bne",commandTable[10].srcOperations=0,commandTable[10].destOperations=1;
	commandTable[11].command = "red",commandTable[11].srcOperations=0,commandTable[11].destOperations=1;
	commandTable[12].command = "prn",commandTable[12].srcOperations=0,commandTable[12].destOperations=1;
	commandTable[13].command = "jsr",commandTable[13].srcOperations=0,commandTable[13].destOperations=1;
	commandTable[14].command = "rts",commandTable[14].srcOperations=0,commandTable[14].destOperations=0;
	commandTable[15].command = "stop",commandTable[15].srcOperations=0,commandTable[15].destOperations=0;
}
void first_parsing_line (char *line, int count) {
	int dupSymbol=0, commandFound=-1, tmp=0;

	char *symbolPointer;
	char *dotCommand;
	int extractResult;

	strip_extra_spaces(line);
	if (line[0] != ';' && strlen(line) != 0) {
		if (!isalpha(line[0]) && line[0] != '.') {
			printf("ERROR: Line %d - line MUST begin with a letter or a dot.\n",count+1);
			errorFlag++;
		} else {
			if (hasSymbol(line) != 0) {
				if (hasSymbol(line+(symbolLen+sizeof(symbolChar))) != 0) {
					printf("ERROR: Line %d - More than 1 symbol sign has been found.\n",count+1);
					errorFlag++;
				} else if (!symIsUpper(getSymbol(line,hasSymbol(line)))) {
					printf("ERROR: Line %d - Symbol has other chars than UPPERCASE.\n",count+1);
					errorFlag++;
				} else {
					if (symbolCounter!=0) {
						symbolPointer = getSymbol(line,hasSymbol(line));
						dupSymbol = findExistingSym(symbolList,symbolPointer,"symbol");
					}
					if (!dupSymbol) {
						if (hasDot(line+(symbolLen+sizeof(symbolChar)+sizeof(spaceChar))) != NULL) {
							symbolPointer = getSymbol(line,hasSymbol(line));
							dotCommand = getNextString(line+(symbolLen+sizeof(symbolChar)+sizeof(spaceChar)+sizeof(dotChar)));
							if (strcmp(dotCommand,"string") == 0) {
								tmp = dc;
								extractResult = extractSymData("string");
							} else if (strcmp(dotCommand,"data") == 0) {
								tmp = dc;
								extractResult = extractSymData("data");
							} else {
								printf("ERROR: Line %d - Instruction line doesn't exist. Make you write everything in lowercase.\n",count+1);
								errorFlag++;
							}
						} else {
							dotCommand = getNextString(line+(symbolLen+sizeof(symbolChar)+sizeof(spaceChar)));
							commandFound = findCommand(dotCommand);
							if (commandFound!=-1) {
								extractResult = extractSym(commandFound,1);
								if (extractResult) {
									ic+=extractResult;
									symbolPointer = getSymbol(line,hasSymbol(line));
									if (symbolCounter == 0)
										symbolList = createSymbolNode(symbolPointer,ic-extractResult,0,1);
									else
										symbolList = addSymbolNode(symbolList,symbolPointer,ic-extractResult,0,1);
									symbolCounter++;
									printf("%d: %s (\"%s\")\n",count+1,line,dotCommand);
								}
							} else {
								printf("ERROR: Line %d - Command not found.\n",count+1);
								errorFlag++;
							}
						}
					} else {
						printf("ERROR: Line %d - Duplicate Symbol Detected.\n",count+1);
						errorFlag++;
					}
				}
			} else if (line[0] == '.'){
				dotCommand = getNextString(line+sizeof(dotChar));
				if (strcmp(dotCommand,"entry") == 0) {
					printf("%d: %s (entry found, ignoring in first parsing)\n",count+1,line);
				} else if (strcmp(dotCommand,"extern") == 0) {
					symbolPointer = getNextString(line+(sizeof(spaceChar)+strlen(dotCommand)+sizeof(spaceChar)));
					dupSymbol = findExistingSym(symbolList,symbolPointer,"symbol");
					if (!dupSymbol) {
						if (symbolCounter == 0)
							symbolList = createSymbolNode(symbolPointer,0,1,0);
						else
							symbolList = addSymbolNode(symbolList,symbolPointer,0,1,0);
						symbolCounter++;
						printf("%d: %s (extern found)\n",count+1,line);
					} else {
						printf("ERROR: Line %d - Duplicate Symbol Detected.\n",count+1);
						errorFlag++;
					}
				} else {
					printf("ERROR: Line %d - Instruction line doesn't exist.\n",count+1);
					errorFlag++;
				}
			} else {
				dotCommand = getNextString(line);
				commandFound = findCommand(dotCommand);
				if (commandFound!=-1) {
					extractResult = extractOperands(line+(sizeof(spaceChar)+strlen((char *)dotCommand)),commandFound,1);
					if (extractResult)
						printf("%d: %s command found: (\"%s\")\n",count+1,line,dotCommand);
						ic+=extractResult;
				} else {
					printf("ERROR: Line %d - Operand doesn't exist.\n",count+1);
					errorFlag++;
				}
			}
		}
	}
}
void second_parsing_line (char *line, int count) {

	char *symbolPointer;
	char *dotCommand;
	int symFound;
	char *temp = malloc(sizeof(char*));
	int i=0;
	int commandFound=-1;
	int extractResult;


	strip_extra_spaces(line);
	if (line[0] != ';' && strlen(line) != 0) {
			if (hasSymbol(line) != 0) {
				if (hasDot(line+(symbolLen+sizeof(symbolChar)+sizeof(spaceChar))) == NULL) {
					dotCommand = getNextString(line+(symbolLen+sizeof(symbolChar)+sizeof(spaceChar)));
					commandFound = findCommand(dotCommand);
					extractResult = extractSym(commandFound,2);
					incHashTable();
				}
			} else if (line[0] == '.'){
				dotCommand = getNextString(line+sizeof(dotChar));
				if (strcmp(dotCommand,"entry") == 0) {
					symbolPointer = getNextString(line+(sizeof(spaceChar)+strlen(dotCommand)+sizeof(spaceChar)));
					symFound = findExistingSym(symbolList,symbolPointer,"entry");
					if (symFound!=-1) {
						fprintf(ent, "%s\t%s\n", symbolPointer, decimalToBase32(symFound,temp));
						entryCounter++;
					} else {
						printf("ERROR: Line %d - Entry symbol does not present in symbol list.\n",count+1);
						errorFlag++;
					}
				}
			} else {
				dotCommand = getNextString(line);
				commandFound = findCommand(dotCommand);
				extractResult = extractOperands(line+(sizeof(spaceChar)+strlen((char *)dotCommand)),commandFound,2);
				incHashTable();
			}
	}
}
void hashTableToFile() {
	char *temp=malloc(sizeof(char*));
	int i=0;
	for (i=0; i<ic-IC_MEM_ALLOCATION; i++) {
		if (hashTable[i].era==0 && hashTable[i].src_addr!=0 && hashTable[i].dest_addr!=0) {
			sprintf(hashTable[i].binaryData,"%d\t0%s%s%s%s%s%s",hashTable[i].addr,decimalToBinary(hashTable[i].rnd,2,temp,1,RND_WIDTH),decimalToBinary(hashTable[i].group,2,temp,1,GROUP_WIDTH),decimalToBinary(hashTable[i].opcode,2,temp,1,OPCODE_WIDTH),decimalToBinary(hashTable[i].src_addr,2,temp,1,SADDR_WIDTH),decimalToBinary(hashTable[i].dest_addr,2,temp,1,DADDR_WIDTH),decimalToBinary(hashTable[i].era,2,temp,1,ERA_WIDTH));
			printf("%s",hashTable[i].binaryData);
		}
	}
}
void replaceStrAddr() {
	mySymbolList* iter;
	for (iter = symbolList; NULL != iter; iter = iter->next) {
		if(iter->ext==0 && iter->action==0) {
			iter->addr+=ic;
		}
	}
}
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
	if (strcmp(type,"random")==0) {
		while (!numberFound) {
			srand(time(NULL));
			int r = ( rand() % totalSym ) + 1;
			iter = symbolList;
			for (i=0; i<r; i++) {
				iter = iter->next;
			}
			if (iter->ext==0) {
				return iter->addr;
			}
		}
	}
	return 0;
}
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
void strip_extra_spaces(char* str) {
  int i,x;

  for(i=x=0; str[i]; ++i) {
	  if (str[i+1] == '\t')
	  		  str[i+1] = ' ';
	  if (isspace(str[i]) && !isalpha(str[i-1]) && (str[i-1] != symbolChar) && (!isdigit(str[i+1]) || !isdigit(str[i-1]) || str[i+1] =='-' || str[i-1] == qmChar))
		  	  i++;
	  if(!isspace(str[i]) || (i>0 && !isspace(str[i-1])))
		  str[x++] = str[i];
  }
  if (isspace(str[x]))
	  str[x] = '\0';
  if (strchr(str,newlineChar) != NULL)
	  str[x] = '\0';
  else
	  str[x] = '\0';
  if (isspace(str[strlen((char *)str)-1]))
  	  str[strlen((char *)str)-1] = '\0';
}

int hasSymbol(char* str) {
	char *symbolPos = strchr(str, symbolChar);
	int tmpLen;

	if (symbolPos == NULL)
		return 0;
	tmpLen = strlen(str)-strlen(symbolPos);
	if (!tmpLen)
		tmpLen+=1;

	symbolLen=tmpLen;
	return tmpLen;
}
char *hasDot(char* str) {
	char *tmpStr;
	tmpStr = strchr(str,dotChar);

	return tmpStr;
}
char *hasQM(char* str) {
	char *tmpStr;
	tmpStr = strchr(str,qmChar);

	return tmpStr;
}
int extractData(char *str, char *type) {

	char rwString[BUF_SIZE];
	char *rwPointer;
	strcpy(rwString,str);
	char *token;
	int num;

	rwPointer = rwString;
	if (str == NULL) {
		printf("ERROR: Line %d - No data has been found in string.\n",count+1);
		errorFlag++;
		return 0;
	}
	if (strcmp(type,"data") == 0) {

		token = strsep(&rwPointer,",");
		if (token == NULL) {
			printf("ERROR: Line %d - Error occurred while trying to parse data field.\n1. Please make sure you don\'t have comma at the beginning or end of the string.\n2. Don't put 2 commas side by side.\n",count+1);
			errorFlag++;
			return 0;
		}
		while (token != NULL) {
			if ((strlen(token)<=1) && (!isdigit(token[0]))) {
				printf("ERROR: Line %d - Empty or invalid char found.\n",count+1);
				errorFlag++;
				return 0;
			}
			token = strsep(&rwPointer,",");
		}
		strcpy(rwString,str);
		rwPointer = rwString;
		token = strsep(&rwPointer,",");

		while (token != NULL) {
			num = atoi(token);
			if (dc == 0) {
				dataTable = createDataNode(dc++,num);
			} else {
				dataTable = addDataNode(dataTable,dc++,num);
			}

			token = strsep(&rwPointer,",");
		}
		return 1;

		} else if (strcmp(type,"string") == 0) {
			int qmFound=0;
			token = str;
			while (token != NULL) {
				token = hasQM(token+1);
				qmFound++;
			}
			if (qmFound==2) {
				str = str+1;

				str[strlen(str)-1] = '\0';

				while (str[0]!='\0') {
					if (dc == 0) {
						dataTable = createDataNode(dc++,(int)str[0]);
						str++;
					} else {
						dataTable = addDataNode(dataTable,dc++,(int)str[0]);
						str++;
					}
				}
				dataTable = addDataNode(dataTable,dc++,0);
				return 1;
			} else if (qmFound<=2) {
				printf("ERROR: Line %d - No quotation marks found for string!\n",count+1);
				errorFlag++;
				return 0;
			} else if (qmFound>2) {
				printf("ERROR: Line %d - More than 1 quotation mark has been found!\n",count+1);
				errorFlag++;
				return 0;
			}
		}
	return 0;
}
int extractOperands(char *str, int opcode, int phase) {
	char rwString[BUF_SIZE];
	char *rwPointer;
	strcpy(rwString,str);
	char *token, *srcAddrValue, *destAddrValue;
	int i=0,singleOperand=0,noOperands=0,srcAddr=-1,destAddr=-1,validateSuccess=0;
	rwPointer = rwString;

	token = strchr(str,commaChar);
	if (opcode==14 || opcode==15) {
		if (token!=NULL) {
			printf("ERROR: Line %d - You can't use operands with this command!\n",count+1);
			errorFlag++;
			return 0;
		}
		noOperands=1;
	}
	if (token == NULL && !noOperands) {
		if (str != NULL && commandTable[opcode].srcOperations==0 && commandTable[opcode].destOperations==1) {
			singleOperand=1;
		} else {
			printf("ERROR: Line %d - Invalid addressing for this operand.\n",count+1);
			errorFlag++;
			return 0;
		}
	}

	token = strsep(&rwPointer,",");

	while (token != NULL && !singleOperand && !noOperands) {
		if (strcmp(token,"\0")==0) {
			printf("ERROR: Line %d - Empty or invalid operand has been found.\n",count+1);
			errorFlag++;
			return 0;
		}
		token = strsep(&rwPointer,",");
		i++;
	}
	if (i==2 && !singleOperand && !noOperands) {
		strcpy(rwString,str);
		rwPointer = rwString;

		token = strsep(&rwPointer,",");
		srcAddrValue = token;
		srcAddr = recognizeOperand(token);

		token = strsep(&rwPointer,",");
		destAddrValue = token;
		destAddr = recognizeOperand(token);

		if (srcAddr!=-1 && destAddr!=-1) {
			if (phase==2) {
				insertToDataTable(opcode,srcAddr,destAddr,srcAddrValue,destAddrValue);
				return 2;
			} else {
				validateSuccess=validOperOpcode(opcode,srcAddr,destAddr);
			}
			if(validateSuccess!=0) {
				return validateSuccess;
			} else {
				printf("ERROR: Line %d - Invalid operands received for this command.\n",count+1);
				errorFlag++;
				return validateSuccess;
			}
		}
		return 1;

	} else if (singleOperand) {
		destAddrValue = token;
		destAddr = recognizeOperand(token);
		if (destAddr!=-1) {
			if (phase==2) {
				insertToDataTable(opcode,-1,destAddr,NULL,destAddrValue);
				return 2;
			} else {
				validateSuccess=validOperOpcode(opcode,-1,destAddr);
			}
			if (validateSuccess!=0) {
				return validateSuccess;
			} else {
				printf("ERROR: Line %d - Invalid operands received for this command.\n",count+1);
				errorFlag++;
				return 0;
			}
		}
		return 0;
	} else if (noOperands) {
		if (phase==2) {
			insertToDataTable(opcode,-1,-1,NULL,NULL);
		}
		return 1;
	} else {
		printf("ERROR: Line %d - Do not place 2 commas side by side\n2. Please make sure you do not exceed 2 operands limit.\n",count+1);
		errorFlag++;
		return 0;
	}
}
void insertToDataTable(int opcode, int srcAddr, int destAddr, char *srcAddrValue, char *destAddrValue) {
	int result=-1;
	char *temp = malloc(sizeof(char*));

	hashTable[hashTableCounter].opcode = opcode;

	if (srcAddr!=-1 && destAddr!=-1) {
		hashTable[hashTableCounter].src_addr = srcAddr;
		hashTable[hashTableCounter].dest_addr = destAddr;
		hashTable[hashTableCounter].group = 2;

		if (srcAddr==21) {
			hashTable[hashTableCounter].rnd = 1;
			incHashTable();
			srand(time(NULL));
			int r = rand();
			hashTable[hashTableCounter].src_reg = r;
		}
		if (destAddr==21) {
			hashTable[hashTableCounter].rnd = 1;
			incHashTable();
			srand(time(NULL));
			int r = rand();
			hashTable[hashTableCounter].dest_reg = r;
		}
		if (srcAddr==22 || destAddr==22) {
			hashTable[hashTableCounter].rnd = 2;
			incHashTable();
			srand(time(NULL));
			int r = rand() % 8192;
			hashTable[hashTableCounter].datanum = r;
		}
		if (srcAddr==23 || destAddr==23) {
			hashTable[hashTableCounter].rnd = 3;
			incHashTable();
			hashTable[hashTableCounter].era = 2;
			hashTable[hashTableCounter].data = findExistingSym(symbolList,NULL,"random");
		}

	} else if (srcAddr==-1 && destAddr!=-1) {
		hashTable[hashTableCounter].group = 1;
	} else if (srcAddr==-1 && destAddr==-1) {

	}
	if (srcAddr==3 && destAddr==3) {
		incHashTable();
	}
	if (srcAddr==3) {
		if (destAddr!=3) {
			incHashTable();
		}
		hashTable[hashTableCounter].src_reg = atoi(srcAddrValue+1);
	}
	if (destAddr==3) {
		if (srcAddr!=3) {
			incHashTable();
		}
		hashTable[hashTableCounter].dest_reg = atoi(destAddrValue+1);
	}
	if(srcAddr==1) {
		incHashTable();
		result = findExistingSym(symbolList,srcAddrValue,"data");
		if (result==-1)
		{
			errorFlag++;
			printf("ERROR! Label does not found.\n");
		}
		if (result==0) {
			hashTable[hashTableCounter].era = 1;
			fprintf(ext, "%s\t%s\n", destAddrValue, decimalToBase32(icForHashTable,temp));
			externCounter++;
		} else {
			hashTable[hashTableCounter].era = 2;
		}
		hashTable[hashTableCounter].data = result;
	}
	if(destAddr==1) {
		incHashTable();
		result = findExistingSym(symbolList,destAddrValue,"data");
		if (result==-1)
		{
			errorFlag++;
			printf("ERROR! Label does not found.\n");
		}
		if (result==0) {
			hashTable[hashTableCounter].era = 1;
			fprintf(ext, "%s\t%s\n", destAddrValue, decimalToBase32(icForHashTable,temp));
			externCounter++;
		} else {
			hashTable[hashTableCounter].era = 2;
		}
		hashTable[hashTableCounter].data = result;
	}
	if(srcAddr==0) {
		incHashTable();
		hashTable[hashTableCounter].datanum = atoi(srcAddrValue+1);
	}
	if(destAddr==0) {
		incHashTable();
		hashTable[hashTableCounter].datanum = atoi(destAddrValue+1);
	}
}
int recognizeOperand(char *str) {
	unsigned int tempNum;
	int invalidResult=-1;

	if (str[0] == 'r') {
		if (strlen(str)==2) {
			str=str+1;
			tempNum = atoi(str);
			if (tempNum>=0 && tempNum<=7) {
				return 3;
			} else {
				printf("ERROR: Line %d - Invalid register number in addressing.\n",count+1);
				errorFlag++;
				return invalidResult;
			}
		} else {
			printf("ERROR: Line %d - Too LESS or MUCH chars for register operand.\n",count+1);
			errorFlag++;
			return invalidResult;
		}
	} else if (str[0] == '#') {
		if (str[1] == '-') {
			str = str+2;
			tempNum = atoi(str);
			if (tempNum<=16384 && tempNum>0) {
				return 0;
			} else {
				printf("ERROR: Line %d - Invalid number entered.\n",count+1);
				errorFlag++;
				return invalidResult;
			}
		} else {
			str = str+2;
			tempNum = atoi(str);
			if (tempNum<=16383 && tempNum>0) {
				return 0;
			} else {
				printf("ERROR: Line %d - Invalid number entered.\n",count+1);
				errorFlag++;
				return invalidResult;
			}
		}
	} else if (strcmp(str,"*")==0) {
		return 21;
	} else if (strcmp(str,"**")==0) {
		return 22;
	} else if (strcmp(str,"***")==0) {
		return 23;
	} else {
		while (strlen(str)!=0) {
			if (str[0]>=65 && str[0]<=90)
				return 1;
			str=str+1;
		}
		printf("ERROR: Line %d - Invalid data operand entered.\n",count+1);
		errorFlag++;
		return invalidResult;
	}
}
int validOperOpcode(int opcode, int srcAddr, int destAddr) {
	int i=0, j=0, x=0, numSrc=0, numDest=0, foundSrc=0, foundDest=0;
	int addressingTableLen = sizeof(addressingTable)/sizeof(addressingTable[0]);
	char addressingSrc[6] = { 0 };
	char addressingDest[6] = { 0 };
	int digit;
	int tmp=0;

	addressingSrc[i++]=1, addressingDest[j++]=1;

	if (opcode>9) {
		for (x=0; x<2; x++) {
			digit = opcode % 10;
			opcode /=10;
			addressingSrc[i++] = digit;
			addressingDest[j++] = digit;
		}
		reverseDigits();
	} else {
		addressingSrc[i++] = opcode;
		addressingDest[j++] = opcode;
	}

	addressingSrc[i++]=0, addressingDest[j++]=1;

	if (srcAddr!=-1 && destAddr!=-1) {
		if (srcAddr>9) {
			for (x=0; x<2; x++) {
				digit = srcAddr % 10;
				srcAddr /=10;
				addressingSrc[i++] = digit;
			}
			reverseDigits();
		} else {
			addressingSrc[i++] = srcAddr;
		}
		if (destAddr>9) {
			for (x=0; x<2; x++) {
				digit = destAddr % 10;
				destAddr /=10;
				addressingDest[j++] = digit;
			}
			reverseDigits();
		} else {
			addressingDest[j++] = destAddr;
		}

		for (x = 0; x < i; x++)
			numSrc = 10 * numSrc + addressingSrc[x];
		for (x = 0; x < j; x++)
			numDest = 10 * numDest + addressingDest[x];

		for (i=0; i<=addressingTableLen; i++) {
			if (addressingTable[i]==numSrc)
				foundSrc=1;
			if (addressingTable[i]==numDest)
				foundDest=1;
			if (foundSrc && foundDest) {
				if (srcAddr==3 && destAddr==3) {
					return 2;
				}
				return 3;
			}
		}
	} else if (srcAddr==-1 && destAddr!=-1) {
		if (destAddr>9) {
			for (x=0; x<2; x++) {
				digit = destAddr % 10;
				destAddr /=10;
				addressingDest[j++] = digit;
			}
			reverseDigits();
		} else {
			addressingDest[j++] = destAddr;
		}
		for (x = 0; x < j; x++)
			numDest = 10 * numDest + addressingDest[x];
		for (i=0; i<=addressingTableLen; i++) {
			if (addressingTable[i]==numDest)
				foundDest=1;
			if (foundDest)
				return 2;
		}
	}
	return 0;
}
char *getNextString(char* str) {
	char *dotPos = strchr(str, spaceChar);
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
char *getSymbol(char* str, int pos) {
	char *myStr = malloc(sizeof(char*));
	memcpy(myStr,str,pos);
	myStr[pos] = '\0';
	return myStr;
}
int symIsUpper(char* str) {
	char *myStr = malloc(sizeof(char*));
	memcpy(myStr,str,strlen(str));
	while (myStr[0] != '\0') {
		if (myStr[0]<65 || myStr[0]>90)
			return 0;
		myStr=myStr+1;
	}
	return 1;
}
mySymbolList *createSymbolNode (char* str, unsigned int dc, int external, int action) {
	mySymbolList* newSymbol = malloc(sizeof(mySymbolList));
	char * myStr = malloc(sizeof(strlen(str)));
	memcpy(myStr,str,strlen(str));

	if (NULL != newSymbol){
		newSymbol->Sym = myStr;
		newSymbol->addr = dc;
		newSymbol->ext = external;
		newSymbol->action = action;
		newSymbol->next = NULL;
	}
	return newSymbol;
}
mySymbolList *addSymbolNode (mySymbolList* symbolList, char* str, unsigned int dc, int external, int action) {
	mySymbolList* newSymbol = createSymbolNode(str,dc,external,action);
	mySymbolList *firstpos = symbolList;
	if (newSymbol != NULL) {
			while (symbolList->next != NULL) {
				symbolList = symbolList->next;
			}
			symbolList->next = newSymbol;
	}
	return firstpos;
}
myDataTable *createDataNode (int dc, int data) {
	myDataTable* newData = malloc(sizeof(myDataTable));
	char *temp = malloc(sizeof(char*));
	char *temp2 = malloc(sizeof(char*));
	char *endp = NULL;
	if (NULL != newData){
		newData->dc = dc;
		newData->data = data;
		newData->binaryData = decimalToBinary(data,2,temp,1,8);
		newData->base32 = decimalToBase32(strtoul(newData->binaryData, &endp, 2),temp2);
		newData->next = NULL;
	}
	return newData;
}
myDataTable *addDataNode (myDataTable* dataTable, int dc, int data) {
	myDataTable* newData = createDataNode(dc,data);
	myDataTable *firstpos = dataTable;
	if (newData != NULL) {
			while (dataTable->next != NULL) {
				dataTable = dataTable->next;
			}
			dataTable->next = newData;
	}
	return firstpos;
}
