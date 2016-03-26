/*
 * struct.h
 *
 *  Created on: 26 במרץ 2016
 *      Author: Dror
 */

#ifndef STRUCT_H_
#define STRUCT_H_

#define MAX_LINE 80
#define BUF_SIZE 100
#define MAX_ARRAY_SIZE 1000
#define IC_MEM_ALLOCATION 100
#define MAX_DIGIT 15
#define MAX_BASE32_DIGIT 3

#define ERA_WIDTH 2
#define DREG_WIDTH 6
#define DADDR_WIDTH 2
#define SREG_WIDTH 6
#define SADDR_WIDTH 2
#define OPCODE_WIDTH 4
#define GROUP_WIDTH 2
#define RND_WIDTH 2
#define NIU_WIDTH 1
#define DATA_WIDTH 13
#define DATANUM_WIDTH 13

#define PADDING_ENABLED 1
#define PADDING_DISABLED 0
#define COMMAND_SIZE 16

typedef struct symbolLists {
	char *Sym;
	unsigned int addr;
	int ext;
	int action;
	struct symbolLists* next;
} mySymbolList;

typedef struct hashTables {
	int addr;
    unsigned int era        : ERA_WIDTH;
    unsigned int dest_reg   : DREG_WIDTH;
    unsigned int dest_addr  : DADDR_WIDTH;
    unsigned int src_reg    : SREG_WIDTH;
    unsigned int src_addr   : SADDR_WIDTH;
    unsigned int opcode     : OPCODE_WIDTH;
    unsigned int group      : GROUP_WIDTH;
    unsigned int rnd        : RND_WIDTH;
    unsigned int data		: DATA_WIDTH;
    int datanum				: DATANUM_WIDTH;
    unsigned int not_in_use : NIU_WIDTH;
    char *binaryData;
    char *base32;
} myHashTable;

typedef struct dataTables {
	int dc;
	int data;
	char *binaryData;
	char *base32;
	struct dataTables* next;
} myDataTable;

typedef struct commandTables {
	char *command;
	int opcode;
	int srcOperations;
	int destOperations;

} myCommandTable;

char *parseProjectName(char* project);
int findExistingSym(mySymbolList *symbolList,char *sym, char *type);
int findCommand(char *command);
int hasSymbol(char* str);
char *getNextString(char* str);
char *getSymbol(char* str, int pos);

#endif /* STRUCT_H_ */
