#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pre_assembler.h"
#include "defined_strings.h"
#include "string_utils.h"
#include "error_handler.h"

/**
 Reads the .as source file, processes macros by expanding them, and writes the output to a .am file.
 Returns a linked list of macros found or NULL if an error occurred.
 Sets error_status flag if errors were detected during processing.
*/
Macro* pre_assembler(char* file_name, int *error_status)
{
    char* as_file_name;
    char* am_file_name;
    FILE* read_file;
    FILE* write_file;
    Macro* head; /* Initialize macros list */

    /* Get actual file names */
    as_file_name = combine_extension(file_name, ".as");
    am_file_name = combine_extension(file_name, ".am");

    /* Open files */
    read_file = fopen(as_file_name, "r");
    write_file = fopen(am_file_name, "w");

    if (read_file == NULL || write_file == NULL) /* check if both files are opened correctly */
    {
        perror("Error opening files");
        *error_status = TRUE;
        if (read_file) fclose(read_file);
        if (write_file)
        {
            fclose(write_file);
            remove(am_file_name);
        }
        free(as_file_name); free(am_file_name);
        return NULL;
    }

    /* Main pre-processing done here */
    head = process_lines(read_file, write_file, as_file_name, error_status);

    fclose(read_file);
    free(as_file_name);
    fclose(write_file);

    if (*error_status) /* In case of errors, no need for .am file */
        remove(am_file_name);

    free(am_file_name);
    return head;
}

/**
 Reads each line from the input file, handles macro definitions and expansions,
 writes processed lines to output, and returns the macro list.
 Updates error_status if syntax or macro errors occur.
*/
Macro* process_lines(FILE* read_file, FILE* write_file, char* file_name, int *error_status)
{
    char buffer[MAX_LINE_LENGTH], temp_buffer[MAX_LINE_LENGTH], *first_word;
    Macro *macro_list = NULL;
    char macro_name[MAX_LINE_LENGTH], *macro_content = NULL;
    size_t macro_size=0;
    int in_macro=0,line_number = 1;

    while (fgets(buffer, sizeof(buffer), read_file) != NULL)
    {
        if (line_is_too_long(buffer, read_file, file_name, line_number, error_status))
        {
            line_number++;
            continue;
        }
        strcpy(temp_buffer, buffer);
        buffer[strcspn(buffer, "\n")] = '\0'; /* remove new line char */

        if (is_empty_str(buffer) || strncmp(buffer, ";", 1) == 0)
        { /* Ignore empty or comment lines */
            line_number++;
            continue;
        }

        trim_spaces(temp_buffer);
        first_word = get_first_word(temp_buffer); /* Store first word - might be mcro/mcroend */
        shorten_string_from_start(temp_buffer, (int)strlen(first_word));

        /* temp_buffer now holds the string without the first word - will be used if a new macro is being defined */

        if (in_macro)
        {
            if (reached_end_of_macro(first_word,macro_name,&macro_content,&macro_size,&macro_list,&in_macro))
            {
                if (!is_empty_str(temp_buffer))
                {
                    *error_status = TRUE;
                    announce_error(file_name,line_number,EXTRANEOUS_MACRO_DEF);
                }
                line_number++;
                continue;
            }
            else /* we need to add the next line to the current macro */
            {
                if (append_line_to_macro(buffer,&macro_content,&macro_size,macro_list,error_status))
                    return NULL; /* memory allocation problem */
                line_number++;
                continue;
            }
        }
        if (check_new_macro_start(first_word, temp_buffer, macro_name,
                                  &in_macro, &macro_content, &macro_size))
        {
            check_valid_macro_start(macro_name,temp_buffer,file_name,error_status, line_number,macro_list);
            line_number++;
            continue;
        }

        /* reaching here means we are not in a macro, and we are not defining a new macro */
        /* so we either have a regular line, or an existing macro which needs  to be written */
        write_line_or_macro_to_file(write_file,macro_list,buffer);
        line_number++;
        free(first_word);
    }

    return macro_list;
}

/**
 Checks if the current line marks the end of a macro definition.
 If so, adds the macro to the list, resets relevant variables, and returns TRUE.
 Otherwise, returns FALSE.
*/
int reached_end_of_macro(char *first_word,char *macro_name,char **macro_content,size_t *macro_size,Macro **macro_list,int *in_macro)
{
    if (strcmp(first_word, MACRO_END_STR) == 0)
    {
        /* Add the final macro data to the linked list */
        *macro_list = add_macro(*macro_list, macro_name, *macro_content);

        if (*macro_content)
        {
            free(*macro_content);
            *macro_content = NULL;
        }

        /* update flags */
        *macro_size = 0;
        *in_macro = 0;
        return TRUE;
    }
    return FALSE;
}

/**
 Appends the given line to the current macro's content buffer,
 reallocating memory as needed.
 Returns TRUE if memory allocation fails, FALSE otherwise.
*/
int append_line_to_macro(char *line,char **macro_content,size_t *macro_size,Macro *macro_list,int *error_status)
{
    size_t new_len;
    char *new_content;

    new_len = strlen(line) + 1 + *macro_size + 1;

    new_content = realloc(*macro_content, new_len);
    if (!new_content)
    {
        perror("realloc failed");
        free_macros(macro_list);
        *error_status = TRUE;
        return TRUE;
    }
    *macro_content = new_content;

    /* Check for first row */
    if (*macro_size == 0)
        (*macro_content)[0] = '\0';

    strcat(*macro_content, line);
    strcat(*macro_content, "\n");

    *macro_size = strlen(*macro_content);

    return FALSE;
}

