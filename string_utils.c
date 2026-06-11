#include "string_utils.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "error_handler.h"
#include "defined_strings.h"
#include "globals.h"

/**
 * Creates a new dynamically allocated copy of the input string.
 * Returns a pointer to the duplicated string, or NULL if memory allocation fails.
 */
char* my_strdup(const char* str)
{ /* strdup() is not valid with C90 */
    size_t len = strlen(str) + 1;
    char* copy = malloc(len);
    if (copy)
        memcpy(copy, str, len);
    return copy;
}

/**
 * Checks if the given string is empty or contains only whitespace characters.
 * Returns TRUE if empty or whitespace-only, FALSE otherwise.
 */
int is_empty_str(char* str)
{
    int i = 0;
    size_t length = strlen(str);

    for (;i < length; i++)
    {
        if (!isspace(str[i]))
            return FALSE;
    }
    return TRUE;
}

/**
 * Concatenates a file name with a given extension.
 * Returns a newly allocated string with the combined result, or NULL on allocation failure.
 */
char *combine_extension(const char *name, const char *extension)
{
    size_t length = strlen(name) + strlen(extension) + 1; /*With null terminator*/
    char *result = malloc(length);
    if (result == NULL) {
        return NULL;
    }
    strcpy(result, name);
    strcat(result, extension);
    return result;
}

/**
 * Detects missing commas between arguments by checking for spaces without commas.
 * Returns TRUE if a missing comma is detected, FALSE otherwise.
 * Reports errors with file name and line number.
 */
int has_missing_comma(const char *str, char *file_name, int line_number)
{
    int in_space = 0;
    int found_com = 0;
    int found_word = 0;

    while (*str)
    {
        if (isspace(*str))
        {
            if (found_word) /* only care about spaces after a word was found */
                in_space = 1;
        }
        else /* char isn't a space */
        {
            if (*str == ',') /* we already checked if there are multiple commas so it's safe to turn on once*/
            {
                found_com = 1;
                in_space = 0;
            }
            else
            {
                if (!found_com && in_space) /* reached end of gap, and no comma was found */
                {
                    announce_error(file_name, line_number, MISSING_COMMA);
                    return TRUE;
                }
                found_word = 1; /* first word detected */
                in_space = 0; /* continue checking */
                found_com = 0;
            }
        }
        str++;
    }
    return FALSE;
}

/**
 * Checks if the input string has a leading or trailing comma after trimming spaces.
 * Returns TRUE if a leading or trailing comma is found, FALSE otherwise.
 * Reports errors with file name and line number.
 */
int has_leading_or_trailing_comma(const char *str, char *file_name, int line_number)
{
    const char *start = str;
    const char *end = str + strlen(str) - 1;

    while (isspace(*start)) start++;
    while (end > start && isspace(*end)) end--;

    if (*start == ',' || *end == ',') {
        announce_error(file_name,line_number,WRONG_COMMA);
        return TRUE;
    }
    return FALSE;
}

/**
 * Detects if the input string contains consecutive commas possibly separated by whitespace.
 * Returns TRUE if consecutive commas are found, FALSE otherwise.
 * Reports errors with file name and line number.
 */
int has_double_comma(const char *str,char *file_name, int line_number)
{
    int i, j;
    for (i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == ',')
        {
            j = i + 1;

            while (isspace(str[j]))
                j++;

            if (str[j] == ',')
            {
                announce_error(file_name,line_number,CONSECUTIVE_COMMAS);
                return TRUE;
            }
        }
    }
    return FALSE;
}

/**
 * Counts the number of arguments in a comma-separated string.
 * Returns the count of arguments found.
 */
int count_args(const char* line) {
    int count = 0;
    char *copy, *token, *end;

    copy = my_strdup(line);

    token = strtok(copy, ",");
    while (token != NULL) {
        /* Skip spaces at start and end of token */
        while (isspace(*token)) token++;
        end = token + strlen(token) - 1;
        while (end > token && isspace(*end)) *end-- = '\0';

        count++;
        token = strtok(NULL, ",");
    }

    free(copy);
    return count;
}

