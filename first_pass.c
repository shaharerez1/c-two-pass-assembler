
#include "first_pass.h"

/**
 * Performs the first pass on the assembly source file.
 * Reads the file line by line, checks syntax, encodes code and data lines into internal structures,
 * manages labels and macros, and prepares for the second pass.
 * Tracks errors via `error_status`, if none found, initiate second pass.
 */
void first_pass(char *file_name, int* error_status, Macro **macro_list)
{
    cmd_line cmds_array[MAX_MACHINE_LINES]; /* As instructed - allowed to assume a maximum size for the number of machine lines */
    Label* labels = NULL; /* Initialize labels linked list */
    int ICF, DCF; /* Final counting variables for code and data lines */
    char *am_file_name = combine_extension(file_name, ".am");
    FILE *read_file = fopen(am_file_name, "r");

    syntax_check(read_file,am_file_name,error_status);

    if (!*error_status)
    {   /* Proceed to actual first pass */
        encode_lines_first(read_file,am_file_name,error_status,cmds_array, &labels, macro_list, &ICF, &DCF);
        if (!*error_status)
        { /* Proceed to second pass */
            second_pass(read_file,am_file_name,error_status,cmds_array, labels,ICF,DCF);
        }
    }
    /* Free allocated memory */
    free(am_file_name);
    free_labels(labels);
    fclose(read_file);
}

/**
 * Reads lines from file and encodes them during first pass.
 * Handles label detection and storage, distinguishes operations from instructions,
 * encodes commands accordingly, and updates instruction and data counters (ICF, DCF).
 * Uses error reporting and macro handling.
 */
void encode_lines_first(FILE *read_file, char *file_name, int *error_status,
                  cmd_line *cmds_array, Label **labels, Macro **macro_list, int *ICF, int *DCF)
{
    int IC=0,DC=0;
    char line_buffer[MAX_LINE_LENGTH];
    char label_buffer[MAX_LABEL_LENGTH];
    char *action_buffer;

    int cmd_index=0; /* Store the current command index in the entire machine lines array */
    int label_flag = FALSE; /* Flag to dictate label declaration */
    int action_key; /* will either store 0-15 OPCODE or 0-4 inst index */
    int line_number = 1;

    while (fgets(line_buffer, sizeof(line_buffer), read_file) != NULL && cmd_index < MAX_MACHINE_LINES)
    {
        label_flag = handle_label_definition(line_buffer,*labels,label_buffer
                                             ,file_name,error_status, line_number, macro_list);

        if (label_flag) shorten_string_from_start(line_buffer,(int)strlen(label_buffer)+1); /* cut label from line */
        trim_spaces(line_buffer);

        action_buffer = get_first_word(line_buffer); /* dynamically allocated memory set here, freed at loop end */

        shorten_string_from_start(line_buffer,(int)strlen(action_buffer)); /* cut action from line - remain with args */
        trim_spaces(line_buffer); /* only row of arguments */

        action_key = is_operation(action_buffer); /* First try operation name */


        if (action_key != UNFOUND)
        {
            if (label_flag)
                add_label(labels,label_buffer,IC,CODE_LBL);
            encode_op_line(cmds_array,cmd_index,action_key,line_buffer,&IC,file_name,line_number,error_status);
            cmd_index++;
        }
        else
        {
            action_key = is_instruction(action_buffer); /* Not operation name, has to be instruction */

            if (action_key != ENTRY_INST && action_key != EXTERN_INST)
            { /* data line (.string/.mat/.data) */
                if (label_flag)
                    add_label(labels,label_buffer,DC,DATA_LBL);
                encode_inst_line(cmds_array,cmd_index,action_key,line_buffer,&DC);
                cmd_index++;
            }
            else if (action_key == EXTERN_INST)
            {
                if (is_valid_new_label_name(line_buffer,*labels,*macro_list,file_name,line_number,error_status))
                    add_label(labels,line_buffer,0,EXTERN_LBL); /* add extern label with value 0 */
            } /* we didn't write a new command line, so cmd_index stays the same */
        }

        free(action_buffer);
        line_number++;
    }

    *ICF = IC; *DCF = DC; /* Update final counts */
    update_machine_lines_addr(cmds_array,*ICF); /* Add offset to every machine line */
    update_labels_addr(*labels,*ICF);  /* Add offset to every label address */
    rewind(read_file); /* Reset for later readings */
}

