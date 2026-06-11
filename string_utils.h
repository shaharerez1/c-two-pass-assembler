#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#define UNFOUND -1

/**
 * @brief Duplicate a string by allocating memory and copying its content.
 *
 * @param str Pointer to the null-terminated string to duplicate.
 * @return Pointer to the newly allocated copy of the string, or NULL on failure.
 */
char* my_strdup(const char* str);

/**
 * @brief Check if a string is empty or contains only whitespace characters.
 *
 * @param str Pointer to the string to check.
 * @return Non-zero (TRUE) if string is empty or whitespace-only, zero (FALSE) otherwise.
 */
int is_empty_str(char* str);

/**
 * @brief Remove leading and trailing whitespace from a string in place.
 *
 * @param str Pointer to the string to trim.
 */
void trim_spaces(char *str);

/**
 * @brief Remove all whitespace characters from a string in place.
 *
 * @param str Pointer to the string to modify.
 */
void remove_spaces(char *str);

/**
 * @brief Extract the first word from a string, allocating a new string for it.
 *
 * @param line Pointer to the input string.
 * @return Pointer to a newly allocated string containing the first word, or NULL if none found.
 */
char* get_first_word(const char* line);

/**
 * @brief Shorten a string by removing the first `index` characters, modifying in place.
 *
 * @param str Pointer to the string to shorten.
 * @param index Number of characters to remove from the start.
 */
void shorten_string_from_start(char *str, int index);

/**
 * @brief Check if a string has a leading or trailing comma after trimming spaces.
 *
 * @param str Pointer to the string to check.
 * @param file_name Source file name for error reporting.
 * @param line_number Source line number for error reporting.
 * @return Non-zero (TRUE) if leading or trailing comma exists, zero (FALSE) otherwise.
 */
int has_leading_or_trailing_comma(const char *str, char *file_name, int line_number);

/**
 * @brief Detect missing commas between arguments in a string.
 *
 * @param str Pointer to the string to check.
 * @param file_name Source file name for error reporting.
 * @param line_number Source line number for error reporting.
 * @return Non-zero (TRUE) if missing comma detected, zero (FALSE) otherwise.
 */
int has_missing_comma(const char *str, char *file_name, int line_number);

/**
 * @brief Detect consecutive commas in a string (possibly separated by whitespace).
 *
 * @param str Pointer to the string to check.
 * @param file_name Source file name for error reporting.
 * @param line_number Source line number for error reporting.
 * @return Non-zero (TRUE) if consecutive commas detected, zero (FALSE) otherwise.
 */
int has_double_comma(const char *str,char *file_name, int line_number);

/**
 * @brief Count the number of comma-separated arguments in a string.
 *
 * @param operand_str Pointer to the argument string.
 * @return Number of arguments counted.
 */
int count_args(const char *operand_str);

/**
 * @brief Validate the syntax of a matrix definition line including bracketed sizes and arguments.
 *
 * @param line Input line containing the matrix definition.
 * @param file_name Source file name for error reporting.
 * @param line_number Source line number for error reporting.
 * @param error_status Pointer to an int to set to TRUE if an error is detected.
 */
void validate_mat_syntax(const char* line, char* file_name, int line_number, int *error_status);

/**
 * @brief Validate the syntax of a data argument list consisting of numbers separated by commas.
 *
 * @param args Input string of data arguments.
 * @param file_name Source file name for error reporting.
 * @param line_number Source line number for error reporting.
 * @param error_status Pointer to an int to set to TRUE if an error is detected.
 */
void validate_data_syntax(const char *args, char *file_name, int line_number, int *error_status);

/**
 * @brief Validate the syntax of a string argument that should be enclosed in quotes.
 *
 * @param arg Input string argument.
 * @param file_name Source file name for error reporting.
 * @param line_number Source line number for error reporting.
 * @param error_status Pointer to an int to set to TRUE if an error is detected.
 */
void validate_string_syntax(const char *arg, char *file_name, int line_number, int *error_status);

/**
 * @brief Validate the syntax of entry or extern argument labels.
 *
 * @param args Input label string.
 * @param file_name Source file name for error reporting.
 * @param line_number Source line number for error reporting.
 * @param error_status Pointer to an int to set to TRUE if an error is detected.
 */
void validate_ent_ext_syntax(const char *args, char *file_name, int line_number, int *error_status);

/**
 * @brief Concatenate a file name and an extension into a new dynamically allocated string.
 *
 * @param name Base file name.
 * @param extension Extension string to append.
 * @return Pointer to newly allocated concatenated string, or NULL on failure.
 */
char *combine_extension(const char *name, const char *extension);

/**
 * @brief If line starts with a valid label (ending with ':'), returns pointer to text after label.
 * Otherwise returns original line.
 * Prints error and returns NULL if label syntax invalid.
 *
 * @param line Input line potentially containing a label.
 * @param file_name Source file name for error reporting.
 * @param line_number Source line number for error reporting.
 * @return Pointer to position after label or original line, or NULL on error.
 */
char* trim_label(char* line, char* file_name, int line_number);

/**
 * @brief Check if a given word matches any defined operation name.
 *
 * @param word Word to check.
 * @return Index of the operation if found, UNFOUND (-1) otherwise.
 */
int is_operation(const char *word);

/**
 * @brief Check if a given word matches any defined instruction name.
 *
 * @param word Word to check.
 * @return Index of the instruction if found, UNFOUND (-1) otherwise.
 */
int is_instruction(const char *word);

/**
 * @brief Check if a string matches a valid register name (r0 to r7).
 *
 * @param str String to check.
 * @return Non-zero (TRUE) if valid register, zero (FALSE) otherwise.
 */
int is_valid_register(const char *str);

/**
 * @brief Check if a string represents a valid signed or unsigned integer number.
 *
 * @param str String to check.
 * @return Non-zero (TRUE) if valid number, zero (FALSE) otherwise.
 */
int is_valid_number(const char *str);

/**
 * @brief Validate if a string represents a matrix reference of the form NAME[reg][reg].
 *
 * @param str Input string to validate.
 * @param file_name Source file name for error reporting.
 * @param line_number Source line number for error reporting.
 * @param error_status Pointer to int to set to TRUE if error detected.
 * @return Non-zero (TRUE) if valid matrix reference, zero (FALSE) otherwise.
 */
int is_valid_matrix(const char *str, char *file_name, int line_number, int *error_status);

/**
 * @brief Checks if a string represents a number outside the range
 *        [-MAX_VAL_DATA_INST, MAX_VAL_DATA_INST].
 *
 * @param str The string representing the number to check.
 * @param numBits Number of available bits to store the number.
 * @return TRUE if the number is out of range, FALSE otherwise.
 */
int is_out_of_range(const char *str, int numBits);

#endif