/**
 * Extracts and returns a newly allocated string containing the first word (non-whitespace sequence) in the input line.
 * Returns NULL if no word is found or memory allocation fails.
 */
char* get_first_word(const char* line)
{
    size_t len;
    char* word;

    while (*line && isspace((unsigned char)*line))
        line++;

    if (*line == '\0')
        return NULL;

    len = 0;
    while (line[len] && !isspace((unsigned char)line[len]))
        len++;

    word = (char*) malloc(len + 1);
    if (!word)
    {
        perror("malloc failed");
        return NULL;
    }

    strncpy(word, line, len);
    word[len] = '\0';

    return word;
}

/**
 * Removes leading and trailing whitespace from the input string in place.
 */

void trim_spaces(char *str)
{
    char *start = str;
    char *end;

    while (isspace(*start)) /* get to first actual char */
        start++;

    end = start;
    while (*end != '\0')
        end++;

    end--;

    while (end > start && isspace(*end))
        end--;

    *(end + 1) = '\0'; /* null terminate after the last actual character */

    if (start != str)
    {
        while (*start)
            *str++ = *start++; /* offset the beginning of the string */
        *str = '\0';
    }
}

/**
 * Removes all whitespace characters from the input string in place.
 */
void remove_spaces(char *str)
{
    char *src = str;
    char *dst = str;

    while (*src != '\0')
    {
        if (!isspace(*src))
        {
            *dst = *src;
            dst++;
        }
        src++;
    }
    *dst = '\0';
}

/**
 * Shortens the input string by removing characters from the start up to the given index.
 * Modifies the string in place.
 */
void shorten_string_from_start(char *str, int index)
{
    size_t len = strlen(str);
    if (index >= len)
        str[0] = '\0';

    else
        memmove(str, str + index, len - index + 1); /* "overwrite" the memory block of str with the shortened string */
    /* including the /0 char */
}


/**
 * Checks if a line starts with a valid label followed by ':'.
 * Returns a pointer to the text after ':' if valid label is found.
 * Returns original line if no label exists.
 * Prints an error and returns NULL if label syntax is invalid.
 */
char* trim_label(char* line, char* file_name, int line_number)
{
    int i = 0;

    /* Find the first ':' */
    while (line[i] && line[i] != ':')
        i++;

    /* No ':' found — not a label */
    if (line[i] == '\0')
        return line;

    /* Check for space before ':' */
    if (i > 0 && isspace(line[i - 1])) {
        announce_error(file_name, line_number, INVALID_LABEL_SYNTAX);
        return NULL;
    }

    /* Move past ':' */
    i++;

    /* Skip spaces after ':' */
    while (isspace(line[i]))
        i++;

    if (is_empty_str(line+i))
    {
        announce_error(file_name, line_number, EMPTY_LINE_AFTER_LBL);
        return NULL;
    }
    return line + i;
}

/**
 * Checks if a given word matches any known instruction name.
 * Returns the index of the instruction if found, or -1 (UNFOUND) if not.
 */
int is_instruction(const char *word)
{
    int i;
    for (i = 0; i < num_instructions; i++) {
        if (strcmp(word, instruction_names[i]) == 0)
            return i;
    }
    return UNFOUND;
}

/**
 * Checks if a given word matches any known operation name.
 * Returns the index of the operation if found, or -1 (UNFOUND) if not.
 */
int is_operation(const char *word)
{
    int i;
    for (i = 0; i < num_operations; i++) {
        if (strcmp(word, operations[i].name) == 0)
            return i;
    }
    return UNFOUND;
}

/**
 * Validates the syntax of a matrix declaration line with row and column brackets and arguments.
 * Sets error_status to TRUE if syntax errors are found, otherwise FALSE.
 * Reports relevant syntax errors with file name and line number.
 */
