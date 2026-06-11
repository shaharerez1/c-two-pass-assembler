
#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "first_pass.h"

/* Linked list of external label usages (for .ext file) */
typedef struct externs_data
{
    char name[MAX_LABEL_LENGTH];
    unsigned int usage_addr;
    struct externs_data *next;
} externs_list;

/**
 * @brief Executes the second pass of the assembler. Resolves label addresses, encodes instructions fully,
 * and generates the necessary output files.
 * @param read_file Pointer to the assembly source file for reading.
 * @param am_file_name Name of the assembly source file.
 * @param error_status Pointer to an integer tracking errors.
 * @param cmds_array Array of commands to encode.
 * @param labels Linked list of labels for address resolution.
 * @param ICF Instruction Counter Final value.
 * @param DCF Data Counter Final value.
 */
void second_pass(FILE *read_file, char *am_file_name, int *error_status,
                 cmd_line *cmds_array, Label *labels, int ICF, int DCF);

/**
 * @brief Reads and processes each line during the second pass. Extracts labels, handles entry directives,
 * and encodes instructions.
 * @param read_file Pointer to the assembly source file.
 * @param file_name Name of the source file.
 * @param error_status Pointer to an error status flag.
 * @param cmds_array Array of command lines.
 * @param labels Linked list of labels.
 * @param externs_head Pointer to the list of external label usages.
 */
void encode_lines_second(FILE *read_file, char *file_name, int *error_status,
                         cmd_line *cmds_array, Label *labels, externs_list **externs_head);

/**
 * @brief Encodes operands of an instruction line. Handles one or two arguments, encoding source and destination operands.
 * @param cmd_array Array of commands.
 * @param cmd_index Index of the command to encode.
 * @param args_buffer Arguments string buffer.
 * @param labels Linked list of labels.
 * @param file_name Source file name.
 * @param line_number Current line number in source file.
 * @param error_status Pointer to error flag.
 * @param externs_head Pointer to external labels usage list.
 */
void finish_op_encoding(cmd_line *cmd_array, int cmd_index, char *args_buffer,
                        Label *labels, char *file_name, int line_number,
                        int *error_status, externs_list **externs_head);

/**
 * @brief Encodes a source operand that is a label. Checks label existence, updates external usage list if needed,
 * and encodes the argument.
 * @param cmd Command line structure to update.
 * @param arg Label operand string.
 * @param labels Linked list of labels.
 * @param externs_head Pointer to external labels usage list.
 * @param file_name Source file name.
 * @param line_number Current source line number.
 * @param error_status Pointer to error status.
 * @param index_in_line Index of the operand in multi-line instructions.
 */
void encode_label_arg_source(cmd_line *cmd, char *arg, Label *labels,
                             externs_list **externs_head, char *file_name,
                             int line_number, int *error_status, unsigned int index_in_line);

/**
 * @brief Encodes a destination operand that is a label. Checks label existence, updates external usage list if needed,
 * and encodes the argument.
 * @param cmd Command line structure to update.
 * @param arg Label operand string.
 * @param labels Linked list of labels.
 * @param externs_head Pointer to external labels usage list.
 * @param file_name Source file name.
 * @param line_number Current source line number.
 * @param error_status Pointer to error status.
 * @param index_in_line Index of the operand in multi-line instructions.
 */
void encode_label_arg_dest(cmd_line *cmd, char *arg, Label *labels,
                           externs_list **externs_head, char *file_name,
                           int line_number, int *error_status, unsigned int index_in_line);

/**
 * @brief Assigns a label as an entry label. Checks if label exists and is not external, then updates its type.
 * @param entry_label_name Name of the entry label.
 * @param labels Linked list of labels.
 * @param file_name Source file name.
 * @param line_number Current source line number.
 * @param error_status Pointer to error status.
 */
void assign_entry_label(char *entry_label_name, Label *labels,
                        char *file_name, int line_number, int *error_status);

/**
 * @brief Adds an external label usage entry to the externs list.
 * @param addr Address where the external label is used.
 * @param label Pointer to the external label.
 * @param head Pointer to the head of the external labels usage list.
 */
void add_to_externs_list(unsigned int addr, Label *label, externs_list **head);

/**
 * @brief Frees the linked list of external label usages.
 * @param head Head of the external labels usage list.
 */
void free_externs_list(externs_list *head);

#endif