/**
 * Encodes a single operation (code) line into the commands array.
 * Sets opcode, addressing modes, encodes arguments depending on opcode type (0, 1, or 2 arguments),
 * and updates the instruction counter (`IC`).
 */
void encode_op_line(cmd_line *array, int index, int opcode, char *args_buffer, int* IC, char *file_name, int line_number, int *error_status)
{
    cmd_line *cmd = &array[index]; /* Get current address */
    cmd->type = CODE;
    cmd->addr = *IC;
    cmd->cmd_set.operation.type = opcode;

    switch (opcode)
    {
        case RTS_OP:
        case STOP_OP:
            cmd->req_lines = 1; /* stop/rts only require one code line - no args to encode */
            cmd->cmd_set.operation.dest_addr = EMPTY;
            cmd->cmd_set.operation.source_addr = EMPTY;
        break;

        case MOV_OP:
        case ADD_OP:
        case SUB_OP:
        case CMP_OP:
        case LEA_OP:
            encode_op_arguments(cmd,args_buffer,2,file_name,line_number, error_status); /* encode 2 args */
        break;

        default:
            encode_op_arguments(cmd,args_buffer,1,file_name,line_number, error_status); /* encode 1 arg */
    }

    (*IC) += cmd->req_lines;
}

/**
 * Encodes an instruction (data, string, matrix) line into the commands array.
 * Processes instruction type and calls corresponding encoding functions for data, strings, or matrices.
 * Updates data counter (`DC`) accordingly.
 */
void encode_inst_line(cmd_line *array, int index, int instcode, char *args_buffer, int* DC)
{
    cmd_line *cmd = &array[index]; /* Get current address */
    cmd->type = DATA;
    cmd->addr = *DC;
    cmd->cmd_set.instruction.type = instcode;

    switch (instcode)
    {
        case DATA_INST:
            encode_data_args(cmd,args_buffer);
            break;
        case STRING_INST:
            encode_string_args(cmd,args_buffer);
            break;
        case MAT_INST:
            encode_mat_args(cmd,args_buffer);
            break;
        default:
            break;
    }

    (*DC) += cmd->req_lines;
}

/**
 * Encodes the arguments for an operation line during first pass.
 * Determines source and destination addressing types, validates them,
 * inserts operand data into the command structure, and calculates how many machine lines are needed.
 */
void encode_op_arguments(cmd_line *cmd, char *args_buffer, int num_args,char *file_name,int line_number, int *error_status)
{
    /* Assisting variables to help organize the clarity of the code */
    char *src_arg;
    char *dest_arg;
    addr_type addr_type_src;
    addr_type addr_type_dest;
    int required_lines = 1; /* Starting with 1 for the operation line itself */

    remove_spaces(args_buffer); /* clear all spaces within the text for correct reading */

    if (num_args < 2) /* one argument */
    {
        dest_arg = args_buffer;
        addr_type_dest = get_addressing_type(dest_arg,file_name,line_number,error_status, &required_lines);

        /* Get the addressing type boolean flag, corresponding to the current opcode */
        if (!valid_dst_addressing[cmd->cmd_set.operation.type][addr_type_dest])
        {
            announce_error(file_name,line_number,INVALID_ADDR_TYPE_DEST);
            *error_status = TRUE;
            return;
        }
        cmd->cmd_set.operation.dest_addr = addr_type_dest;
        cmd->cmd_set.operation.source_addr = EMPTY;

        /* Finish encoding destination argument within line */
        insert_dest_data_first_pass(cmd,dest_arg);
    }

    else /* two arguments, need to use source and dest */
    {
        src_arg = strtok(args_buffer, ",");
        dest_arg = strtok(NULL, ",");
        addr_type_src = get_addressing_type(src_arg,file_name,line_number,error_status, &required_lines);
        addr_type_dest = get_addressing_type(dest_arg,file_name,line_number,error_status, &required_lines);

        /* Get the addressing type boolean flag, corresponding to the current opcode */
        if (!valid_src_addressing[cmd->cmd_set.operation.type][addr_type_src])
        {
            announce_error(file_name,line_number,INVALID_ADDR_TYPE_SOURCE);
            *error_status = TRUE;
            return;
        }
        if (!valid_dst_addressing[cmd->cmd_set.operation.type][addr_type_dest])
        {
            announce_error(file_name,line_number,INVALID_ADDR_TYPE_DEST);
            *error_status = TRUE;
            return;
        }

        cmd->cmd_set.operation.source_addr = addr_type_src; /* valid addressing - update in command struct */
        cmd->cmd_set.operation.dest_addr = addr_type_dest;

        if (addr_type_src == REG && addr_type_dest == REG)
            required_lines--; /* In case of two register arguments, we only need one code line (added 2 by now) */

        /* Finish encoding arguments within line */
        insert_src_data_first_pass(cmd,src_arg);
        insert_dest_data_first_pass(cmd,dest_arg);

    }

    cmd->req_lines = required_lines; /* Update required machine lines after processing */
}

