
#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#define MAX_LEN_EXCEEDED "Maximum length of line exceeded, over 80 characters\n"

#define EXTRANEOUS_MACRO_DEF "Extraneous text after macro definition lines\n"

#define FORBIDDEN_MACRO_NAME "Macro name can not be operation/instruction name\n"

#define DUPLICATE_MACRO "Macro name has already been defined \n"

#define SAVED_MCRO_NAME "Label name can not store a saved macro name\n"

#define FORBIDDEN_LABEL_NAME "Label name can not be operation/instruction/register name\n"

#define LABEL_DUPLICATE "Label name has already been defined\n"

#define FORBIDDEN_CHARS_LBL "Illegal characters were detected in label definition\n"

#define LABEL_LEN_EXCEEDED "Maximum length of label exceeded, over 30 characters\n"

#define EMPTY_LABEL "Empty label definition detected\n"

#define EMPTY_LINE_AFTER_LBL "Empty line after label definition detected\n"

#define MULTIPLE_COMMAS "Multiple consecutive commas detected\n"

#define MISSING_COMMA "Missing comma, arguments must be comma-seperated\n"

#define WRONG_COMMA "Unexpected comma placement within command\n"

#define CONSECUTIVE_COMMAS "Consecutive commas detected\n"

#define NO_ARGS_DETECTED "No arguments detected\n"

#define MANY_MAT_ARGS "Received matrix arguments are more than expected\n"

#define NON_NUMERIC_VAL "One or more argument(s) provided are non numeric\n"

#define VALUE_OUT_OF_RANGE "Numeric value out of range (10 bits signed)\n"

#define MISSING_CLOSING_BRACKET "Missing closing bracket in matrix definition\n"

#define INVALID_BRACKET_FORMAT "Invalid bracket syntax in matrix definition (values/bracket structure)\n"

#define FEW_ARGS "command requires more arguments than provided\n"

#define INVALID_ADDR_TYPE_SOURCE "Operation doesn't support source argument type\n"

#define INVALID_ADDR_TYPE_DEST "Operation doesn't support destination argument type\n"

#define MANY_ARGS "command requires less arguments than provided\n"

#define WRONG_MAT_REGS "Matrix indexes are invalid - supporting only register's names 0-7\n"

#define STRING_MISSING_QUOTES "String definiton missing valid quotes structure\n"

#define EXCESSIVE_TEXT "Excessive text after end of command \n"

#define INVALID_LABEL_SYNTAX "Invalid label definition syntax\n"

#define UNRESOLVED_LABEL "Unresolved label name used in command line\n"

#define INVALID_IMDT "Invalid immediate number detected, either non numeric or out of 10 bit range\n"

#define UNDEFINED_ACTION "Undefined action - name must be operation/instruction\n"

#define ENTRY_NOT_IMPLEMENTED ".entry instruction is not implemented in the source file\n"

#define BOTH_ENTRY_EXTERN_LBL "Entry label name could not use existing extern label's name\n"

/**
 * @brief Function to report an error to stdout, with file name and line number.
 * @param file_name current file name to report (with extension)
 * @param line_number current line number in the reported file
 * @param message error message (defined in 'error_handler.h')
 */
void announce_error(const char* file_name, const int line_number, const char* message);

#endif
