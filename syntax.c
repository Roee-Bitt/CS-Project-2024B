/*SOURCE FILE FOR SYNTAX*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "structs.h"
#include "syntax.h"
#define MAX_LABEL 32
#define MAX_LINE 82
#define INITIAL_SIZE 2
#define END_OF_DATA 999999999
#define MIN_NUMBER (-pow(2,14))
#define MAX_NUMBER (pow(2,14)-1)
#define SKIP_WHITES \
while(isWhite(c = *lineptr)) {\
	lineptr++;\
}

const char* asmReserved[] = {".data", ".string", ".entry", ".extern", "mov", "cmp",
				"add", "sub", "lea", "clr", "not", "inc", "dec", "jmp",
				"bne", "red", "prn", "jsr", "rts", "stop", "r0", "r1",
				"r2", "r3", "r4", "r5", "r6", "r7", "macr", "endmacr"};
				
const char* asmRegisters[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

const opcode asmOpcodes[] = {{"mov", 0}, {"cmp", 1}, {"add", 2}, {"sub", 3}, {"lea", 4}, {"clr", 5}, {"not", 6}, {"inc", 7},
			 {"dec", 8}, {"jmp", 9}, {"bne", 10}, {"red", 11}, {"prn", 12}, {"jsr", 13}, {"rts", 14}, {"stop", 15}};

/* GETS CODE OF ERROR AND PRINTS ERROR ACCORDINGLY */
void error(int num, int lineNum, char* fileName) {
	if(lineNum != -1) { printf("\n**ERROR** %s(%d): ", fileName, lineNum+1); }
	else { printf("\n**ERROR** %s: ", fileName); }
	switch(num){
		case 1:
			printf("Macro already defined elsewhere\n");
			break;
		case 2:
			printf("Macro name too long (31 characters max)\n");
			break;
		case 3:
			printf("Failed to allocate memory\n");
			break;
		case 4:
			printf("Error occurred when deploying macros\n");
			break;
		case 5:
			printf("Line too long (80 characters max)\n");
			break;
		case 6:
			printf("Error occurred when reading file\n");
			break;
		case 7:
			printf("Extra characters on line\n");
			break;
		case 8:
			printf("Illegal macro name\n");
			break;
		case 9:
			printf("Unable to access file or file does not exist\n");
			break;
		case 10:
			printf("Error occurred when creating file\n");
			break;
		case 11:
			printf("Illegal character on line\n");
			break;
		case 12:
			printf("Label name too long (31 characters max)\n");
			break;
		case 13:
			printf("Label name cannot contain blank characters\n");
			break;
		case 14:
			printf("Label name cannot contain special characters (alphanumeric only)\n");
			break;
		case 15:
			printf("Label name has to start with a letter\n");
			break;
		case 16:
			/*printf("Label name cannot be a reserved word (instruction, register, etc.)\n");*/
			printf("Reserved word used as a label\n");
			break;
		case 17:
			printf("Label name already defined as a macro\n");
			break;
		case 18:
			printf("Label already defined elsewhere\n");
			break;
		case 19:
			printf("Label cannot point to an empty line\n");
			break;
		case 20:
			printf("Must be a blank space after label definition\n");
			break;
		case 21:
			printf("Undefined instruction\n");
			break;
		case 22:
			printf("Cannot define more than one label in one line\n");
			break;
		case 23:
			printf("Missing parameter\n");
			break;
		case 24:
			printf("Label cannot be defined as entry and extern at the same file\n");
			break;
		case 25:
			printf("Missing opening quotation marks on string definiton\n");
			break;
		case 26:
			printf("Missing closing quotation marks on string definiton\n");
			break;
		case 27:
			printf("Invalid data - not an integer\n");
			break;
		case 28:
			printf("Multiple consecutive commas on data definition\n");
			break;
		case 29:
			printf("Illegal comma on start of data\n");
			break;
		case 30:
			printf("Missing comma between elements\n");
			break;
		case 31:
			printf("Invalid use of '+' or '-'\n");
			break;
		case 32:
			printf("Value out of range (valid range is -16,384 to 16,383)\n");
			break;
		case 33:
			printf("Undefined label - must have value\n");
			break;
		case 34:
			printf("Error occurred when closing file\n");
			break;
		case 35:
			printf("Illegal register name - does not exist\n");
			break;
		case 36:
			printf("Illegal instruction - does not exist\n");
			break;
		case 37:
			printf("Too few operands to instruction\n");
			break;
		case 38:
			printf("Undefined symbol\n");
			break;
		case 39:
			printf("Invalid source operand type to instruction\n");
			break;
		case 40:
			printf("Invalid target operand type to instruction\n");
			break;
		case 41:
			printf("Invalid number\n");
			break;
		case 42:
			printf("Illegal comma after instruction\n");
			break;
		case 43:
			printf("Invalid syntax\n");
			break;
		case 44:
			printf("Error occurred when translating to machine language\n");
			break;
		case 45:
			printf("\n");
			break;
		case 46:
			printf("\n");
			break;
		case 47:
			printf("\n");
			break;
		case 48:
			printf("\n");
			break;
		case 49:
			printf("\n");
			break;
		case 50:
			printf("\n");
			break;
		case 51:
			printf("\n");
			break;
		case 52:
			printf("\n");
			break;
		case 53:
			printf("\n");
			break;
		case 54:
			printf("\n");
			break;
	}
}

