#include "second_pass.h"
#include "output_handler.h"

/**
 * Executes the second pass on the assembly source.
 * Resolves labels, encodes instructions, handles externals, and generates output files.
 * Frees external references list after processing.
 */
void second_pass(FILE *read_file,char *file_name,int *error_status, cmd_line *cmds_array, Label *labels,int ICF, int DCF)
{
    externs_list *externs_head = NULL; /* Initialize externs usages list */
    encode_lines_second(read_file,file_name,error_status,cmds_array,labels,&externs_head);
    if (!(*error_status))
        create_output_files(cmds_array,labels,externs_head,file_name,ICF,DCF);
    free_externs_list(externs_head); /* Free allocated memory */
}

/**
 * Reads lines during the second pass, extracts operands, resolves labels.
 * Assigns entry labels and updates command array.
 * Tracks errors for unresolved or invalid labels.
 */
void encode_lines_second(FILE *read_file,char *file_name,int *error_status
                        ,cmd_line *cmds_array,Label *labels,externs_list **externs_head)
{
    char line_buffer[MAX_LINE_LENGTH];
    char *temp_buffer,*action_buffer;
    int cmd_index = 0, line_number = 1, action_key;;
    while (fgets(line_buffer, sizeof(line_buffer), read_file) != NULL && cmd_index < MAX_MACHINE_LINES)
    {

        /* The following line are similar to the first pass, extracting needed parts of the current line */

        temp_buffer = trim_label(line_buffer,file_name,0);
        action_buffer = get_first_word(temp_buffer); /* dynamically allocated memory set here, freed at loop end */
        trim_spaces(temp_buffer);

        shorten_string_from_start(temp_buffer,(int)strlen(action_buffer)); /* cut action from line - remain with args */
        trim_spaces(temp_buffer); /* only row of arguments */

        action_key = is_instruction(action_buffer);

        if (action_key != UNFOUND)
        {
            if (action_key != ENTRY_INST && action_key != EXTERN_INST)
                cmd_index++; /* already fully encoded data lines (.data/.mat/.string) */
            else if (action_key == ENTRY_INST)
                assign_entry_label(temp_buffer,labels,file_name,line_number,error_status);
        }
        else /* Operation detected */
        {
            finish_op_encoding(cmds_array,cmd_index,temp_buffer,labels,file_name,line_number,error_status, externs_head);
            cmd_index++;
        }

        line_number++;
        free(action_buffer);
    }
}

/**
 * Finalizes encoding of command operands by parsing args and encoding labels.
 * Handles source and destination operands for direct and matrix addressing modes.
 */
void finish_op_encoding(cmd_line *cmd_array, int cmd_index,char *args_buffer, Label *labels,
                        char *file_name,int line_number, int *error_status, externs_list **externs_head)
{
    cmd_line *cmd = &cmd_array[cmd_index];
    char *arg1 = strtok(args_buffer, ","); /* Get first arg */
    char *arg2 = strtok(NULL, ","); /* Get second arg, if none - won't be used, arg1 will represent dest arg */
    unsigned int index_in_line = 1; /* Index of the specific argument line within the whole lines of the command */

    /* Value index_in_line will be used to find the specific address, in case of external symbol being found */

    if (arg2) /* there are 2 arguments in this line */
    {
        /* begin with checking the first argument (source) */
        if (cmd->cmd_set.operation.source_addr == DIRECT)
        {
            encode_label_arg_source(cmd,arg1,labels,externs_head,file_name,line_number,error_status,index_in_line);
            index_in_line++;
        }
        else if (cmd->cmd_set.operation.source_addr == MTRX)
        {
            strchr(arg1, '[')[0] = '\0'; /* Keep only the matrix name */
            encode_label_arg_source(cmd,arg1,labels,externs_head,file_name,line_number,error_status,index_in_line);
            index_in_line+=2; /* MTRX addressing requires 2 lines */
        }
        else /* Source argument is either immediate or register, no need to encode, just update line index */
            index_in_line++;

        /* continue to second argument (dest) */
        if (cmd->cmd_set.operation.dest_addr == DIRECT)
        {
            encode_label_arg_dest(cmd,arg2,labels,externs_head,file_name,line_number,error_status,index_in_line);
        }
        else if (cmd->cmd_set.operation.dest_addr == MTRX)
        {
            strchr(arg2, '[')[0] = '\0'; /* Keep only the matrix name */
            encode_label_arg_dest(cmd,arg2,labels,externs_head,file_name,line_number,error_status,index_in_line);
        }
    }
    else /* Only one argument - which will always be dest argument */
    {
        if (cmd->cmd_set.operation.dest_addr == DIRECT)
        {
            encode_label_arg_dest(cmd,arg1,labels,externs_head,file_name,line_number,error_status,index_in_line);
        }
        else if (cmd->cmd_set.operation.dest_addr == MTRX)
        {
            strchr(arg1, '[')[0] = '\0'; /* Keep only the matrix name */
            encode_label_arg_dest(cmd,arg1,labels,externs_head,file_name,line_number,error_status,index_in_line);
        }
    }
}

