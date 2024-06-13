/* gcc -ansi -pedantic -Wall assembler.c -o assembler */

#include <stdio.h>
#include <string.h>

int main() {
  char name[100];
  printf("Hello There.\n");
  fgets(name, 100, stdin);
  name[strlen(name)-1] = '\0';
  printf("%s.\n",name);
  return 0;
}