int isWhite(char c) {
	return c == ' ' || c == '\t';
}

int isWhiteExt(char c) {
	return c == ' ' || c == '\t' || c == '\n';
}

char* getNext(char* line, char* dest) {
	char c = *line;
	int i = 0;
	while(isWhite(c) && c != '\0') { c = *++line; }
	while(!isWhiteExt(c) && c != '\0') {
		dest[i++] = c;
		if(c == ',') {
			line++;
			break;
		}
		c = *++line;
	}
	dest[i] = '\0';
	return line;
}

/* RETURNS SUBSTRING FROM INDEX 0 TO LEN - 1 */
char* substring(char* str, char *dest, int len) {
	int index = 0;
	while(index < len && str[index] != '\0') {
		dest[index] = str[index];
		index++;
	}
	dest[index] = '\0';
	return dest;
}

int isEmpty(char* line) {
	char c;
	c = *line;
	while(c != '\n' && c != '\0') {
		if(!isWhite(c)) {
			return 0;
		}
		c = *++line;
	}
	return 1;
}

/* REPLACES FIRST OCCURENCE OF old IN str TO new (LIMITED TO old LENGTH) */
char* replace(char* str, char* old, char* new) {
	char *newptr, *ptr;
	
	if(!(ptr = strstr(str,old))) {
		return str;
	}
	
	newptr = new;
	ptr--;
	do {
		ptr++;
		if(*newptr == '\0') {
			*ptr = *(ptr+strlen(old));
		}
		else {
			*ptr = *newptr++;
		}
	} while(*ptr != '\0');
	
	return str;
}

int isReserved(char* name) {
	int i, count_res = 30;
	for(i = 0; i < count_res; i++) {
		if(strcmp(asmReserved[i], name) == 0) {
			return 1;
		}
	}
	return 0;
}

int isOpcode(char* word, int line, char* fileName) {
	int i;
	if(strchr(word, ',') != NULL) {
		error(42, line, fileName);
		replace(word, ",", "");
	}
	for(i = 0; i < ASM_OP_COUNT; i++) {
		if(strcmp(word, asmOpcodes[i].name) == 0) {
			return i;
		}
	}
	return -1;
}