/**
 * Encodes a string argument into the instruction's data array.
 * Extracts the string between quotes, converts each character into ASCII values stored in the command,
 * appends a null terminator, and counts required lines.
 */
void encode_string_args(cmd_line *cmd, char *args_buffer)
{
    int required_lines = 0;
    char copy[MAX_LINE_LENGTH];
    int i;

    /* Extract string from quotes */
    sscanf(args_buffer, "\"%[^\"]\"", copy);

    required_lines = (int)strlen(copy) + 1; /* +1 for null terminator */

    /* Copy ASCII values of each character into the instruction array */
    for (i = 0; i < required_lines - 1; i++) {
        cmd->cmd_set.instruction.array[i] = (unsigned char)copy[i];
    }

    /* Add null terminator as 0 */
    cmd->cmd_set.instruction.array[i] = 0;

    /* Optionally, update any metadata like how many lines were used */
    cmd->req_lines = required_lines;
}

/**
 * Encodes data arguments into the instruction's data array.
 * Parses comma-separated numeric values and stores them sequentially.
 * Updates the count of required machine lines.
 */
void encode_data_args(cmd_line *cmd, char *args_buffer)
{
    int required_lines = count_args(args_buffer); /* Number of comma-separated arguments */
    char *token;
    int i = 0;

    remove_spaces(args_buffer); /* Remove all spaces */

    token = strtok(args_buffer, ",");
    while (token != NULL && i < required_lines)
    {
        /* atoi() function is safe because we already validated that
         * the row of arguments consists of only numeric values in range */
        cmd->cmd_set.instruction.array[i++] = atoi(token);
        token = strtok(NULL, ",");
    }

    cmd->req_lines = required_lines;
}

/**
 * Encodes matrix arguments into the instruction's data array.
 * Reads matrix dimensions, parses values row-wise,
 * fills missing values with zeros, and updates required machine lines.
 */
void encode_mat_args(cmd_line *cmd, char *args_buffer)
{
    int rows, cols, required_lines, i = 0;
    char *numbers_start;
    char copy[MAX_LINE_LENGTH];
    char *token;

    strncpy(copy, args_buffer, MAX_LINE_LENGTH);
    copy[MAX_LINE_LENGTH - 1] = '\0';
    /* Create assisting copy that we can modify */

    sscanf(copy, "[%d][%d]", &rows, &cols); /* Already validated bracket structure */

    required_lines = rows * cols;
    cmd->req_lines = required_lines;

    /* Locate the start of the numbers after the brackets */
    numbers_start = strchr(copy, ']');
    numbers_start = strchr(numbers_start + 1, ']');
    numbers_start++; /* Move past the second ']' */

    remove_spaces(numbers_start);

    if (*numbers_start == '\0')
    {
        /* No values given, fill with zeros */
        for (i = 0; i < required_lines; i++)
        {
            cmd->cmd_set.instruction.array[i] = 0;
        }
        return;
    }

    /* Parse and store the values */
    token = strtok(numbers_start, ",");
    while (token != NULL && i < required_lines)
    {
        cmd->cmd_set.instruction.array[i++] = atoi(token); /* Already validated actual values */
        token = strtok(NULL, ",");
    }

    /* Fill any remaining entries with 0 */
    while (i < required_lines)
    {
        cmd->cmd_set.instruction.array[i++] = 0;
    }
}

