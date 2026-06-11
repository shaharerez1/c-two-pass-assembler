
#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

#include "globals.h"
#include <stdio.h>

typedef struct Macro
{
    char name[MAX_LINE_LENGTH];
    char *content; /* Dynamically allocated variable to store the entire macro text */
    struct Macro *next;
} Macro;

/**
 * @brief Entry point for the pre-assembler step.
 * Reads the source file, processes macros, and writes the expanded output file.
 * @param file_name The base name of the source file (without extension)
 * @param error_status Pointer to an int flag set TRUE if errors occur
 * @return Pointer to the head of the macro linked list, or NULL on failure
 */
Macro* pre_assembler(char* file_name, int *error_status);

/**
 * @brief Processes lines from the input file, handling macro definitions and expansions.
 * @param read_file File pointer to the input file (.as)
 * @param write_file File pointer to the output file (.am)
 * @param file_name Name of the input file for error messages
 * @param error_status Pointer to an int that will be set to TRUE on error
 * @return Pointer to the head of the linked list of macros found during processing
 */
Macro* process_lines(FILE* read_file, FILE* write_file, char* file_name, int* error_status);

/**
 * @brief Detects the start of a new macro definition.
 * @param first_word The first word of the current line
 * @param remaining_text The rest of the line after the first word
 * @param macro_name Buffer to store the macro's name if found
 * @param in_macro Pointer to int flag whether currently inside a macro definition
 * @param macro_content Pointer to macro content buffer to initialize if macro starts
 * @param macro_size Pointer to size of the macro content buffer
 * @return TRUE if a new macro definition start was detected, FALSE otherwise
 */
int check_new_macro_start(const char* first_word,char* remaining_text,char* macro_name,int* in_macro,char** macro_content,size_t* macro_size);

/**
 * @brief Detects if the current line marks the end of a macro definition.
 * @param first_word The first word of the current line
 * @param macro_name The name of the macro currently being defined
 * @param macro_content Pointer to macro content buffer, to be freed if macro ends
 * @param macro_size Pointer to size of the macro content buffer
 * @param macro_list Pointer to the linked list of macros to add the new macro to
 * @param in_macro Pointer to int flag indicating macro definition status; reset on end
 * @return TRUE if macro end detected, FALSE otherwise
 */
int reached_end_of_macro(char *first_word,char *macro_name,char **macro_content,size_t *macro_size,Macro **macro_list,int *in_macro);

/**
 * @brief Appends a line of text to the current macro's content buffer, reallocating memory as needed.
 * @param line The line to append
 * @param macro_content Pointer to macro content buffer
 * @param macro_size Pointer to size of the macro content buffer
 * @param macro_list Linked list of macros, freed on allocation failure
 * @param error_status Pointer to int error flag, set TRUE on failure
 * @return TRUE on memory allocation failure, FALSE otherwise
 */
int append_line_to_macro(char *line,char **macro_content,size_t *macro_size,Macro *macro_list,int *error_status);

/**
 * @brief Adds a new macro to the linked list of macros.
 * @param head Pointer to the head of the macro list
 * @param name Name of the macro
 * @param content Content of the macro
 * @return Pointer to the new head of the macro list (new macro added)
 */
Macro* add_macro(Macro *head, const char *name, const char *content);

/**
 * @brief Finds a macro in the linked list by name.
 * @param head Pointer to the head of the macro list
 * @param name Name of the macro to find
 * @return Pointer to the macro if found, NULL otherwise
 */
Macro* find_macro(Macro *head, const char *name);

/**
 * @brief Frees all memory allocated for the macro linked list.
 * @param head Pointer to the head of the macro list
 */
void free_macros(Macro *head);

/**
 * @brief Validates a macro's name for conflicts and checks for extraneous text after the macro name.
 * @param macro_name The macro's name
 * @param reminder_text The rest of the line after the macro name
 * @param file_name The source file name, used for error reporting
 * @param error_status Pointer to error flag to set TRUE on invalid macro name
 * @param line_number The current line number for error reporting
 * @param head Head of macros linked list, for search of duplicates
 */
void check_valid_macro_start(char* macro_name,char* reminder_text, char* file_name,int *error_status, int line_number, Macro *head);

/**
 * @brief Checks if a line exceeds the maximum allowed length.
 * @param buffer The line buffer to check
 * @param read_file File pointer to the source file
 * @param file_name The source file name, used for error reporting
 * @param line_number The current line number for error reporting
 * @param error_status Pointer to error flag, set TRUE if line is too long
 * @return TRUE if the line is too long, FALSE otherwise
 */
int line_is_too_long(const char buffer[MAX_LINE_LENGTH], FILE* read_file, const char* file_name, int line_number, int *error_status);

/**
 * @brief Writes a line or expanded macro content to the output file.
 * If the first_word matches a macro name in the list, writes the macro's content instead.
 * @param write_file File pointer to the output file
 * @param current_list Linked list of macros available for expansion
 * @param first_word The first word of the line to check for macro expansion
 */
void write_line_or_macro_to_file(FILE* write_file, Macro* current_list,char* first_word);


#endif