int isRegister(char* line) {
	int i;
	for(i = 0; i < REGS_COUNT; i++) {
		if(strcmp(asmRegisters[i], line) == 0) {
			return i;
		}
	}
	if(line[0] == 'r') {
		for(i = 1; i < strlen(line); i++) {
			if(!isdigit(line[i])) {
				return -1;
			}
		}
		return REGS_COUNT;
	}
	return -1;
}

int isRegisterPtr(char* line) {
	int i;
	if(line[0] == '*') {
		for(i = 0; i < REGS_COUNT; i++) {
			if(strcmp(asmRegisters[i], line+1) == 0) {
				return i;
			}
		}
	}
	if(line[1] == 'r') {
		for(i = 2; i < strlen(line); i++) {
			if(!isdigit(line[i])) {
				return -1;
			}
		}
		return REGS_COUNT;
	}
	return -1;
}

int isNumber(char* line) {
	int i = 1;
	char* ptr;
	if(line[0] != '#') {
		return pow(2,12);
	}
	if(line[1] != '+' && line[1] != '-' && !isdigit(line[1])) {
		return -pow(2,12);
	}
	else if(line[1] == '+' || line[1] == '-') {
		i++;
	}
	
	for(; i < strlen(line); i++) {
		if(!isdigit(line[i]) || isWhite(line[i])) {
			return -pow(2,12);
		}
	}
	return (int)strtol(line+1, &ptr, 10);
	
}

int isLabel(char* line, label_table* table) {
	label_node *node;
	node = table->head;
	while(node != NULL) {
		if(strcmp(line, node->name) == 0) {
			return 1;
		}
		node = node->next;
	}
	return 0;
}

int addressingMethod(char* line, int address, char* fileName, label_table* table) {
	int res;
	if(strchr(line, ',') != NULL) {
		replace(line, ",", "");
	}
	if((res = isNumber(line)) != pow(2,12) && res !=(-pow(2,12))) {
		return 0;
	}
	else if(res == -pow(2,12)) {
		error(41, address, fileName);
		return -1;
	}
	else if(isLabel(line, table)) {
		return 1;
	}
	else if((res = isRegisterPtr(line)) < REGS_COUNT && res != -1) {
		return 2;
	}
	else if(res == REGS_COUNT) {
		error(35, address, fileName);
		return -1;
	}
	else if((res = isRegister(line)) < REGS_COUNT && res != -1) {
		return 3;
	}
	else if(res == REGS_COUNT) {
		error(35, address, fileName);
		return -1;
	}
	return 10;
}

int validAddressMethod(int opcode, int operand, char* line, label_table* table) {
	int res = addressingMethod(line, -1, NULL, table);
	switch(opcode) {
		case 0:
		case 2: /* mov, add, sub HAS THE SAME VALID ADDRESSING METHODS */
		case 3:
			if(operand == 1) {
				return 1;
			}
			else {
				return res != 0;
			}
			break;
		case 1: /* cmp */
			return 1;
			break;
		case 4:
			if(operand == 1) {
				return res == 1 || res == 10;
			}
			else {
				return res != 0;
			}
			break;
		case 5:
		case 6:
		case 7: /* clr, not, inc, dec, red HAS THE SAME VALID ADDRESSING METHODS */
		case 8:
		case 11:
			return res != 0;
			break;
		case 9:
		case 10: /* jmp, bne, jsr HAS THE SAME VALID ADDRESSING METHODS */
		case 13:
			return res == 1 || res == 2 || res == 10;
			break;
		case 12: /* prn */
			return 1;
			break;
		default:
			return -1;
			break;
			
	}
}

/* SYNTAX FOR LABELS */
int label_syn(char* label, char* fileName, int line, macro_list* macrs) {
	char *c;
	
	c = label;
	while(!isWhite(*c) && *c != ':') { c++; }
	if(*c != ':') {
		error(13, line, fileName);
		return -1;
	}
	
	c = label;
	while(isalnum(*c) && *c != ':') { c++; }
	if(*c != ':') {
		printf("%d\n", *c);
		error(14, line, fileName);
		return -1;
	}
	
	c = label;
	if(!isalpha(*c)) {
		error(15, line, fileName);
		return -1;
	}
	
	replace(label, ":", "");
	if(isReserved(label)) {
		error(16, line, fileName);
		return -1;
	}
	
	if(contains_macr(label, macrs) == 2) {
		error(17, line, fileName);
		return -1;
	}
	
	return 0;
}

