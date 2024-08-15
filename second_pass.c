#include <string.h>
#include <stdlib.h>
#include "structs.h"
#include "syntax.h"
#include "second_pass.h"
#define ADDRESS_LEN 4
#define OCTAL_LEN 5
#define MEM_START 100
#define OCTAL_MASK 0x7FFF
#define MAX_LINE 82
#define SKIP_ARE(x) (x<<3)
#define OPCODE(x) (x<<11)
#define TARGET_METHOD(x) (1<<(x+3))
#define SOURCE_METHOD(x) (1<<(x+7))
#define TARGET(x) (x<<3)
#define SOURCE(x) (x<<7)
#define SOURCE_REG(x) (x<<6)
#define A_BIT 4
#define R_BIT 2
#define E_BIT 1


int second_pass(char* fileName, firstPassRes* results) {
	cmd* lines = results->lines;
	label_table* table = results->table;
	int i = 0, line, IC = results->IC, addRes1, addRes2, operands, index = 0;
	char lineArr[MAX_LINE], **words;
	short code, *machine;
	label_node* lnode;
	
	machine = (short*) malloc(sizeof(short)*(IC-100));
	
	if(hasEnt(table)) {	
		createEntFile(fileName, table);
	}
	
	if(hasExt(table)) {
		createExtFile(fileName, table);
	}
	
	while(lines[i].content != NULL) {
		line = lines[i].line_num;
		code = 0;
		strcpy(lineArr, lines[i].content);
		words = split(lineArr);
		operands = strArrLen(words) - 1;
		
		code += OPCODE(asmOpcodes[isOpcode(words[0], line, fileName)].code);
		
		if(operands == 1) {
			addRes1 = addressingMethod(words[1],line, fileName, table);
			code += TARGET_METHOD(addRes1);
			addRes2 = -1;
		}
		if(operands == 2) {
			addRes1 = addressingMethod(words[1],line, fileName, table);
			code += SOURCE_METHOD(addRes1);
			addRes2 = addressingMethod(words[2],line, fileName, table);
			code += TARGET_METHOD(addRes2);
		}
		code += A_BIT; /* A.R.E FIELD IS ALWAYS 'A' BIT ON IN THE FIRST WORD */
		machine[index++] = code;
		
		code = 0;
		if(operands > 0) {
			switch(addRes1) {
				case 0: /* IMMEDIATE VALUE */
					code += SKIP_ARE((short) isNumber(words[1]));
					code += A_BIT;
					break;
				case 1:
					lnode = find_label(words[1], table);
					if(strcmp(lnode->type, "ext") == 0) {
						code = E_BIT;
					}
					else {
						code = R_BIT;
						code += SKIP_ARE(lnode->address);
					}
					break;
				case 2:
					if(addRes2 == 2) {
						code += SOURCE_REG(isRegisterPtr(words[1]));
						code += TARGET(isRegisterPtr(words[2]));
					}
					else if(addRes2 == 3) {
						code += SOURCE_REG(isRegisterPtr(words[1]));
						code += TARGET(isRegister(words[2]));
					}
					else if(addRes2 == -1) {
						code += TARGET(isRegisterPtr(words[1]));
					}
					else {
						code += SOURCE_REG(isRegisterPtr(words[1]));
					}
					code += A_BIT;
					break;
				case 3:
					if(addRes2 == 2) {
						code += SOURCE_REG(isRegister(words[1]));
						code += TARGET(isRegisterPtr(words[2]));
					}
					else if(addRes2 == 3) {
						code += SOURCE_REG(isRegister(words[1]));
						code += TARGET(isRegister(words[2]));
					}
					else if(addRes2 == -1) {
						code += TARGET(isRegister(words[1]));
					}
					else {
						code += SOURCE_REG(isRegister(words[1]));
					}
					code += A_BIT;
					break;
				default:
					error(44, line, fileName);
					break;
			}
		}
		
		if(code != 0) {
			machine[index++] = code;
		}
		
		code = 0;
		if(operands == 2) {
			switch(addRes2) {
				case 0: /* IMMEDIATE VALUE */
					code += SKIP_ARE((short) isNumber(words[2]));
					code += A_BIT;
					break;
				case 1:
					lnode = find_label(words[2], table);
					if(strcmp(lnode->type, "ext") == 0) {
						code = E_BIT;
					}
					else {
						code = R_BIT;
						code += SKIP_ARE(lnode->address);
					}
					break;
				case 2:
					if(addRes1 != 2 && addRes1 != 3) {
						code += TARGET(isRegisterPtr(words[2]));
						code += A_BIT;
					}
					break;
				case 3:
					if(addRes1 != 2 && addRes1 != 3) {
						code += TARGET(isRegister(words[2]));
						code += A_BIT;
					}
					break;
				default:
					error(44, line, fileName);
					break;
			}
		}
		if(code != 0) {
			machine[index++] = code;
		}
		
		i++;
		splitFree(words);
	}
	
	createObFile(fileName, results->strlist, results->datalist, results->IC, results->DC, machine);
	
	freeResults(results);
	printf("second pass success\n");
	return 0;
}