/**
 * Inserts the destination operand's data into the command during the first pass.
 * Handles immediate, register, and matrix addressing modes specifically.
 */
void insert_dest_data_first_pass(cmd_line *cmd, char *arg)
{
    int mat_val1, mat_val2;

    switch (cmd->cmd_set.operation.dest_addr)
    {
        case IMDT:
            cmd->cmd_set.operation.dest_arg.ARE = ABSOLUTE;
            cmd->cmd_set.operation.dest_arg.data.val_8bit = (int)strtol(arg + 1, NULL, 10); /* num after # */
            break;

        case REG:
            cmd->cmd_set.operation.dest_arg.ARE = ABSOLUTE;
            cmd->cmd_set.operation.dest_arg.data.sep_data.right_4bit = (int)strtol(arg + 1, NULL, 10); /* num after 'r' */
            cmd->cmd_set.operation.dest_arg.data.sep_data.left_4bit = EMPTY;
            break;

        case MTRX:
            sscanf(arg, "%*[^[][%*c%d][%*c%d]", &mat_val1, &mat_val2); /* sscanf is safe because of the checks we did */
            cmd->cmd_set.operation.mtrx_regs_dest.ARE = ABSOLUTE;
            cmd->cmd_set.operation.mtrx_regs_dest.data.sep_data.left_4bit = mat_val1;
            cmd->cmd_set.operation.mtrx_regs_dest.data.sep_data.right_4bit = mat_val2;
            break;

        default: /* case of direct addressing - we don't know any details at this point */
            break;
    }
}

/**
 * Inserts the source operand's data into the command during the first pass.
 * Handles immediate, register, and matrix addressing modes specifically.
 */
void insert_src_data_first_pass(cmd_line *cmd, char *arg)
{
    int mat_val1, mat_val2;

    switch (cmd->cmd_set.operation.source_addr)
    {
        case IMDT:
            cmd->cmd_set.operation.source_arg.ARE = ABSOLUTE;
            cmd->cmd_set.operation.source_arg.data.val_8bit = (int)strtol(arg + 1, NULL, 10); /* num after # */
            break;

        case REG:
            cmd->cmd_set.operation.source_arg.ARE = ABSOLUTE;
            cmd->cmd_set.operation.source_arg.data.sep_data.left_4bit = (int)strtol(arg + 1, NULL, 10); /* num after 'r' */
            cmd->cmd_set.operation.source_arg.data.sep_data.right_4bit = EMPTY;
            break;

        case MTRX:
            sscanf(arg, "%*[^[][%*c%d][%*c%d]", &mat_val1, &mat_val2); /* sscanf is safe because of the checks we did */
            cmd->cmd_set.operation.mtrx_regs_src.ARE = ABSOLUTE;
            cmd->cmd_set.operation.mtrx_regs_src.data.sep_data.left_4bit = mat_val1;
            cmd->cmd_set.operation.mtrx_regs_src.data.sep_data.right_4bit = mat_val2;
            break;

        default: /* case of direct addressing - we don't know any details at this point */
            break;
    }
}

/**
 * Determines the addressing type of a given operand string.
 * Returns one of the addressing types: IMDT, DIRECT, MTRX, REG.
 * Validates operand format and updates required lines count accordingly.
 */
addr_type get_addressing_type(const char *operand, char *file_name, int line_number, int *error_status, int* req_lines)
{
    if (operand[0] == '#')
    {
        if (is_valid_number(operand + 1) && !is_out_of_range(operand + 1,_8BIT))
        { /* Valid 8-bit number */
            (*req_lines)++;
            return IMDT;
        }
        announce_error(file_name,line_number,INVALID_IMDT);
        *error_status = TRUE;
        return IMDT;
    }
    else if (is_valid_register(operand))
    {
        (*req_lines)++;
        return REG;
    }
    else if (is_valid_matrix(operand,file_name,line_number,error_status))
    {
        (*req_lines)+=2; /* two additional lines for matrix addressing */
        return MTRX;
    }
    else
    { /* Default case is direct addressing - Will be checked in second pass*/
        (*req_lines)++;
        return DIRECT;
    }
}

