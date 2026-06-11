#include "label.h"
#include <ctype.h>
#include "globals.h"
#include "error_handler.h"
#include "string_utils.h"

/**
 * Adds a new label to the list with the given name, address, and type.
 * Allocates memory for the new label and inserts it at the head of the list.
 * Returns TRUE on success, FALSE if memory allocation fails.
 */
int add_label(Label **list, const char *name, unsigned int addr, label_type type)
{
    Label *new_label = (Label *)malloc(sizeof(Label));
    if (!new_label) return FALSE;

    strncpy(new_label->name, name, MAX_LABEL_LENGTH - 1);
    new_label->name[MAX_LABEL_LENGTH - 1] = '\0'; /* Null-terminate explicitly */
    new_label->addr = addr;
    new_label->type = type;
    new_label->next = *list;
    *list = new_label;
    return TRUE;
}

/**
 * Searches the linked list of labels for a label matching the given name.
 * Returns a pointer to the found label or NULL if not found.
 */
Label *find_label(Label *list, const char *name)
{
    char *copy = my_strdup(name);
    trim_spaces(copy);
    while (list != NULL) {
        if (strcmp(list->name, copy) == 0)
        {
            free(copy);
            return list;
        }
        list = list->next;
    }
    free(copy);
    return NULL; /* label not found */
}

/**
 * Frees all memory allocated for the labels in the linked list.
 * Iterates through the list and deallocates each label node.
 */
void free_labels(Label *list) {
    Label *tmp;
    while (list != NULL)
    {
        tmp = list;
        list = list->next;
        free(tmp);
    }
}


/**
 * Parses a line to check if it contains a label definition.
 * If a valid label is found, copies its name into label_name_out.
 * Returns TRUE if a new valid label is found, FALSE otherwise.
 * Updates error_status and reports errors on invalid labels.
 */
int handle_label_definition(const char *line, Label *labels, char *label_name_out,
                            char *file_name, int *error_status, int line_number, Macro **macro_list)
{
    const char *colon_ptr = strchr(line, ':');
    int len, i;

    if (!colon_ptr)
        return FALSE; /* No label in line */

    /* Extract label name (trimmed before colon) */
    len = (int)(colon_ptr - line);

    if (len >= MAX_LABEL_LENGTH)
    {
        announce_error(file_name, line_number, LABEL_LEN_EXCEEDED);
        *error_status = TRUE;
        return FALSE;
    }
    else if (len<=0)
    {
        announce_error(file_name, line_number, EMPTY_LABEL);
        *error_status = TRUE;
        return FALSE;
    }

    for (i = 0; i < len; i++) {
        label_name_out[i] = line[i];
    }
    label_name_out[len] = '\0'; /* Null-terminate */

    /* Validate final label name */
    if (!is_valid_new_label_name(label_name_out, labels, *macro_list, file_name, line_number, error_status)) {
        return FALSE;
    }

    return TRUE;
}

/**
 * Validates that a label name is well-formed, unique among labels and macros,
 * and does not conflict with reserved names.
 * Returns TRUE if valid, FALSE if invalid and sets error_status accordingly.
 */
int is_valid_new_label_name(const char *name, Label *labels, Macro *macro_list, const char *file_name, int line_number, int *error_status)
{
    int i, len = (int)strlen(name);

    /* Length check */
    if (len >= MAX_LABEL_LENGTH)
    {
        announce_error(file_name, line_number, LABEL_LEN_EXCEEDED);
        *error_status = TRUE;
        return FALSE;
    }
    else if (len<=0)
    {
        announce_error(file_name, line_number, EMPTY_LABEL);
        *error_status = TRUE;
        return FALSE;
    }

    /* Character validity check */
    for (i = 0; i < len; i++)
    {
        if (!isalnum(name[i]) && name[i] != '_') {
            announce_error(file_name, line_number, FORBIDDEN_CHARS_LBL);
            *error_status = TRUE;
            return FALSE;
        }
    }

    /* Check if label already exists */
    if (find_label(labels, name))
    {
        announce_error(file_name, line_number, LABEL_DUPLICATE);
        *error_status = TRUE;
        return FALSE;
    }

    /* Check if macro with same name exists */
    if (find_macro(macro_list, name)) {
        announce_error(file_name, line_number, SAVED_MCRO_NAME);
        *error_status = TRUE;
        return FALSE;
    }

    /* Check for forbidden name such as instruction/operation name */
    if (is_instruction(name)!= UNFOUND || is_operation(name)!= UNFOUND || is_valid_register(name))
    {
        announce_error(file_name,line_number,FORBIDDEN_LABEL_NAME);
        *error_status = TRUE;
        return FALSE;
    }

    return TRUE; /* All checks are valid */
}

/**
 * Updates addresses of code and data labels by adding the starting index ,and ICF offset for DATA only.
 * Entry and extern labels are not modified here.
 */
void update_labels_addr(Label *start, int ICF)
{
    Label *curr = start;
    while (curr != NULL)
    {
        if (curr->type == CODE_LBL)
        {
            curr->addr += STARTING_INDEX;
        }
        else if (curr->type == DATA_LBL)
        {
            curr->addr += STARTING_INDEX + ICF;
        }
        /* Entry labels doesn't exist at this point, and Extern labels don't contain adresses */
        curr = curr->next;
    }
}