int createEntFile(char* fileName, label_table *table) {
	char* ent_name;
	FILE *fent;
	int i, longLabel;
	label_node *node;
	
	ent_name = (char*) malloc(sizeof(char)*(strlen(fileName) + 1));
	
	if(ent_name == NULL) { 
		error(3, -1, fileName);
		exit(EXIT_FAILURE);
	}
	
	strcpy(ent_name, fileName);
	ent_name[strchr(ent_name, '.') - ent_name] = '\0';
	strcat(ent_name, ".ent");
	
	fent = fopen(ent_name, "w");
	free(ent_name);
	
	if(fent == NULL) {
		error(10, -1, fileName);
		return -1;
	}
	
	node = table->head;
	longLabel = 0;
	while(node != NULL) {
		if(strcmp(node->type, "ent") == 0 && strlen(node->name) > longLabel) {	
			longLabel = strlen(node->name);
		}
		node = node->next;
	}
	node = table->head;
	while(node != NULL) {
		if(strcmp(node->type, "ent") == 0) {
			if(node->address > 0) {
				fprintf(fent, "%s ", node->name);
				for(i = strlen(node->name); i < longLabel; i++) {
					fprintf(fent, " ");
				}
				for(i = len(node->address); i < ADDRESS_LEN; i++) {
					fprintf(fent, "0");
				}
				fprintf(fent, "%d\n", node->address);
			}
			else {
				error(33, -node->address, fileName);
				removeLabel(node, table);
			}
		}
		node = node->next;
	}
	fclose(fent);
	return 0;
}

int createExtFile(char* fileName, label_table *table) {
	char* ext_name;
	FILE *fext;
	int i, longLabel;
	label_node *node;
	
	ext_name = (char*) malloc(sizeof(char)*(strlen(fileName) + 1));
	
	if(ext_name == NULL) { 
		error(3, -1, fileName);
		exit(EXIT_FAILURE);
	}
	
	strcpy(ext_name, fileName);
	ext_name[strchr(ext_name, '.') - ext_name] = '\0';
	strcat(ext_name, ".ext");
	
	fext = fopen(ext_name, "w");
	free(ext_name);
	
	if(fext == NULL) {
		error(10, -1, fileName);
		return -1;
	}
	
	node = table->head;
	longLabel = 0;
	while(node != NULL) {
		if(strcmp(node->type, "ext") == 0 && strlen(node->name) > longLabel) {	
			longLabel = strlen(node->name);
		}
		node = node->next;
	}
	node = table->head;
	while(node != NULL) {
		if(strcmp(node->type, "ext") == 0) {
			fprintf(fext, "%s\t", node->name);
			for(i = strlen(node->name); i < longLabel; i++) {
				fprintf(fext, " ");
			}
			for(i = len(node->address); i < ADDRESS_LEN; i++) {
				fprintf(fext, "0");
			}
			fprintf(fext, "%d\n", node->address);
		}
		node = node->next;
	}
	fclose(fext);
	return 0;
}

int createObFile(char* fileName, string_list* strlist, data_list* datalist, int IC, int DC, short *machine) {
	char* ob_name;
	FILE *fob;
	int i, k;
	string_node *strnode = strlist->head;
	data_node *datnode = datalist->head;
	
	ob_name = (char*) malloc(sizeof(char)*(strlen(fileName) + 1));
	
	if(ob_name == NULL) { 
		error(3, -1, fileName);
		exit(EXIT_FAILURE);
	}
	
	strcpy(ob_name, fileName);
	ob_name[strchr(ob_name, '.') - ob_name] = '\0';
	strcat(ob_name, ".ob");
	
	fob = fopen(ob_name, "w");
	free(ob_name);
	
	if(fob == NULL) {
		error(10, -1, fileName);
		return -1;
	}
	
	IC -= MEM_START;
	
	for(i = len(IC); i <= ADDRESS_LEN; i++) {
		fprintf(fob, " ");
	}
	fprintf(fob, "%d %d\n", IC, DC);
	
	for(i = 0; i < IC; i++) {
		fprintf(fob, "%.4d ", i+MEM_START);
		if(machine[i] < 0) {
			fprintf(fob, "%.5o\n", (unsigned int) machine[i] & OCTAL_MASK);
		}
		else {
			fprintf(fob, "%.5o\n", machine[i]);
		}
	}
	for(i = 0; i < DC; i++) {
		if(strnode != NULL && datnode != NULL) {
			if(datnode->address < strnode->address) {
				fprintf(fob, "%.4d ", i+IC+MEM_START);
				if(datnode->value < 0) {
					fprintf(fob, "%.5o\n", (unsigned int) datnode->value & OCTAL_MASK);
				}
				else {
					fprintf(fob, "%.5o\n", datnode->value);
					datnode = datnode->next;
				}
			}
			else {
				for(k = 0; k < strlen(strnode->value); k++) {
					fprintf(fob, "%.4d %.5o", i+IC+MEM_START, strnode->value[k]);
					i++;
				}
				fprintf(fob, "%.4d %.5o", i+IC+MEM_START, '\0');
				strnode = strnode->next;
			}
		}
	}
	
	
	fclose(fob);
	return 0;
}
