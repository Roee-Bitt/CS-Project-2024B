#ifndef STRUCTS_H
#define STRUCTS_H
#include <stdio.h>

typedef struct command {
	char* file_name;
	int line_num;
	char *content;
} cmd;

typedef struct macro_node {
	int line_num;
	char* file_name;
	char* macro_name;
	char* macro_cont;
	struct macro_node *next;
} macro_node;

typedef struct macro_list {
	macro_node *head;
	macro_node *tail;
} macro_list;

typedef struct label_node {
	char* name;
	char* type;
	char* point;
	int address;
	struct label_node *next;
} label_node;

typedef struct label_table {
	label_node *head;
	label_node *tail;
} label_table;

typedef struct string_node {
	char* value;
	int address;
	struct string_node *next;
} string_node;

typedef struct string_list {
	string_node *head;
	string_node *tail;
} string_list;

typedef struct data_node {
	int value;
	int address;
	struct data_node *next;
} data_node;

typedef struct data_list {
	data_node *head;
	data_node *tail;
} data_list;

typedef struct opcode {
	char* name;
	short code;
} opcode;

typedef struct firstPassRes {
	int IC;
	int DC;
	cmd* lines;
	string_list* strlist;
	data_list* datalist;
	label_table* table;
} firstPassRes;

void printTxt(cmd *lines, int num);
macro_node* new_node(char* name, char* cont, char* file_name, int line_num);
void addToEnd(macro_node* node, macro_list* list);
int updateList(macro_list *list, macro_node *nodeptr);
void free_node(macro_node* node);
void free_list(macro_list* list);
int contains_macr(char* line, macro_list* macrs);
macro_node* find_macr(char* line, macro_list* macrs);
void free_lines(cmd* line);
int lenLines(cmd* line);

label_node* new_label_node(char* name, char* type, char* point, int address);
void addLabelToEnd(label_node* nodeptr, label_table* listptr);
int updateTable(label_table *table, label_node *nodeptr, int line, char* fileName);
void removeLabel(label_node* node, label_table* table);
void free_label(label_node* node);
void free_table(label_table* table);
int contains_label(char* line, label_table* labels);
label_node* find_label(char* line, label_table* labels);
int hasEnt(label_table* table);
int hasExt(label_table* table);

string_node* new_str_node(char* value, int address);
void addStrToEnd(string_node* node, string_list* list);
void free_string(string_node* node);
void free_strings(string_list* list);
void printStrings(string_list* list);

data_node* new_data_node(int value, int address);
void addDataToEnd(data_node* node, data_list* list);
void free_data(data_list* list);
void printData(data_list* list);

firstPassRes* createResults(int IC, int DC, cmd* lines, string_list* strlist, data_list* datalist, label_table* table);
void freeResults(firstPassRes* node);

#endif
