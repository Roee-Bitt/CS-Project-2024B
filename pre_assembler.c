#include "syntax.h"
#include "structs.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#define MAX_MACR_NAME 32
#define MAX_LINE 82
#define NO_MACR 0
#define MACR_DEF 1
#define MACR_DEP 2

macro_list* macros(cmd *lines, int len) {
	FILE *fp;
	int i = 0, j, err_flag = 0, len_count, name_ind, macr_err_flag = 0, macr_flag;
	char *ptr, *iterator, *content, *name, *line, *am_file;
	macro_node *node;
	macro_list *list;
	list = (macro_list*) malloc(sizeof(macro_list));
	
	if(list == NULL) { error(3,-1,lines[i].file_name); exit(EXIT_FAILURE); }
	
	for(i = 0; i < len; i++) {
		
		if((ptr = strstr(lines[i].content, "macr ")) && !strstr(lines[i].content, "endmacr")) {
			iterator = lines[i].content;
			while(iterator < ptr) {
				if(!isWhite(*iterator)) {
					break;
				}
				iterator++;
			}
			
			if(iterator != ptr) {
				error(7,i,lines[i].file_name); /* Extra characters on line */
				err_flag = 1;
				continue;
			}
			
			j = i + 1;
			len_count = 0;
			while(!(iterator = strstr(lines[j].content, "endmacr"))) {
				len_count += strlen(lines[j].content);
				j++;
			}
			
			line = (char*) malloc(sizeof(char)*MAX_LINE);
			if(line == NULL) { error(3,-1,lines[i].file_name); exit(EXIT_FAILURE); }
			
			strcpy(line, lines[j].content);
			replace(line, "endmacr", "");
			if(!isEmpty(line)) {
				error(7,j,lines[j].file_name);
				err_flag = 1;
				continue;
			}
			
			content = (char*) malloc(sizeof(char)*len_count+1);
			if(content == NULL) { error(3,-1,lines[i].file_name); exit(EXIT_FAILURE); }
			
			j = i + 1;
			if(!strstr(lines[j].content, "endmacr")) { /* FOR FIRST TIME TO PREVENT LAST CAT TO APPEAR */
				strcpy(content, lines[j].content);
				j++;
			}

			while(!strstr(lines[j].content, "endmacr")) { /* CATTING THE REST */
				strcat(content, lines[j].content);
				j++;
			}
			content[len_count] = '\0';

			name = (char*) malloc(sizeof(char)*MAX_MACR_NAME);
			if(name == NULL) { error(3,-1,lines[i].file_name); exit(EXIT_FAILURE); }
			
			ptr = ptr + 4;
			name_ind = 0;
			while(isWhite(*ptr) && *ptr != '\0') { ptr++; }
			if(!isalpha(*ptr)) {
				error(8, i, lines[i].file_name); /* MACRO NAME HAS TO START WITH LETTER */
				err_flag = 1;
				free(content);
				free(name);
				continue;
			}
			
			while(!isWhiteExt(*ptr) && *ptr != '\0' && name_ind < MAX_MACR_NAME - 1) { /* FOR INDEX REASONS */
				name[name_ind++] = *ptr;
				ptr++;
			}
			name[name_ind] = '\0';
			
			if(!isWhiteExt(*ptr) && *ptr != '\0') {
				error(2,i,lines[i].file_name); /* Macro name too long */
				err_flag = 1;
				free(content);
				free(name);
				continue;
			}
			
			strcpy(line, lines[i].content);
			replace(line, "macr", "");
			replace(line, name, "");
			if(!isEmpty(line)) {
				error(7,i,lines[i].file_name);
				err_flag = 1;
				free(content);
				free(name);
				continue;
			}
			
			node = new_node(name, content, lines[i].file_name, i);
			
			if(updateList(list, node) != 0) {
				err_flag = 1;
			}
			
			free(content);
			free(name);
			i = j;
		}
	}
	
	if(err_flag) {
		free_list(list);
		free(list);
		return NULL;
	}
	
	printf("macro success\n");
	
	am_file = (char*) malloc(sizeof(char)*(strlen(lines[0].file_name) + 4));
	if(am_file == NULL) { 
		error(3, -1, lines[0].file_name);
		free_list(list);
		exit(EXIT_FAILURE);
	}

	strcpy(am_file, lines[0].file_name);
	am_file[strchr(am_file, '.') - am_file] = '\0';
	strcat(am_file, ".am");
	
	fp = fopen(am_file, "w");
	free(am_file);
	
	if(fp == NULL) {
		error(10, -1, lines[0].file_name);
		return NULL;
	}
	
	for(i = 0; i < len; i++) {
		macr_flag = contains_macr(lines[i].content, list);
		if(macr_flag == MACR_DEF) {
			while(strstr(lines[i].content, "endmacr") == NULL) {
				i++;
			}
		}
		else if(macr_flag == MACR_DEP) {
			node = find_macr(lines[i].content, list);
			if(node == NULL) {
				error(4, -1, lines[0].file_name);
				fclose(fp);
				free_list(list);
				macr_err_flag = 1;
			}
			fprintf(fp, "%s", node->macro_cont);
		}
		else {
			fprintf(fp, "%s", lines[i].content);
		}
	}
	if(macr_err_flag) {	
		return NULL;
	}
	
	fclose(fp);
	
	printf("macro deploy success\n");
	
	return list;
}
