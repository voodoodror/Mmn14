/*
 * assembler.h
 *
 *  Created on: Mar 3, 2016
 *      Author: dbletter
 */

#define MAX_LINE 80
#define BUF_SIZE 100
#define MAX_ARRAY_SIZE 1000
#define COMMAND_SIZE 16

#define ERA_WIDTH 2
#define DREG_WIDTH 3
#define DADDR_WIDTH 2
#define SREG_WIDTH 3
#define SADDR_WIDTH 2
#define OPCODE_WIDTH 4
#define GROUP_WIDTH 2
#define RND_WIDTH 2
#define NIU_WIDTH 1

#define COMB_OFFSET 0
#define DREG_OFFSET 2
#define DADDR_OFFSET 5
#define SREG_OFFSET 7
#define SADDR_OFFSET 10
#define OPCODE_OFFSET 12
#define TYPE_OFFSET 16
#define RSVD_OFFSET 17

#define MAX_DIGIT 8

#define MOV 0
#define CMP 1
#define ADD 2
#define SUB 3
#define NOT 4
#define CLR 5
#define LEA 6
#define INC 7
#define DEC 8
#define JMP 9
#define BNE 10
#define RED 11
#define PRN 12
#define JSR 13
#define RTS 14
#define STOP 15
#define extractSym(X) extractOperands(line+(symbolLen+sizeof(symbolChar)+sizeof(spaceChar)+sizeof(dotChar)+strlen(dotCommand)),X);
#define extractSymData(X) \
		extractData(line+(symbolLen+sizeof(symbolChar)+sizeof(spaceChar)+sizeof(dotChar)+strlen(dotCommand)+sizeof(spaceChar)),X);\
		if (extractResult) {\
			printf("%d: %s ("X" found)\n",count+1,line);\
		if (symbolCounter == 0)\
			symbolList = createSymbolNode(symbolPointer,tmp,0,0);\
		else\
			symbolList = addSymbolNode(symbolList,symbolPointer,tmp,0,0);\
		symbolCounter++;\
	}
#define reverseDigits() \
		memcpy(&tmp,&addressingSrc[i-2],sizeof(int));\
		memcpy(&addressingSrc[i-2],&addressingSrc[i-1],sizeof(int));\
		memcpy(&addressingSrc[i-1],&tmp,sizeof(int));\
		memcpy(&tmp,&addressingDest[j-2],sizeof(int));\
		memcpy(&addressingDest[j-2],&addressingDest[j-1],sizeof(int));\
		memcpy(&addressingDest[j-1],&tmp,sizeof(int));

typedef struct symbolLists {
	char *Sym;
	unsigned int addr;
	int ext;
	int action;
	struct symbolLists* next;
} mySymbolList;

typedef struct hashTables {
    unsigned int era        : ERA_WIDTH;
    unsigned int dest_reg   : DREG_WIDTH;
    unsigned int dest_addr  : DADDR_WIDTH;
    unsigned int src_reg    : SREG_WIDTH;
    unsigned int src_addr   : SADDR_WIDTH;
    unsigned int opcode     : OPCODE_WIDTH;
    unsigned int group      : GROUP_WIDTH;
    unsigned int rnd        : RND_WIDTH;
    unsigned int not_in_use : NIU_WIDTH;
} myHashTable;

typedef struct commandTables {
	char *command;
	int opcode;
	int srcOperations;
	int destOperations;

} myCommandTable;

typedef struct dataTables {
	int dc;
	unsigned int data;
	struct dataTables* next;
} myDataTable;

int symIsUpper(char* str);
void first_parsing_line (char *line, int count);
void init_command_table();
void strip_extra_spaces (char* str);
int hasSymbol(char* str);
char *hasDot(char* str);
int extractData(char* str, char * type);
char *getSymbol(char* str, int pos);
char *getNextString(char* str);
char *hasQM(char* str);
int extractOperands(char *str, int opcode);
int findCommand(char *command);
int findDuplicateSym(mySymbolList *symbolList,char *sym);
int recognizeOperand(char *str);
int validOperOpcode(int opcode, int srcAddr, int destAddr);

mySymbolList *createSymbolNode (char* str, unsigned int dc, int external, int action);
mySymbolList *addSymbolNode (mySymbolList* symbolList, char* str, unsigned int dc, int external, int action);
myDataTable *createDataNode (int dc, unsigned int data);
myDataTable *addDataNode (myDataTable* dataTable, int dc, unsigned int data);

char *to_base(int num, int base, char *result, int pad)
{
	int index = 0, i;
	char tmp[MAX_DIGIT + 1];
	for (i = 0; i < MAX_DIGIT; i++) /* reset all the cells */
	{
		result[i] = '0';
		tmp[i] = '0';
	}

	while (num != 0)
	{
		tmp[index] = (num % base) + '0';
		num /= base;
		index++;
	}

	for (i = 0; i < MAX_DIGIT; i++)
	{
		result[i] = tmp[MAX_DIGIT - 1 - i];
	}
    result[i] = '\0';

    if (pad)
    {
        return result;
    }
    else
    {
        return result + MAX_DIGIT - index;
    }
}
