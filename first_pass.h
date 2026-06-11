
#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include <stdio.h>
#include "pre_assembler.h"
#include "command_line_structure.h"
#include "label.h"
#include "error_handler.h"
#include "defined_strings.h"
#include "string_utils.h"
#include "globals.h"
#include "second_pass.h"

/**
 * @brief Performs the first pass on the assembly source file.
 * Reads the file, checks syntax, encodes commands and data,
 * processes labels and macros, and prepares for the second pass.
 *
 * @param file_name Input file name without extension.
 * @param error_status Pointer to an integer that stores if errors occurred (TRUE/FALSE).
 * @param macro_list Pointer to the list of macros used for expansion.
 */
void first_pass(char *file_name, int* error_status, Macro **macro_list);

/**
 * @brief Encodes lines from the input file during the first pass.
 * Reads lines, detects labels and instructions/operations, encodes them,
 * manages label storage, and updates instruction/data counters.
 *
 * @param read_file Open file pointer of the assembly source.
 * @param file_name Name of the file, used for error messages.
 * @param error_status Pointer to error status flag.
 * @param cmds_array Array to store encoded commands.
 * @param labels Pointer to label list for storing new labels.
 * @param macro_list Pointer to macro list for macro expansion.
 * @param ICF Pointer to instruction counter final value.
 * @param DCF Pointer to data counter final value.
 */
void encode_lines_first(FILE *read_file, char *file_name, int *error_status,
                  cmd_line *cmds_array, Label **labels, Macro **macro_list, int *ICF, int *DCF);

/**
 * @brief Encodes a single operation (code) line into the command array.
 * Sets opcode and addressing modes, encodes arguments based on opcode,
 * and updates the instruction counter accordingly.
 *
 * @param array Command array to insert into.
 * @param index Index in the array to encode this operation.
 * @param opcode Opcode of the operation.
 * @param args_buffer Arguments string for the operation.
 * @param IC Pointer to the current instruction counter (updated inside).
 * @param file_name File name for error reporting.
 * @param line_number Line number for error reporting.
 * @param error_status Pointer to error status flag.
 */
void encode_op_line(cmd_line *array, int index, int opcode, char *args_buffer, int* IC, char *file_name, int line_number, int *error_status);

/**
 * @brief Encodes an instruction (data, string, matrix) line into the commands array.
 * Processes instruction type and calls encoding functions for data, string, or matrix.
 * Updates the data counter accordingly.
 *
 * @param array Command array.
 * @param index Index at which to encode.
 * @param instcode Instruction code.
 * @param args_buffer Arguments string.
 * @param DC Pointer to the data counter (updated inside).
 */
void encode_inst_line(cmd_line *array, int index, int instcode, char *args_buffer, int* DC);

/**
 * @brief Encodes the arguments of an operation line during the first pass.
 * Determines addressing types for source and destination,
 * validates them, inserts operand data, and calculates required machine lines.
 *
 * @param cmd Pointer to the command struct to update.
 * @param args_buffer String containing the operands.
 * @param num_args Number of expected arguments (1 or 2).
 * @param file_name File name for error reporting.
 * @param line_number Line number for error reporting.
 * @param error_status Pointer to error status flag.
 */
void encode_op_arguments(cmd_line *cmd, char *args_buffer, int num_args, char *file_name,int line_number, int *error_status);

/**
 * @brief Encodes a string argument into the instruction's data array.
 * Extracts characters inside quotes, converts to ASCII values,
 * stores them, appends a null terminator, and counts required lines.
 *
 * @param cmd Pointer to command structure to update.
 * @param args_buffer String containing the quoted string argument.
 */
void encode_string_args(cmd_line *cmd, char *args_buffer);

/**
 * @brief Encodes data arguments into the instruction's data array.
 * Parses comma-separated integer values and stores them sequentially.
 * Updates the number of required lines.
 *
 * @param cmd Pointer to command structure.
 * @param args_buffer String containing comma-separated numeric data.
 */
void encode_data_args(cmd_line *cmd, char *args_buffer);

/**
 * @brief Encodes matrix arguments into the instruction's data array.
 * Parses matrix dimensions and elements, stores values row-wise,
 * fills missing elements with zeros, and updates required lines.
 *
 * @param cmd Pointer to command structure.
 * @param args_buffer String containing matrix definition and values.
 */
void encode_mat_args(cmd_line *cmd, char *args_buffer);

/**
 * @brief Inserts the destination operand's data into the command during first pass.
 * Handles immediate, register, matrix addressing,
 * and updates the command structure with operand details.
 *
 * @param cmd Pointer to command structure.
 * @param arg Operand string representing the destination.
 */
void insert_dest_data_first_pass(cmd_line *cmd, char *arg);

/**
 * @brief Inserts the source operand's data into the command during first pass.
 * Handles immediate, register, matrix addressing,
 * and updates the command structure with operand details.
 *
 * @param cmd Pointer to command structure.
 * @param arg Operand string representing the source.
 */
void insert_src_data_first_pass(cmd_line *cmd, char *arg);

/**
 * @brief Determines the addressing type of a given operand string.
 * Recognizes immediate, direct, matrix, or register addressing.
 * Updates required machine lines count accordingly.
 *
 * @param operand Operand string.
 * @param file_name File name for error reporting.
 * @param line_number Line number for error reporting.
 * @param error_status Pointer to error flag.
 * @param req_lines Pointer to required lines count to update.
 * @return The addressing type as an addr_type enum.
 */
addr_type get_addressing_type(const char *operand, char *file_name, int line_number, int *error_status, int* req_lines);

/**
 * @brief Performs syntax checks on the source file before encoding.
 * Checks for comma errors, invalid labels, and validates instructions and operations.
 * Reports errors and sets the error status flag.
 *
 * @param read_file Open file pointer of the source file.
 * @param file_name File name for error reporting.
 * @param error_status Pointer to error status flag.
 */
void syntax_check(FILE* read_file, char* file_name, int* error_status);

/**
 * @brief Validates the arguments of an operation (opcode).
 * Checks for correct number of arguments and comma usage.
 * Sets error flag and reports if validation fails.
 *
 * @param action_num Operation index.
 * @param temp_buffer Arguments string.
 * @param error_status Pointer to error flag.
 * @param file_name File name for error reporting.
 * @param line_number Line number for error reporting.
 */
void validate_operation_args(int action_num, char *temp_buffer,int *error_status, char *file_name, int line_number);

/**
 * @brief Validates the arguments of an instruction (data, string, matrix, entry, extern).
 * Calls appropriate syntax validators based on instruction type.
 * Sets error flag if validation fails.
 *
 * @param action_num Instruction index.
 * @param temp_buffer Arguments string.
 * @param error_status Pointer to error flag.
 * @param file_name File name for error reporting.
 * @param line_number Line number for error reporting.
 */
void validate_instruction_args(int action_num, char *temp_buffer,int *error_status, char *file_name, int line_number);

/**
 * @brief Updates the addresses of machine lines after first pass.
 * Adjusts code and data line addresses by adding starting indexes,
 * so data lines follow code lines properly in memory.
 *
 * @param array Command array.
 * @param ICF Final instruction counter value.
 */
void update_machine_lines_addr(cmd_line *array, int ICF);

#endif
