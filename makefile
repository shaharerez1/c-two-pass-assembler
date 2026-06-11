CC = gcc
CFLAGS = -Wall -pedantic -ansi

OBJ = assembler.o pre_assembler.o string_utils.o defined_strings.o globals.o first_pass.o second_pass.o output_handler.o label.o error_handler.o

assembler: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o assembler

assembler.o: assembler.c pre_assembler.h first_pass.h
	$(CC) $(CFLAGS) -c assembler.c

pre_assembler.o: pre_assembler.c pre_assembler.h defined_strings.h string_utils.h error_handler.h
	$(CC) $(CFLAGS) -c pre_assembler.c

string_utils.o: string_utils.c string_utils.h error_handler.h defined_strings.h globals.h
	$(CC) $(CFLAGS) -c string_utils.c

defined_strings.o: defined_strings.c defined_strings.h
	$(CC) $(CFLAGS) -c defined_strings.c

globals.o: globals.c globals.h
	$(CC) $(CFLAGS) -c globals.c

first_pass.o: first_pass.c first_pass.h
	$(CC) $(CFLAGS) -c first_pass.c

second_pass.o: second_pass.c second_pass.h output_handler.h
	$(CC) $(CFLAGS) -c second_pass.c

output_handler.o: output_handler.c output_handler.h
	$(CC) $(CFLAGS) -c output_handler.c

label.o: label.c label.h globals.h error_handler.h string_utils.h
	$(CC) $(CFLAGS) -c label.c

error_handler.o: error_handler.c
	$(CC) $(CFLAGS) -c error_handler.c

clean:
	rm -f $(OBJ) assembler
