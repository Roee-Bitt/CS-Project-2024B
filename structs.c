#include "structs.h"
#include "syntax.h"
#include <stdlib.h>
#include <string.h>

void printTxt(cmd *lines, int num) {
	int i;
	for(i = 0; i < num; i++) {
		printf("%s", lines[i].content);
	}
}

macro_node* new_node(char* name, char* cont, char* file_name, int line_num) {
	macro_node* node = (macro_node*) malloc(sizeof(macro_node));
	node->file_name = (char*) malloc(sizeof(char)*strlen(file_name));
	strcpy(node->file_name, file_name);
	node->line_num = line_num;
	node->macro_name = (char*) malloc(sizeof(char)*strlen(name)+1);
	strcpy(node->macro_name, name);
	node->macro_cont = (char*) malloc(sizeof(char)*strlen(cont)+1);
	strcpy(node->macro_cont, cont);
	node->next = NULL;
	return node;
}

void addToEnd(macro_node* node, macro_list* list) {
	if(list->head == NULL) {
		list->head = node;
		list->tail = node;
		return;
	}
	list->tail->next = node;
	list->tail = node;
}

int updateList(macro_list *list, macro_node *nodeptr) {
	macro_node *iterator_node;
	if(list->head == NULL) { /* IF LIST IS EMPTY */
		addToEnd(nodeptr, list);
		return 0;
	}
	iterator_node = list->head;
	while(iterator_node != NULL) {
		if(strcmp(iterator_node->macro_name, nodeptr->macro_name) == 0) {
			error(1, nodeptr->line_num, nodeptr->file_name);
			return 1;
		}
		iterator_node = iterator_node->next;
	}
	addToEnd(nodeptr, list);
	return 0;
}

void free_node(macro_node* node) {
	free(node->file_name);
	free(node->macro_name);
	free(node->macro_cont);
	free(node);
}

void free_list(macro_list* list) {
	macro_node *node;
	while(list->head != NULL) {
		node = list->head;
		list->head = list->head->next;
		free_node(node);
	}
	free(list);
}
 
int contains_macr(char* line, macro_list* macrs) {
	macro_node *node;
	int i;
	char *macr_ptr;
	node = macrs->head;
	while(node != NULL) {
		if((macr_ptr = strstr(line, node->macro_name)) != NULL && (isWhiteExt(macr_ptr[-1]) || macr_ptr[-1] == '\0') && (isWhiteExt(macr_ptr[strlen(node->macro_name)]) || macr_ptr[strlen(node->macro_name)] == '\0')) {
			if(strstr(line, "macr") != NULL) {
				return 1;
			}
			else {
				for(i = 0; i < strlen(line); i++) {
					if(!isWhite(line[i])) {
						if(memcmp(macr_ptr, line + i, strlen(node->macro_name)) == 0) {
							i += strlen(node->macro_name);
						}
						else {
							return 0;
						}
					}
				}
				return 2;
			}
		}
		node = node->next;
	}
	return 0;
}