/**
 * Encodes a source operand label by resolving its address and setting ARE bits.
 * Reports error if label not found.
 */
void encode_label_arg_source(cmd_line *cmd,char *arg,Label *labels,externs_list **externs_head,
                             char *file_name,int line_number,int *error_status, unsigned int index_in_line)
{
    Label *temp_label;

    temp_label = find_label(labels,arg);

    if (temp_label == NULL) /* Uninitialized label */
    {
        announce_error(file_name,line_number,UNRESOLVED_LABEL);
        *error_status = TRUE;
    }
    else
    {
        if (temp_label->type == EXTERN_LBL)
        {
            add_to_externs_list(cmd->addr+index_in_line,temp_label,externs_head);
            cmd->cmd_set.operation.source_arg.ARE = EXTERNAL; /* Add external ARE to argument data */
        }
        else
            cmd->cmd_set.operation.source_arg.ARE = RELOCATABLE; /* Non-external labels are always relocatable */

        /* Direct/Matrix addressing argument is always represented by 8-2 bit structure */
        cmd->cmd_set.operation.source_arg.data.val_8bit = temp_label->addr;
    }
}

/**
 * Encodes a destination operand label by resolving its address and setting ARE bits.
 * Reports error if label not found.
 */
void encode_label_arg_dest(cmd_line *cmd,char *arg,Label *labels,externs_list **externs_head,
                             char *file_name,int line_number,int *error_status, unsigned int index_in_line)
{
    Label *temp_label;

    temp_label = find_label(labels,arg);

    if (temp_label == NULL) /* Uninitialized label */
    {
        announce_error(file_name,line_number,UNRESOLVED_LABEL);
        *error_status = TRUE;
    }
    else
    {
        if (temp_label->type == EXTERN_LBL)
        {
            add_to_externs_list(cmd->addr+index_in_line,temp_label,externs_head);
            cmd->cmd_set.operation.dest_arg.ARE = EXTERNAL; /* Add external ARE to argument data */
        }
        else
            cmd->cmd_set.operation.dest_arg.ARE = RELOCATABLE; /* Non-external labels are always relocatable */

        /* Direct/Matrix addressing argument is always represented by 8-2 bit structure */
        cmd->cmd_set.operation.dest_arg.data.val_8bit = (int)temp_label->addr;
    }
}

/**
 * Adds an external label reference with its usage address to the externs list.
 */
void add_to_externs_list(unsigned int addr, Label *label, externs_list **head)
{
    externs_list *new_node;
    externs_list *current;

    new_node = (externs_list *)malloc(sizeof(externs_list));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    strncpy(new_node->name, label->name, MAX_LABEL_LENGTH - 1);
    new_node->name[MAX_LABEL_LENGTH - 1] = '\0'; /* Make sure of end of str */
    new_node->usage_addr = addr;
    new_node->next = NULL;

    if (*head == NULL) /* Case of empty list */
        *head = new_node;
    else
    {
        current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

/**
 * Frees the linked list tracking external label references.
 */
void free_externs_list(externs_list *head)
{
    externs_list *tmp;
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

/**
 * Marks a label as ENTRY if it exists and is not external.
 * Reports an error if label is external or missing.
 */
void assign_entry_label(char *entry_label_name,Label *labels,char *file_name,int line_number, int *error_status)
{
    while (labels != NULL)
    {
        if (strcmp(labels->name, entry_label_name) == 0)
        {
            if (labels->type == EXTERN_LBL) /* Label name could not be both entry and external */
            {
                announce_error(file_name,line_number,BOTH_ENTRY_EXTERN_LBL);
                *error_status = TRUE;
                return;
            }
            labels->type=ENTRY_LBL;
            return;
        }
        labels = labels->next;
    }

    announce_error(file_name,line_number,ENTRY_NOT_IMPLEMENTED);
    *error_status = TRUE;
}