/* SYNTAX FOR STRINGS DECLARATIONS */
int string_syn(char* string, char* fileName, int line) {
	char *c, *qmark_ptr, space[2];
	int count = 0;
	space[1] = '\0';
	c = string;
	while(isWhite(*c)) { /* KNOWING THERE"S A NEWLINE CHARACTER AT THE END THAT WILL STOP */
		space[0] = *c;
		replace(string, space, "");
		c++;
	}
	if(*c != '\"') {
		error(25, line, fileName);
		return -1;
	}
	while(*c != '\n') {
		if(*c == '\"') { 
			count++;
			qmark_ptr = c;
		}
		c++;
	}
	
	replace(string, "\"", "");
	
	if(count == 1) {
		error(26, line, fileName);
		return -1;
	}
	if(!isEmpty(qmark_ptr)) { /* CHARACTERS AFTER LAST QUOTATION MARK */
		error(7, line, fileName);
		return -1;
	}
	
	*(qmark_ptr-1) = '\0';
	
	return 0;
}

char** split(char str[]) {
	char *word, **arr;
	int index = 0;
	word = (char*) malloc(sizeof(char)*MAX_LINE);
	arr = (char**) malloc(sizeof(char*)*(MAX_LINE)); /* MAX AMOUNT OF WORDS */
	
	if(word == NULL || arr == NULL) { fprintf(stderr, "Memory allocation failed\n"); exit(EXIT_FAILURE); }
	
	while(!isEmpty((str = getNext(str, word)))) {
		arr[index] = (char*) malloc(sizeof(char)*(strlen(word)+1));
		strcpy(arr[index], word);
		index++;
	}
	
	arr[index] = (char*) malloc(sizeof(char)*(strlen(word)+1));
	strcpy(arr[index], word);
	index++;
	arr[index] = "\0";
	
	arr = (char**) realloc(arr, sizeof(char*)*(index+1));
	
	if(arr == NULL) { fprintf(stderr, "Memory allocation failed\n"); exit(EXIT_FAILURE); }
	
	free(word);
	
	return arr;
}

void splitFree(char** arr) {
	int i = 0;
	while(strcmp(arr[i], "\0") != 0) {
		free(arr[i]);
		i++;
	}
	free(arr);
}

int lenDataArr(int array[]) {
	int count = 0, index = 0;
	while(array[index] != END_OF_DATA) {
		count++;
		index++;
	}
	return count;
}

/* CHECKS PARAMETERS LIST'S SYNTAX AND ERRORS ACCORDINGLY */
int data_syn(char* line, char* fileName, int address) {
	int index = 0, commaFlag = 0, numDetect = 0, needComma = 0;
	char* token;
	int c = line[index];
	while(c != '\n') {
		if(ispunct(c) && c != ',' && c != '-' && c != '+') {
			error(11,address,fileName);
			return -1;
		}
		if((isalpha(c) || !isdigit(c)) && c != ',' && !isWhite(c) && c != '-' && c != '+') {
			error(27,address,fileName);
			return -1;
		}
		c = line[++index];
	}
	index = 0;
	c = line[index];
	while(c != '\0') {
		if(c == ',' && !numDetect) {
			error(29,address,fileName);
			return -1;
		}
		if(c == ',' && !needComma) {
			error(28,address,fileName);
			return -1;
		}
		if(c == ',') {
			needComma = 0;
			commaFlag = 1;
		}
		if(isdigit(c) && !needComma) {
			numDetect = 1;
			commaFlag = 0;
			if(isWhite(line[index+1]) || line[index+1] == ',') {
				needComma = 1;
			}
		}
		else if(isdigit(c)) {
			error(30,address,fileName);
			return -1;
		}
		c = line[++index];
	}
	
	if(commaFlag) {
		error(7,address,fileName);
		return -1;
	}
	
	index = 0;
	token = strtok(line, ",");
	while(token != NULL) {
		c = token[index];
		while(isWhite(c)) { c = token[++index]; }
		if((c == '+' || c == '-') && isWhite(token[index+1])) {
			error(31,address,fileName);
			return -1;
		}
		token = strtok(NULL, ",");
	}
	
	return 0;
}

