#ifndef LABEL_H
#define LABEL_H


#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "pre_assembler.h"

typedef enum
{
    CODE_LBL=0,
    DATA_LBL,
    ENTRY_LBL,
    EXTERN_LBL
} label_type;

typedef struct Label
{
    char name[MAX_LABEL_LENGTH];
    unsigned int addr;
    struct Label *next;
    label_type type;

} Label;

/**
 * @brief Adds a new label to the linked list.
 * @param list Pointer to the head of the label list.
 * @param name The name of the label to add.
 * @param addr The address associated with the label.
 * @param type The type of the label (code, data, extern, entry).
 * @return Returns 1 on success, 0 if memory allocation fails.
 */
int add_label(Label **list, const char *name, unsigned int addr, label_type type);

/**
 * @brief Finds a label in the list by its name.
 * @param list The head of the label list.
 * @param name The label name to search for.
 * @return Returns a pointer to the Label if found, NULL otherwise.
 */
Label *find_label(Label *list, const char *name);

/**
 * @brief Frees all memory allocated for the label linked list.
 * @param list The head of the label list to free.
 */
void free_labels(Label *list);

/**
 * @brief Parses a line to identify and extract a label definition.
 * @param line The line string to parse.
 * @param labels The current list of labels.
 * @param label_name_out Buffer to output the extracted label name.
 * @param file_name The source file name (for error reporting).
 * @param error_status Pointer to error status flag to update on error.
 * @param line_number The current line number (for error reporting).
 * @param macro_list List of macros to check for name conflicts.
 * @return Returns TRUE if a valid new label is found, FALSE otherwise.
 */
int handle_label_definition(const char *line, Label *labels, char *label_name_out,
                            char *file_name, int *error_status, int line_number, Macro **macro_list);

/**
 * @brief Checks if a label name is valid, unique among labels and macros, and not reserved.
 * @param name The label name to validate.
 * @param labels The list of existing labels.
 * @param macro_list The list of existing macros.
 * @param file_name The source file name (for error reporting).
 * @param line_number The current line number (for error reporting).
 * @param error_status Pointer to error status flag to update on error.
 * @return Returns TRUE if the label name is valid, FALSE otherwise.
 */
int is_valid_new_label_name(const char *name, Label *labels, Macro *macro_list,
                            const char *file_name, int line_number, int *error_status);

/**
 * @brief Updates addresses of code and data labels by adding start index and ICF offset.
 * Entry and extern labels are not updated by this function.
 * @param start The head of the label list.
 * @param ICF The Instruction Counter Final value for address offsetting.
 */
void update_labels_addr(Label *start, int ICF);

#endif