macro_node* find_macr(char* line, macro_list* macrs) {
	macro_node *node = macrs->head;
	while(isWhite(*line)) { line++; }
	while(node != NULL) {
		if(memcmp(line, node->macro_name, strlen(node->macro_name)) == 0) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}

void free_lines(cmd* line) {
	int i = 0;
	while(line[i].content != NULL) {
		free(line[i].content);
		i++;
	}
}

int lenLines(cmd* line) {
	int i = 0;
	while(line[i].content != NULL) {
		i++;
	}
	return i;
}












label_node* new_label_node(char* name, char* type, char* point, int address) {
	label_node* node = (label_node*) malloc(sizeof(label_node));
	node->address = address;
	node->name = (char*) malloc(sizeof(char)*(strlen(name)+1));
	strcpy(node->name, name);
	node->type = (char*) malloc(sizeof(char)*(strlen(type)+1));
	strcpy(node->type, type);
	node->point = (char*) malloc(sizeof(char)*(strlen(point)+1));
	strcpy(node->point, point);
	node->next = NULL;
	return node;
}

void addLabelToEnd(label_node* node, label_table* table) {
	if(table->head == NULL) {
		table->head = node;
		table->tail = node;
		return;
	}
	table->tail->next = node;
	table->tail = node;
}

int updateTable(label_table *table, label_node *node, int line, char* fileName) {
	label_node *iterator_node;
	if(table->head == NULL) { /* IF TABLE IS EMPTY */
		addLabelToEnd(node, table);
		return 0;
	}
	iterator_node = table->head;
	while(iterator_node != NULL) {
		if(strcmp(iterator_node->name, node->name) == 0) {
			if(strcmp(iterator_node->type, "local") == 0 && strcmp(node->type, "ent") == 0) {
				strcpy(iterator_node->type, "ent");
				return 0;
			}
			else if(strcmp(iterator_node->type, "ent") == 0 && strcmp(node->type, "local") == 0) {
				if(iterator_node->address > 0) {
					error(18, line, fileName);
					return -1;
				}
				strcpy(iterator_node->point, node->point);
				iterator_node->address = node->address;
				return 0;
			}
			else if(strcmp(iterator_node->type, "ext") == 0 && strcmp(node->type, "ext-use") == 0) {
				strcpy(node->type, "ext");
				addLabelToEnd(node, table);
				if(iterator_node->address == -1) {
					removeLabel(iterator_node, table);
				}
				return 0;
			}
			else if(strcmp(iterator_node->type, node->type) == 0) {
				error(18, line, fileName);
				return -1;
			}
			else {
				error(24, line, fileName);
				return -1;
			}
		}
		iterator_node = iterator_node->next;
	}
	addLabelToEnd(node, table);
	return 0;
}

void removeLabel(label_node* node, label_table* table) {
	label_node* iterator = table->head;
	while(iterator != NULL && iterator->next != node) {
		iterator = iterator->next;
	}
	if(iterator == NULL) { /* SHOULDN'T HAPPEN SINCE WE GIVE IT VALID PARAMETERS, BUT ANY CASE */
		return;
	}
	iterator->next = node->next;
	free_label(node);
}

void free_label(label_node* node) {
	free(node->name);
	free(node->type);
	free(node->point);
	free(node);
}

void free_table(label_table* table) {
	label_node *node;
	while(table->head != NULL) {
		node = table->head;
		table->head = table->head->next;
		/*printf("%s %s %s %d\n", node->name, node->type, node->point, node->address);*/
		free_label(node);
	}
	free(table);
}

int contains_label(char* line, label_table* labels) {
	label_node *node;
	char *label;
	node = labels->head;
	while(node != NULL) {
		if((label = strstr(line, node->name)) != NULL && (isWhiteExt(label[-1]) || label[-1] == '\0') && (isWhiteExt(label[strlen(node->name)]) || label[strlen(node->name)] == '\0')) {
			return 1;
		}
		node = node->next;
	}
	return 0;
}

label_node* find_label(char* line, label_table* labels) {
	label_node *node = labels->head;
	while(isWhite(*line)) { line++; }
	while(node != NULL) {
		if(memcmp(line, node->name, strlen(node->name)) == 0) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}

int hasEnt(label_table* table) {
	label_node *node = table->head;
	if(node == NULL) {	
		return 0;
	}
	while(node != NULL) {
		if(strcmp(node->type, "ent") == 0) {
			return 1;
		}
		node = node->next;
	}
	return 0;
}

int hasExt(label_table* table) {
	label_node *node = table->head;
	if(node == NULL) {	
		return 0;
	}
	while(node != NULL) {
		if(strcmp(node->type, "ext") == 0) {
			return 1;
		}
		node = node->next;
	}
	return 0;
}

void printTable(label_table* table) {
	label_node *node;
	node = table->head;
	while(node != NULL) {
		printf("LABEL:\nname: %s\ntype: %s\naddress: %d\n", node->name, node->type, node->address);
		node = node->next;
	}
}














string_node* new_str_node(char* value, int address) {
	string_node* node = (string_node*) malloc(sizeof(string_node));
	node->address = address;
	node->value = (char*) malloc(sizeof(char)*(strlen(value)+1));
	strcpy(node->value, value);
	node->next = NULL;
	return node;
}

void addStrToEnd(string_node* node, string_list* list) {
	if(list->head == NULL) {
		list->head = node;
		list->tail = node;
		return;
	}
	list->tail->next = node;
	list->tail = node;
}

void free_string(string_node* node) {
	free(node->value);
	free(node);
}

void free_strings(string_list* list) {
	string_node *node;
	while(list->head != NULL) {
		node = list->head;
		list->head = node->next;
		free_string(node);
	}
	free(list);
}

void printStrings(string_list* list) {
	string_node *node;
	node = list->head;
	while(node != NULL) {
		printf("STR:\nValue: %s\nAddress: %d\n", node->value, node->address);
		node = node->next;
	}
}





data_node* new_data_node(int value, int address) {
	data_node* node = (data_node*) malloc(sizeof(data_node));
	node->value = value;
	node->address = address;
	node->next = NULL;
	return node;
}

void addDataToEnd(data_node* node, data_list* list) {
	if(list->head == NULL) {
		list->head = node;
		list->tail = node;
		return;
	}
	list->tail->next = node;
	list->tail = node;
}

void free_data(data_list* list) {
	data_node *node;
	while(list->head != NULL) {
		node = list->head;
		list->head = node->next;
		free(node);
	}
	free(list);
}

void printData(data_list* list) {
	data_node *node;
	node = list->head;
	while(node != NULL) {
		printf("DATA:\nValue: %d\nAddress: %d\n", node->value, node->address);
		node = node->next;
	}
}




firstPassRes* createResults(int IC, int DC, cmd* lines, string_list* strlist, data_list* datalist, label_table* table) {
	firstPassRes* node = (firstPassRes*) malloc(sizeof(firstPassRes));
	node->IC = IC;
	node->DC = DC;
	node->lines = lines;
	node->strlist = strlist;
	node->datalist = datalist;
	node->table = table;
	return node;
}
void freeResults(firstPassRes* node) {
	free_table(node->table);
	free_strings(node->strlist);
	free_data(node->datalist);
	free_lines(node->lines);
}
