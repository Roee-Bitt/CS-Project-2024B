#ifndef SYNTAX_H
#define SYNTAX_H
#define ASM_RES_COUNT 30
#define ASM_OP_COUNT 16
#define REGS_COUNT 8
#include "structs.h"
void error(int num, int lineNum, char* fileName);
char* substring(char* str, char *dest, int len);
int paramSyntax(char* line, int lineNum, char* fileName);
char* extractParam(char* line);
char* getNext(char* line, char* dest);
int isWhite(char c);
int isWhiteExt(char c);
int isEmpty(char* line);
char* replace(char* str, char* old, char *replace);
int isReserved(char* name);
int isOpcode(char* word, int line, char* fileName);
int isRegister(char* line);
int isRegisterPtr(char* line);
int isNumber(char* line);
int isLabel(char* line, label_table* table);
int addressingMethod(char* line, int address, char* fileName, label_table* table);
int validAddressMethod(int opcode, int operand, char* line, label_table* table);
char** split(char str[]);
void splitFree(char** arr);	
int lenStrArr(char** array);
int lenDataArr(int array[]);
int data_syn(char* line, char* fileName, int address);
int* extract(char* line, char* fileName, int address);
int len(int num);
int strArrLen(char** arr);
int label_syn(char* label, char* fileName, int line, macro_list* macrs);
int string_syn(char* string, char* fileName, int line);
int toOctal(int x);

extern const char* asmReserved[];
extern const char* asmRegisters[];
extern const opcode asmOpcodes[];

#endif