/**
 Detects the start of a new macro definition.
 If found, initializes macro variables and returns TRUE, else FALSE.
*/
int check_new_macro_start(const char* first_word,char* remaining_text,char* macro_name,
                        int* in_macro,char** macro_content,size_t* macro_size)
{
    char* macro_id;

    if (strcmp(first_word, MACRO_START_STR) == 0)
    {
        macro_id = get_first_word(remaining_text);
        if (macro_id)
        {
                strncpy(macro_name, macro_id, MAX_LINE_LENGTH);
                macro_name[MAX_LINE_LENGTH - 1] = '\0';

                *in_macro = TRUE;
                *macro_content = NULL;
                *macro_size = 0;

                free(macro_id);
        }
        return TRUE;
    }
    return FALSE;
}

/**
 Writes a line to the output file.
 If the line matches a macro name, writes the macro content instead.
*/
void write_line_or_macro_to_file(FILE* write_file, Macro* current_list,char* buffer)
{
    Macro* existing_macro;
    existing_macro = find_macro(current_list, buffer); /* will return NULL if doesn't exist as macro */

    if (existing_macro) /* Write macro content to write file */
        fputs(existing_macro->content, write_file);
    else
    { /* Keep given line */
        fputs(buffer, write_file);
        fputc('\n', write_file);
    }
}

/**
 Checks if a line is too long (missing newline before EOF),
 sets error flag and returns TRUE if so, else FALSE.
 On case of overflowing line, flush until the line end, to not interrupt further readings from file
*/
int line_is_too_long(const char buffer[MAX_LINE_LENGTH], FILE* read_file,
                     const char* file_name, int line_number, int* error_status)
{
    if (buffer[strlen(buffer) - 1] != '\n' && !feof(read_file))
    {
        int c;
        /* flush until end of line */
        while ((c = fgetc(read_file)) != '\n' && c != EOF)
            ;

        announce_error(file_name, line_number, MAX_LEN_EXCEEDED);
        *error_status = TRUE;
        return TRUE;
    }
    return FALSE;
}
/**
 Validates macro name for conflicts and extra text after macro name definition,
 sets error flag if invalid.
*/
void check_valid_macro_start(char* macro_name,char* reminder_text, char* file_name,int *error_status, int line_number, Macro *head)
{
    int i=0;
    char* copy;
    copy = malloc(strlen(reminder_text) + 1);
    if (copy)
        strcpy(copy, reminder_text);

    trim_spaces(copy); /* trimmed version of the entire string reminder after mcro defintion
                          will be used to compare to the macro name, to check if there is any extra text */

    if (find_macro(head, macro_name) != NULL)
    {
        *error_status = TRUE;
        announce_error(file_name, line_number, DUPLICATE_MACRO);
        return;
    }

    /* Check if macro name is a safe operation name */
    for (; i<num_operations;i++)
    {
        if (strcmp(macro_name, operations[i].name) == 0)
        {
            *error_status = TRUE;
            announce_error(file_name, line_number, FORBIDDEN_MACRO_NAME);
            return;
        }
    }
    i=0;

    /* Check if macro name is a safe instruction name */
    for (; i<num_instructions;i++)
    {
        if (strcmp(macro_name, instruction_names[i]) == 0)
        {
            *error_status = TRUE;
            announce_error(file_name, line_number, FORBIDDEN_MACRO_NAME);
            return;
        }
    }

    if (strcmp(macro_name, copy) != 0) /* If the remaining text isn't only the macro name, there is extra text */
    {
        *error_status = TRUE;
        announce_error(file_name, line_number, EXTRANEOUS_MACRO_DEF);
    }

    free(copy);
}

/**
 Creates a new macro struct with given name and content,
 returns pointer to new macro (or old list head on failure).
*/
Macro* add_macro(Macro *head, const char *name, const char *content)
{
    Macro *new_macro = malloc(sizeof(Macro));
    if (!new_macro)
    {
        perror("malloc failed");
        return head;
    }

    strncpy(new_macro->name, name, MAX_LINE_LENGTH);
    new_macro->name[MAX_LINE_LENGTH - 1] = '\0';

    new_macro->content = malloc(strlen(content) + 1);
    if (!new_macro->content)
    {
        perror("malloc failed");
        free(new_macro);
        return head;
    }
    strcpy(new_macro->content, content);

    new_macro->next = head;
    return new_macro;
}

/**
 Finds a macro by name in the linked list,
 returns pointer to macro or NULL if not found.
*/
Macro* find_macro(Macro *head, const char *name)
{
    Macro *curr = head;
    char* copy;
    copy = malloc(strlen(name) + 1);
    if (copy)
        strcpy(copy, name);

    /* name might contain spaces, so we need to ignore them */
    trim_spaces(copy);

    while (curr) {
        if (strcmp(curr->name, copy) == 0)
        {
            free(copy);
            return curr;
        }
        curr = curr->next;
    }
    free(copy);
    return NULL;
}

/**
 Frees all memory allocated for the macro linked list.
*/
void free_macros(Macro *head)
{
    Macro *curr = head;
    while (curr)
    {
        Macro *tmp = curr;
        curr = curr->next;
        free(tmp->content);
        free(tmp);
    }
}
