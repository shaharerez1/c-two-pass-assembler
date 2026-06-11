
#ifndef OUTPUT_HANDLER_H
#define OUTPUT_HANDLER_H

#include "second_pass.h"

/**
 * @brief Creates output files (.obj, .ext, .ent) from the assembled commands, labels, and externals.
 *
 * @param cmds_array Array of assembled commands.
 * @param labels Linked list of labels.
 * @param externs_head Linked list of external label usages.
 * @param file_name Base filename for output files.
 * @param ICF Instruction Counter Final value.
 * @param DCF Data Counter Final value.
 */
void create_output_files(cmd_line *cmds_array, Label *labels, externs_list *externs_head, const char *file_name, const int ICF, const int DCF);

/**
 * @brief Generates filenames for the .obj, .ext, and .ent files from the input filename.
 *
 * @param input_filename Original input filename.
 * @param obj_filename Pointer to dynamically allocated string for .obj filename.
 * @param ext_filename Pointer to dynamically allocated string for .ext filename.
 * @param ent_filename Pointer to dynamically allocated string for .ent filename.
 */
void create_filenames(const char* input_filename,
                      char** obj_filename,
                      char** ext_filename,
                      char** ent_filename);

/**
 * @brief Creates the .ext file listing all external labels and their usage addresses.
 *
 * @param ext_filename Filename for the externals output file.
 * @param externs_head Linked list of external label usages.
 */
void create_ext_file(char *ext_filename, externs_list *externs_head);

/**
 * @brief Creates the .ent file listing all entry labels and their addresses.
 *
 * @param ent_filename Filename for the entries output file.
 * @param labels Linked list of labels.
 */
void create_ent_file(char *ent_filename, Label *labels);

/**
 * @brief Creates the .obj file with encoded commands and data.
 *
 * @param obj_filename Filename for the object output file.
 * @param cmds_array Array of assembled commands.
 * @param ICF Instruction Counter Final value.
 * @param DCF Data Counter Final value.
 */
void create_obj_file(char *obj_filename, cmd_line *cmds_array, int ICF, int DCF);

/**
 * @brief Converts an integer to a custom base-4 representation using characters 'a' to 'd' with fixed width.
 *
 * @param num Number to convert.
 * @param width Width of the resulting string.
 * @return Dynamically allocated base-4 string representation.
 */
char* convertToBase4Custom(int num, int width);

/**
 * @brief Converts an integer to a custom base-4 representation without fixed width or padding.
 *
 * @param num Number to convert.
 * @return Dynamically allocated base-4 string representation.
 */
char* convertToBase4Custom_NoPadding(int num);

/**
 * @brief Writes the data section arguments of a command to the output file.
 *
 * @param curr_cmd Command containing data arguments.
 * @param write_file File stream to write to.
 */
void write_inst_arguments(cmd_line *curr_cmd, FILE *write_file);

/**
 * @brief Writes the source argument of a command operation to the output file.
 *
 * @param cmd Command containing the source argument.
 * @param file File stream to write to.
 * @param index_in_line Pointer to the current line index, updated as lines are written.
 */
void write_source_argument(cmd_line *cmd, FILE *file, int *index_in_line);

/**
 * @brief Writes the destination argument of a command operation to the output file.
 *
 * @param cmd Command containing the destination argument.
 * @param file File stream to write to.
 * @param index_in_line Pointer to the current line index, updated as lines are written.
 */
void write_dest_argument(cmd_line *cmd, FILE *file, int *index_in_line);

/**
 * @brief Writes the operation arguments of a command to the output file.
 *
 * @param cmd Command to write arguments for.
 * @param file File stream to write to.
 */
void write_op_arguments(cmd_line *cmd, FILE *file);

/**
 * @brief Writes a single operation argument to the output file.
 *
 * @param cmd Command containing the argument.
 * @param arg Operation argument to write.
 * @param file File stream to write to.
 * @param index_in_line Pointer to the current line index, updated as lines are written.
 */
void write_argument(cmd_line *cmd, op_arg arg, FILE *file, int *index_in_line);

/**
 * @brief Writes the address field for an encoded line to the output file.
 *
 * @param file File stream to write to.
 * @param addr Base address of the line.
 * @param index Offset index of the line from the base address.
 */
void write_address(FILE *file, int addr, int index);

/**
 * @brief Writes a value field for an encoded line to the output file.
 *
 * @param file File stream to write to.
 * @param value Value to write.
 * @param width Width of the encoded value field.
 */
void write_value(FILE *file, int value, int width);

/**
 * @brief Writes the ARE (Absolute/Relocatable/External) bits field to the output file.
 *
 * @param file File stream to write to.
 * @param are ARE value to write.
 */
void write_ARE(FILE *file, int are);

/**
 * @brief Writes two combined register fields and a trailing 'a' character to the output file.
 *
 * @param file File stream to write to.
 * @param left_reg Left register value.
 * @param right_reg Right register value.
 */
void write_registers(FILE *file, int left_reg, int right_reg);

#endif