void validate_mat_syntax(const char* line, char* file_name, int line_number, int *error_status)
{
    char* copy = my_strdup(line);
    char* ptr;
    int rows, cols, count = 0; /* Count (of arguments) should be equal to rows*cols by the end */
    char* token;

    ptr = strchr(copy, '[');
    if (!ptr || sscanf(ptr, "[%d][%d]", &rows, &cols) != 2 || rows <= 0 || cols <= 0) {
        free(copy);
        announce_error(file_name,line_number,INVALID_BRACKET_FORMAT);
        *error_status = TRUE;
        return;
    }

    ptr = strchr(ptr, ']');
    ptr = strchr(ptr + 1, ']');
    if (!ptr) {
        free(copy);
        announce_error(file_name,line_number,MISSING_CLOSING_BRACKET);
        *error_status = TRUE;
        return;
    }

    ptr++; /* after brackets */
    while (isspace(*ptr)) ptr++;
    if (*ptr == '\0') {
        free(copy);
        return;
    }

    /* Check only the arguments part for syntax errors regarding commas */
    if (has_leading_or_trailing_comma(ptr, file_name, line_number) || has_missing_comma(ptr, file_name, line_number))
    {
        free(copy);
        *error_status = TRUE;
        return;
    }

    /* Loop for checking each number's digits one by one */
    token = strtok(ptr, ",");
    while (token)
    {

        if (!is_valid_number(token))
        {
            announce_error(file_name,line_number,NON_NUMERIC_VAL);
            *error_status = TRUE;
            free(copy);
            return;
        }

        if (is_out_of_range(token,_10BIT)) /* Number is numeric - now check actual value for 10 bit range) */
        {
            announce_error(file_name, line_number, VALUE_OUT_OF_RANGE);
            *error_status = TRUE;
            free(copy);
            return;
        }

        count++; /* Valid argument, increase argument count */
        token = strtok(NULL, ",");
    }

    if (count > rows * cols) {
        announce_error(file_name,line_number,MANY_MAT_ARGS);
        *error_status = TRUE;
    }

    free(copy);
}

/**
 * Validates the syntax of a data argument list consisting of comma-separated numbers.
 * Sets error_status to TRUE if syntax errors or invalid values are found.
 */
void validate_data_syntax(const char *args, char *file_name, int line_number, int *error_status)
{
    char *copy, *token;
    int found_args = 0;

    if (has_leading_or_trailing_comma(args, file_name, line_number) ||
        has_missing_comma(args, file_name, line_number)) /* Commas check */
    {
        *error_status = TRUE;
        return;
    }

    copy = my_strdup(args);
    token = strtok(copy, ",");

    while (token)
    {
        while (isspace(*token))
            token++;

        if (!is_valid_number(token))
        {
            announce_error(file_name,line_number,NON_NUMERIC_VAL);
            *error_status = TRUE;
            free(copy);
            return;
        }

        if (is_out_of_range(token,_10BIT)) /* Number is numeric - now check actual value for 10 bit range) */
        {
            announce_error(file_name, line_number, VALUE_OUT_OF_RANGE);
            *error_status = TRUE;
            free(copy);
            return;
        }

        found_args++;
        token = strtok(NULL, ",");
    }

    if (found_args == 0)
    {
        announce_error(file_name, line_number, NO_ARGS_DETECTED);
        *error_status = TRUE;
    }

    free(copy);
}


/**
 * Validates that a string argument starts and ends with quotes and has no extra text.
 * Sets error_status to TRUE if quotes are missing or syntax is incorrect.
 */
