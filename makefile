assembler: assembler.o structs.o syntax.o pre_assembler.o first_pass.o second_pass.o structs.h syntax.h pre_assembler.h conserved_list.h first_pass.h second_pass.h
	gcc -g -pedantic -Wall -ansi assembler.o syntax.o structs.o pre_assembler.o first_pass.o second_pass.o -o assembler
assembler.o: assembler.c
	gcc -c -pedantic -Wall -ansi assembler.c -o assembler.o
pre_assembler.o: pre_assembler.c syntax.h
	gcc -c -pedantic -Wall -ansi pre_assembler.c -o pre_assembler.o
syntax.o: syntax.c syntax.h structs.h
	gcc -c -pedantic -Wall -ansi syntax.c -o syntax.o
structs.o: structs.c structs.h
	gcc -c -pedantic -Wall -ansi structs.c -o structs.o
first_pass.o: first_pass.c first_pass.h
	gcc -c -pedantic -Wall -ansi first_pass.c -o first_pass.o
second_pass.o: second_pass.c second_pass.h
	gcc -c -pedantic -Wall -ansi second_pass.c -o second_pass.o