/* CONVERTING LINE OF CHARACTERS INTO ARRAY OF INTS */
int* extract(char* line, char* fileName, int address) {
	int index = 0;
	char c = line[0];
	int space = INITIAL_SIZE, num = 0;
	int *nums, *temp;
	char *ptr, *commaptr, *sub;
	nums = (int*) malloc(sizeof(int)*INITIAL_SIZE); /* ALLOCATE A NEW INT ARRAY MEMORY */
	if(nums == NULL) { fprintf(stderr, "Memory allocation failed\n"); exit(EXIT_FAILURE); }
	while(c != '\0') { /* COPY CHARACTERS UNTIL END OF WORD OR LINE */
		if(index >= space - 1) { /* RE-ALLOCATE MEMORY AS MUCH AS NEEDED */
		      	space++;
		      	temp = realloc(nums, sizeof(int) * space);
		      	if(temp == NULL) { fprintf(stderr, "Memory allocation failed\n"); exit(EXIT_FAILURE); }
			nums = temp;
    		}
    		commaptr = strchr(line, ',');
    		if(commaptr == NULL) {
    			commaptr = strchr(line, '\0');
    			sub = (char*) malloc(sizeof(char)*(commaptr - line));
	    		if(sub == NULL) { fprintf(stderr, "Memory allocation failed\n"); exit(EXIT_FAILURE); }
	    		substring(line, sub, commaptr - line);
	    		num = (int)strtol(sub,&ptr,10);
			free(sub);
			if(num < MIN_NUMBER || num > MAX_NUMBER) {
    				error(32,address,fileName);
    				return (int*)-1;
    			}
    			nums[index++] = num;
    			nums[index] = END_OF_DATA;
    			return nums;
    		}
    		sub = (char*) malloc(sizeof(char)*(commaptr - line));
    		if(sub == NULL) { fprintf(stderr, "Memory allocation failed\n"); exit(EXIT_FAILURE); }
    		substring(line, sub, commaptr - line);
    		num = (int)strtol(sub,&ptr,10);
		free(sub);
		line = line + (commaptr - line) + 1; /* IF WE HAVNE'T REACHED THE LAST ONE */
		c = line[0];
    		if(num < MIN_NUMBER || num > MAX_NUMBER) {
    			error(32,address,fileName);
    			return (int*)-1;
    		}
    		nums[index++] = num;
	}

	return nums;	
}

void print(char** line) {
	int i = 0;
	printf("\n[");
	while(strcmp(line[i], "\0") != 0) {
		printf("\"%s\", ", line[i]);
		i++;
	}
	printf("\"%s\"]\n", line[i]);
}

int len(int num) {
	int count = 0;
	if(num == 0) {
		return 1;
	}
	while(num > 0) {
		count++;
		num /= 10;
	}
	return count;
}

int strArrLen(char** arr) {
	char* word;
	int index = 0;
	word = arr[index];
	while(strcmp(word, "\0") != 0) {
		word = arr[++index];
	}
	return index;
}

int toOctal(int x) {
	int res = 0, digit = 1, rem;
	while(x > 0) {
		rem = x % 8;
		res += rem * digit;
		x /= 8;
		digit *= 10;
	}
	return res;
}