/**
 * Performs a syntax check on the input assembly file.
 * Reads line by line and validates comma placement, label correctness,
 * and argument syntax for instructions and operations.
 * Sets error status if any violations are detected.
 */
void syntax_check(FILE* read_file, char* file_name, int* error_status)
{
    char buffer[MAX_LINE_LENGTH];
    char *temp_buffer;
    char* action_word;
    int line_number = 1, action_num;

    while (fgets(buffer, sizeof(buffer), read_file) != NULL)
    {
        /* Comma checks */
        if (has_leading_or_trailing_comma(buffer, file_name,line_number) || has_double_comma(buffer, file_name,line_number))
        {
            *error_status = TRUE;
            line_number++;
            continue;
        }

        temp_buffer = trim_label(buffer,file_name,line_number);
        if (temp_buffer == NULL)
        { /* invalid label definition detected */
            *error_status = TRUE;
            line_number++;
            continue;
        }

        action_word = get_first_word(temp_buffer); /* Extract instruction/operation name */

        trim_spaces(temp_buffer);
        shorten_string_from_start(temp_buffer,(int)strlen(action_word));
        /*temp_buffer now holds only the arguments buffer*/

        action_num = is_instruction(action_word);

        /* Validate first word - either operation or instruction */
        if (action_num != UNFOUND) /* case of instruction */
        {
            validate_instruction_args(action_num,temp_buffer,error_status,file_name,line_number);
        }
        else /* case of operation or error */
        {
            action_num = is_operation(action_word);
            if (action_num == UNFOUND)
            {
                announce_error(file_name, line_number, UNDEFINED_ACTION);
                *error_status = TRUE;
                line_number++;
                free(action_word);
                continue;
            }
            validate_operation_args(action_num,temp_buffer,error_status,file_name,line_number);
        }
        line_number++;
        free(action_word); /* Free allocated memory */
    }
    rewind(read_file);
}

/**
 * Validates the arguments of an instruction line based on its type.
 * Calls the appropriate validation function for matrix, data, string, entry, or extern instructions.
 */
void validate_instruction_args(int action_num, char *temp_buffer,int *error_status, char *file_name, int line_number)
{
    inst_type type = action_num; /* action_num will always correspond with inst_type ENUM */
    switch(type)
    {
        case MAT_INST:
            validate_mat_syntax(temp_buffer,file_name,line_number,error_status);
            break;
        case DATA_INST:
            validate_data_syntax(temp_buffer,file_name,line_number,error_status);
            break;
        case STRING_INST:
            validate_string_syntax(temp_buffer,file_name,line_number,error_status);
            break;
        case ENTRY_INST:
        case EXTERN_INST:
            validate_ent_ext_syntax(temp_buffer,file_name,line_number,error_status);
    }
}

/**
 * Validates the arguments of an operation line.
 * Checks comma placement and the number of arguments against expected counts.
 * Sets error status if too few or too many arguments are found.
 */
void validate_operation_args(int action_num, char *temp_buffer,int *error_status, char *file_name, int line_number)
{
    int actual_args=0;

    if (has_missing_comma(temp_buffer,file_name,line_number) || has_leading_or_trailing_comma(temp_buffer,file_name,line_number))
    {
        *error_status = TRUE;
        return;
    }

    actual_args = count_args(temp_buffer);

    if (actual_args < operations[action_num].num_args)
    {
        announce_error(file_name, line_number, FEW_ARGS);
        *error_status = TRUE;
    }
    else if (actual_args > operations[action_num].num_args)
    {
        announce_error(file_name, line_number, MANY_ARGS);
        *error_status = TRUE;
    }
}

/**
 * Updates the addresses of machine code and data lines after first pass.
 * Adjusts code lines by starting index and data lines by starting index plus instruction counter.
 */
void update_machine_lines_addr(cmd_line *array, int ICF)
{
    int i;

    for (i=0;i<MAX_MACHINE_LINES;i++)
    {
        if (array[i].type == CODE)
            array[i].addr+=STARTING_INDEX;
        else if (array[i].type == DATA)
            array[i].addr+= STARTING_INDEX + ICF; /* Data lines always come after code lines */
    }
}