void validate_string_syntax(const char *arg, char *file_name, int line_number, int *error_status)
{
    const char *start = arg;
    const char *end;

    if (has_leading_or_trailing_comma(arg,file_name, line_number)) {
        *error_status = TRUE;
        return;
    }

    while (isspace(*start)) start++;

    if (*start != '"') { /* missing starting quote */
        announce_error(file_name, line_number, STRING_MISSING_QUOTES);
        *error_status = TRUE;
        return;
    }

    end = strchr(start + 1, '"');
    if (!end) { /* missing ending quote */
        announce_error(file_name, line_number, STRING_MISSING_QUOTES);
        *error_status = TRUE;
        return;
    }

    end++;
    while (isspace(*end)) end++;

    if (*end != '\0') { /* extra text after closing quote */
        announce_error(file_name, line_number, EXCESSIVE_TEXT);
        *error_status = TRUE;
    }
}

/**
 * Validates the syntax of entry/extern argument labels.
 * Sets error_status to TRUE if the label is invalid, forbidden, or syntax errors exist.
 */
void validate_ent_ext_syntax(const char *args, char *file_name, int line_number, int *error_status)
{
    char label[MAX_LINE_LENGTH];
    const char *p = args;

    if (has_leading_or_trailing_comma(args, file_name, line_number)) {
        *error_status = TRUE;
        return;
    }

    while (isspace(*p)) p++;

    /* scan 30 chars as instructed */
    if (sscanf(p, "%31s", label) != 1) {
        announce_error(file_name, line_number, NO_ARGS_DETECTED);
        *error_status = TRUE;
        return;
    }

    p += strlen(label);
    while (isspace(*p)) p++;

    if (*p != '\0') /* check for remaining text after name of label */
    {
        announce_error(file_name, line_number, EXCESSIVE_TEXT);
        *error_status = TRUE;
        return;
    }

    /* check for safe words */
    if (is_instruction(label) != UNFOUND || is_operation(label) != UNFOUND) {
        announce_error(file_name, line_number, FORBIDDEN_LABEL_NAME);
        *error_status = TRUE;
    }
}

/**
 * Checks if the input string matches a valid register name (r0 to r7).
 * Returns TRUE if valid register, FALSE otherwise.
 */
int is_valid_register(const char *str)
{
    return strlen(str) == 2 && str[0] == 'r' && str[1] >= '0' && str[1] <= '7';
}

/**
 * Checks if the input string represents a valid signed or unsigned integer number.
 * Returns TRUE if valid number, FALSE otherwise.
 */
int is_valid_number(const char *str)
{
    if (str[0] == '-' || str[0] == '+')
        str++;
    if (!*str)
        return FALSE;
    while (*str)
    {
        if (!isdigit((unsigned char)*str))
            return FALSE;
        str++;
    }
    return TRUE;
}

/**
 * Validates if the input string represents a matrix reference in the format NAME[reg][reg].
 * Returns TRUE if valid format and registers, FALSE otherwise.
 * Sets error_status to TRUE if invalid registers are detected.
 */
int is_valid_matrix(const char *str, char *file_name, int line_number, int *error_status)
{
    char name[MAX_LABEL_LENGTH], reg1[MAX_LABEL_LENGTH], reg2[MAX_LABEL_LENGTH];
    int matched;

    matched = sscanf(str, "%[^[][%[^]]][%[^]]]", name, reg1, reg2); /* Format NAME[reg1][reg2] */

    if (matched != 3 || strlen(name) == 0) /* Check if all elements have been scanned correctly */
        return FALSE;

    if (!is_valid_register(reg1) || !is_valid_register(reg2))
    {
        announce_error(file_name, line_number, WRONG_MAT_REGS);
        *error_status = TRUE;
        return FALSE;
    }

    return TRUE;
}


/* Returns TRUE if str is outside [-MAX_VAL_DATA_INST, MAX_VAL_DATA_INST], FALSE otherwise */
int is_out_of_range(const char *str, int numBits)
{
    long val;
    val = strtol(str, NULL, 10);

    switch (numBits)
    {
        case _10BIT:
            if (val > MAX_VAL_10BIT || val < -MAX_VAL_10BIT)
                return TRUE;
        break;
        case _8BIT:
            if (val > MAX_VAL_8BIT || val < -MAX_VAL_8BIT)
                return TRUE;
        break;
    }
    return FALSE;
}