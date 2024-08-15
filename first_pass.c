#include "syntax.h"
#include "structs.h"
#include "first_pass.h"
#include <stdlib.h>
#include <string.h>
#define MAX_LINE 82
#define MAX_LABEL 32
#define TWO_OPERAND 4
#define ONE_OPERAND 13
#define SKIP_WHITES \
while(isWhite(c = *line)) {\
	line++;\
}

firstPassRes* first_pass(char* fileName, macro_list* macrs) {
	FILE *fp;
	int i, count = 0, IC = 100, DC = 0, k, label_flag = 0, *dataArr, code, addRes1, addRes2, index = 0, err_flag = 0;
	char *line, *origin, c, *label, *ptr, *subline, **words, lineArr[MAX_LINE];
	cmd* lines;
	label_node *lnode;
	label_table *table;
	string_node *strnode;
	string_list *strlist, *undefined;
	data_node *dataNode;
	data_list *datalist;
	table = (label_table*) malloc(sizeof(label_table));
	table->head = table->tail = NULL;
	strlist = (string_list*) malloc(sizeof(string_list));
	strlist->head = strlist->tail = NULL;
	undefined = (string_list*) malloc(sizeof(string_list));
	undefined->head = undefined->tail = NULL;
	datalist = (data_list*) malloc(sizeof(data_list));
	datalist->head = datalist->tail = NULL;
	origin = line = (char*) malloc(sizeof(char)*MAX_LINE + 1);
	
	fp = fopen(fileName, "r");
	if(fp == NULL) {
		error(9, -1, fileName);
		return NULL;
	}
	
	while((c = getc(fp)) != EOF) { /* COUNT NUMBER OF LINES IN INPUT */
		if(c == '\n') {
			count++;
		}
	}
	
	if(fseek(fp, 0, SEEK_SET) != 0) { /* RESETTING fp TO START OF FILE */
		error(6, -1, fileName);
		fclose(fp);
		return NULL;
	}
	
	lines = (cmd*) malloc(sizeof(cmd) * (count+1));
	
	for(i = 0; i < count; i++) {
		line = origin;
		label_flag = 0;
		if(fgets(line, MAX_LINE, fp) != NULL) {
			if(isEmpty(line)) {
				continue;
			}
			
			if(strchr(line, '\n') == NULL) {
				error(5, i ,fileName);
				i--;
				continue;
			}
			
			SKIP_WHITES
			
			if(c == ';') {
				continue;
			}
			
			if(c != '.' && (ptr = strchr(line, ':')) != NULL) {
				if(isEmpty(ptr+1)) {
					error(19, i, fileName);
					continue;
				}
				if(!isWhite(*(ptr+1))) {
					error(20, i, fileName);
					continue;
				}
				if(ptr-line >= MAX_LABEL) {
					error(12, i, fileName);
					continue;
				}
				label = (char*) malloc(sizeof(char)*MAX_LABEL);
				if(label == NULL) { exit(EXIT_FAILURE); }
				substring(line, label, ptr-line + 1);
				if(strstr(line, ".entry") != NULL || strstr(line, ".extern") != NULL) {
					replace(label, ":", "");
					printf("**WARNING** %s(%d): Redundant label on line (%s)\n", fileName, i+1, label);
				}
				else if(label_syn(label,fileName,i,macrs) == 0) {
					if(strstr(line, ".string") == NULL && strstr(line, ".data") == NULL) {
						lnode = new_label_node(label, "local", "code", IC);
					}
					else {
						lnode = new_label_node(label, "local", "data", DC);
					}
					if(updateTable(table, lnode, i, fileName) == -1) {
						free(label);
						continue;
					}
				}
				else {
					free(label);
					continue;
				}
				line += strlen(label)+1;
				free(label);
				label_flag = 1;
			}
			SKIP_WHITES
			subline = (char*) malloc(sizeof(char)*MAX_LINE);
			if(subline == NULL) { exit(EXIT_FAILURE); }
			ptr = strchr(line, ' ');
			if(ptr == NULL) { ptr = strchr(line, '\t'); }
			if(ptr == NULL) { ptr = strchr(line,  '\n'); }
			substring(line, subline, ptr-line);
			
			/* .entry CASE */
			if(strcmp(subline, ".entry") == 0) {
				if(strchr(ptr, ',') != NULL) {
					error(22, i, fileName);
					free(subline);
					continue;
				}
				line = ptr;
				if(isEmpty(line)) {
					error(23, i, fileName);
					free(subline);
					continue;
				}
				SKIP_WHITES
				if((ptr = strchr(line, ' ')) != NULL) {
					if(!isEmpty(ptr)) {
						error(7, i, fileName);
						free(subline);
						continue;
					}
				}
				if((ptr = strchr(line, '\t')) != NULL) {
					if(!isEmpty(ptr)) {
						error(7, i, fileName);
						free(subline);
						continue;
					}
				}
				
				getNext(line, subline);
				strcat(subline, ":");
				
				if(label_syn(subline,fileName,i,macrs) == 0) {
					lnode = new_label_node(subline, "ent", "null", -i);
					if(updateTable(table, lnode, i, fileName) == -1) {
						free(subline);
						continue;
					}
				}
				else {
					free(subline);
					continue;
				}
				free(subline);
			}
			
			/*.extern CASE */
			else if(strcmp(subline, ".extern") == 0) {
				if(strchr(ptr, ',') != NULL) {
					error(22, i, fileName);
					free(subline);
					continue;
				}
				line = ptr;
				if(isEmpty(line)) {
					error(23, i, fileName);
					free(subline);
					continue;
				}
				SKIP_WHITES
				if((ptr = strchr(line, ' ')) != NULL) {
					if(!isEmpty(ptr)) {
						error(7, i, fileName);
						free(subline);
						continue;
					}
				}
				if((ptr = strchr(line, '\t')) != NULL) {
					if(!isEmpty(ptr)) {
						error(7, i, fileName);
						free(subline);
						continue;
					}
				}
				
				getNext(line, subline);
				strcat(subline, ":");
				
				if(label_syn(subline,fileName,i,macrs) == 0) {
					lnode = new_label_node(subline, "ext", "", -1);
					if(updateTable(table, lnode, i, fileName) == -1) {
						free(subline);
						continue;
					}
				}
				else {
					free(subline);
					continue;
				}
				free(subline);
			}
			
			/* .string CASE */
			else if(strcmp(subline, ".string") == 0) {
				line = ptr;
				if(isEmpty(line)) {
					error(23, i, fileName);
					free(subline);
					continue;
				}
				SKIP_WHITES
				strcpy(subline, line);
				if(string_syn(line,fileName,i) == 0) {
					strnode = new_str_node(line, DC);
					DC += strlen(line) + 1; /* FOR 0 TERMINATOR */
					addStrToEnd(strnode, strlist);
					if(!label_flag) {
						printf("\n**WARNING** %s(%d): Variable defined with no symbol pointing to it\n", fileName, i+1);
					}
				}
				free(subline);
			}
			
			/* .data CASE */
			else if(strcmp(subline, ".data") == 0) {
				line = ptr;
				if(isEmpty(line)) {
					error(23, i, fileName);
					free(subline);
					continue;
				}
				SKIP_WHITES
				strcpy(subline, line);
				if(data_syn(subline,fileName,i) == 0) {
					dataArr = extract(line,fileName,i);
					if(dataArr != (int*)-1) {
						for(k = 0; k < lenDataArr(dataArr); k++) {
							dataNode = new_data_node(dataArr[k], DC);
							DC++;
							addDataToEnd(dataNode, datalist);
						}
					
						if(!label_flag) {
							printf("\n**WARNING** %s(%d): Variable defined with no symbol pointing to it\n", fileName, i+1);
						}
					}
				}
				free(subline);
			}
			else {
				strcpy(lineArr, line);
				words = split(lineArr);
				if((code = isOpcode(words[0], i, fileName)) == -1) {
					if(isReserved(words[0])) {
						error(43, i, fileName);
						splitFree(words);
						continue;
					}
					else {
						error(36, i, fileName);
						splitFree(words);
						continue;
					}
				}
				if(code <= TWO_OPERAND) {
					if(strArrLen(words) < 3) {
						error(37, i, fileName);
						splitFree(words);
						continue;
					}
					if(strArrLen(words) > 3) {
						error(7, i, fileName);
						splitFree(words);
						continue;
					}
					addRes1 = addressingMethod(words[1], i, fileName, table);
					if(addRes1 == -1) {
						splitFree(words);
						continue;
					}
					else if(addRes1 == 10) {
						strnode = new_str_node(words[1], IC + 1);
						addStrToEnd(strnode, undefined);
					}
					else {
						if(!validAddressMethod(code, 1, words[1], table)) {
							error(39, i, fileName);
							splitFree(words);
							continue;
						}
						if(addRes1 == 1) {
							lnode = find_label(words[1], table);
							if(strcmp(lnode->type, "ext") == 0) {
								lnode = new_label_node(words[1], "ext-use", "", IC + 1);
								updateTable(table, lnode, i, fileName);
							}
						}
					}
					addRes2 = addressingMethod(words[2], i, fileName, table);
					if(addRes2 == -1) {
						splitFree(words);
						continue;
					}
					else if(addRes2 == 10) {
						strnode = new_str_node(words[2], IC + 2);
						addStrToEnd(strnode, undefined);
					}
					else {
						if(!validAddressMethod(code, 2, words[2], table)) {
							error(40, i, fileName);
							splitFree(words);
							continue;
						}
						if(addRes2 == 1) {
							lnode = find_label(words[2], table);
							if(strcmp(lnode->type, "ext") == 0) {
								lnode = new_label_node(words[2], "ext-use", "", IC + 2);
								updateTable(table, lnode, i, fileName);
							}
						}
					}
					if((addRes1 == 2 || addRes1 == 3) && (addRes2 == 2 || addRes2 == 3)) {
						IC += 2;
					}
					else {
						IC += 3;
					}
					lines[index].content = (char*) malloc(sizeof(char)*strlen(line) + 1);
					strcpy(lines[index].content, line);
					lines[index].file_name = fileName;
					lines[index].line_num = i;
					index++;
				}
				else if(code <= ONE_OPERAND) {
					if(strArrLen(words) < 2) {
						error(37, i, fileName);
						splitFree(words);
						continue;
					}
					if(strArrLen(words) > 2) {
						error(7, i, fileName);
						splitFree(words);
						continue;
					}
					addRes1 = addressingMethod(words[1], i, fileName, table);
					if(addRes1 == -1) {
						splitFree(words);
						continue;
					}
					else if(addRes1 == 10) {
						strnode = new_str_node(words[1], IC + 1);
						addStrToEnd(strnode, undefined);
					}
					else {
						if(!validAddressMethod(code, 2, words[1], table)) {
							error(40, i, fileName);
							splitFree(words);
							continue;
						}
						if(addRes1 == 1) {
							lnode = find_label(words[1], table);
							if(strcmp(lnode->type, "ext") == 0) {
								lnode = new_label_node(words[1], "ext-use", "", IC + 1);
								updateTable(table, lnode, i, fileName);
							}
						}
					}
					IC += 2;
					lines[index].content = (char*) malloc(sizeof(char)*(strlen(line)+1));
					strcpy(lines[index].content, line);
					lines[index].file_name = fileName;
					lines[index].line_num = i;
					index++;
				}
				else {
					if(strArrLen(words) > 1) {
						error(7, i, fileName);
						splitFree(words);
						continue;
					}
					IC += 1;
					lines[index].content = (char*) malloc(sizeof(char)*(strlen(line)+1));
					strcpy(lines[index].content, line);
					lines[index].file_name = fileName;
					lines[index].line_num = i;
					index++;
				}
				splitFree(words);
			}
		}
	}
	
	line = origin;	
	fgets(line, MAX_LINE, fp); /* DEMO fgets TO GET TO EOF */
	if(!feof(fp)) {
		error(6,-1,fileName);
		return NULL;
	}
	
	free(origin);
	
	strnode = undefined->head;
	while(strnode != NULL) {
		if(!isLabel(strnode->value, table)) {
			error(38, strnode->address, fileName);
			err_flag = 1;
		}
		lnode = find_label(strnode->value, table);
		if(strcmp(lnode->type, "ext") == 0) {
			lnode = new_label_node(strnode->value, "ext-use", "", strnode->address);
			updateTable(table, lnode, strnode->address, fileName);
		}
		strnode = strnode->next;
	}
	
	if(err_flag == 1) {
		return NULL;
	}
	
	lnode = table->head;
	while(lnode != NULL) {
		if(strcmp(lnode->point, "data") == 0) {	
			lnode->address += IC;
		}
		lnode = lnode->next;
	}
	
	lines[index].content = NULL;
	
	fclose(fp);
	printf("firstpass success\n");
	return createResults(IC, DC, lines, strlist, datalist, table);
}
