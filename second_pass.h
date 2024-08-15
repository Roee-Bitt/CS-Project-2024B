#ifndef SECOND_PASS_H
#define SECOND_PASS_H

int second_pass(char* fileName, firstPassRes* results);
int createEntFile(char* fileName, label_table *table);
int createExtFile(char* fileName, label_table *table);
int createObFile(char* fileName, string_list* strlist, data_list* datalist, int IC, int DC, short *machine);

#endif
