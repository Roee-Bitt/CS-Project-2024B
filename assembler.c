#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "structs.h"
#include "syntax.h"
#include "pre_assembler.h"
#include "conserved_list.h"
#include "first_pass.h"
#include "second_pass.h"
#define MAX_LINE 82
#define SHUTDOWN\
	free(line);\
	free_list(macrs);\
	free(am_file);\
	free_lines(lines);\
	free(lines);

int main(int argc, char* argv[]) {
	
	FILE *fp;
	int i, k, count;
	char c, *am_file, *line;
	cmd *lines;
	macro_list *macrs;
	firstPassRes* results;
	
	if(argc == 1) { /* IF PROGRAM GOT NO ARGUMENTS */
		fprintf(stderr,"ERR: No Arguments Were Passed To The Program.\n");
		return 1;
	}
	
	for(k = 1; k < argc; k++) {
		count = 0;
		strcat(*(++argv),".as");
		
		fp = fopen(*argv, "r");
		
		if(fp == NULL) {
			error(9, -1, *argv);
			continue;
		}
		
		while((c = getc(fp)) != EOF) { /* COUNT NUMBER OF LINES IN INPUT */
			if(c == '\n') {
				count++;
			}
		}

		lines = (cmd*) malloc(sizeof(cmd) * (count+1));
		if(lines == NULL) { error(3, -1, *argv); fclose(fp); exit(EXIT_FAILURE); }

		if(fseek(fp, 0, SEEK_SET) != 0) { /* RESETTING fp TO START OF FILE */
			error(6, -1, *argv);
			free(lines);
			continue;
		}
		
		i = 0;
		line = (char*) malloc(sizeof(char)*MAX_LINE);
		while(fgets(line, MAX_LINE, fp) != NULL) { /* GETTING LINES FROM FILE TO ARRAY 'lines' */
			if(strchr(line,'\n') != NULL) {
				strchr(line, '\n')[1] = '\0';
			} /* TERMINATING LINES */
			else {
				while((c = getc(fp)) != '\n' && c != '\0' && c != EOF); 
			} /* IN CASE OF A LONG LINE */
			lines[i].content = (char*) malloc(sizeof(char)*strlen(line) + 1);
			strcpy(lines[i].content, line);
			lines[i].file_name = *argv;
			lines[i].line_num = i;
			i++;
		}
		if(!feof(fp)) {	
			error(6,-1,*argv);
			fclose(fp);
			free(lines);
			continue;
		}
		
		lines[i].content = NULL;
		
		macrs = (macro_list*) macros(lines, count);
		if(macrs == NULL) {
			fclose(fp);
			free(lines);
			continue;
		}
		
		fclose(fp);
		
		am_file = (char*) malloc(sizeof(char)*(strlen(*argv) + 4));
		if(am_file == NULL) { 
			error(3, -1, *argv);
			free_list(macrs);
			exit(EXIT_FAILURE);
		}
	
		strcpy(am_file, *argv);
		am_file[strchr(am_file, '.') - am_file] = '\0';
		strcat(am_file, ".am");
		
		results = first_pass(am_file, macrs);
		second_pass(am_file, results);
		SHUTDOWN
	}
	return 0;
} 
