#include "output_handler.h"
#include <stddef.h>

/**
Creates the output files (.obj, .ext, .ent) based on the assembled commands,
labels, and externals. It generates the filenames, creates the extern and entry files
if applicable, and writes the object file before freeing allocated filenames.
*/
void create_output_files(cmd_line *cmds_array,Label *labels,externs_list *externs_head,
                         const char *file_name,const int ICF,const int DCF)
{
    char *obj_filename, *ext_filename, *ent_filename;
    Label *temp_ptr = labels;

    /* Conjoin original name with the 3 corresponding file extensions */
    create_filenames(file_name,&obj_filename,&ext_filename,&ent_filename);

    if (externs_head != NULL) create_ext_file(ext_filename,externs_head);

    while (temp_ptr != NULL)
    {
        if (temp_ptr->type == ENTRY_LBL)
        {
            create_ent_file(ent_filename,temp_ptr); /* Start from the first .entry label */
            break;
        }
        temp_ptr = temp_ptr->next;
    }

    create_obj_file(obj_filename,cmds_array,ICF,DCF);

    free(obj_filename);
    free(ext_filename);
    free(ent_filename);
}

/**
Creates the .obj file by writing encoded commands and data.
It loops through all commands, writing code instructions first with their addresses,
opcodes, and operands, then writes data instructions separately.
*/
void create_obj_file(char *obj_filename, cmd_line *cmds_array, int ICF, int DCF)
{
    FILE *write_file = fopen(obj_filename, "w");
    cmd_line *curr_cmd;
    int i;

    /* First line represents the final count of IC and DC  */
    write_address(write_file,ICF,0);
    write_address(write_file,DCF,0);
    fputc('\n',write_file);

    /* Start writing actual lines of code */
    for (i = 0; i < ICF + DCF; i++) /* Loop through operations only - need to output them first */
    {
        curr_cmd = &cmds_array[i];
        if (curr_cmd->type == CODE && curr_cmd->req_lines >= 1)
        {
            /* Write address */
            write_address(write_file, (int)curr_cmd->addr, 0);

            /* Write opcode (2 characters) */
            write_value(write_file, (int)curr_cmd->cmd_set.operation.type, 2);

            /* Write source addressing method (1 character) */
            write_value(write_file, (int)curr_cmd->cmd_set.operation.source_addr, 1);

            /* Write destination addressing method (1 character) */
            write_value(write_file, (int)curr_cmd->cmd_set.operation.dest_addr, 1);

            fputc('a', write_file); /* The first line ARE is always 0 */
            fputc('\n', write_file);

            write_op_arguments(curr_cmd, write_file);
        }
    }
    for (i = 0; i < ICF + DCF; i++) /* Loop through data instructions only - .mat/.data/.string */
    {
        curr_cmd = &cmds_array[i];
        if (curr_cmd->type == DATA && curr_cmd->req_lines >= 1)
        {
            write_inst_arguments(curr_cmd,write_file);
        }
    }

    fclose(write_file);
}

/**
Writes the data section arguments to the file.
It iterates through all required lines of the command's data and writes the address
and data value, handling negative values by converting them using two's complement.
*/
void write_inst_arguments(cmd_line *curr_cmd, FILE *write_file)
{
    int temp_num;
    int index_in_line; /* data arguments encoding starts from the line's address itself */

    for (index_in_line=0; index_in_line < curr_cmd->req_lines; index_in_line++)
    {
        write_address(write_file, (int)curr_cmd->addr, index_in_line);
        temp_num = curr_cmd->cmd_set.instruction.array[index_in_line];
        if (temp_num < 0)
            temp_num = MAX_VAL_10BIT*2 + temp_num; /* 2's complement method */
        write_value(write_file, temp_num, 5); /* data values are represented by the entire width */
        fputc('\n', write_file);
    }
}

/**
Writes the machine code for an operation's arguments.
Handles different operand types and addresses, writing registers or values
accordingly, and uses a special case when both arguments are registers.
*/
void write_op_arguments(cmd_line *cmd,FILE *file)
{
    int index_in_line = 1; /* Already wrote the first machine line of the command */

    switch(cmd->cmd_set.operation.type)
    {
        /* No arguments to write */
        case RTS_OP:
        case STOP_OP:
            break;

        case MOV_OP:
        case ADD_OP:
        case SUB_OP:
        case CMP_OP:
        case LEA_OP:
            if (cmd->cmd_set.operation.source_addr == cmd->cmd_set.operation.dest_addr
                && cmd->cmd_set.operation.source_addr == REG)
            { /* Special case of both arguments being registers */
                write_address(file, (int)cmd->addr, index_in_line);
                write_registers(file, cmd->cmd_set.operation.source_arg.data.sep_data.left_4bit,
                                      cmd->cmd_set.operation.dest_arg.data.sep_data.right_4bit);
            }
            else
            {
                write_source_argument(cmd, file, &index_in_line);
                write_dest_argument(cmd, file, &index_in_line);
            }
            break;

        default:
            write_dest_argument(cmd, file, &index_in_line);
    }
}

/**
Writes the source operand of an operation to the output file.
Supports matrix, register, immediate, and direct addressing modes,
writing the appropriate address, value, and ARE bits.
*/
void write_source_argument(cmd_line *cmd, FILE *file, int *index_in_line)
{
    switch(cmd->cmd_set.operation.source_addr)
    {
        case MTRX:
            /* First line: matrix label */
            write_address(file, (int)cmd->addr, *index_in_line);
            write_value(file, cmd->cmd_set.operation.source_arg.data.val_8bit, 4);
            write_ARE(file, cmd->cmd_set.operation.source_arg.ARE);
            (*index_in_line)++;

            /* Second line: matrix registers */
            write_address(file, (int)cmd->addr, *index_in_line);
            write_registers(file,
                cmd->cmd_set.operation.mtrx_regs_src.data.sep_data.left_4bit,
                cmd->cmd_set.operation.mtrx_regs_src.data.sep_data.right_4bit);
            (*index_in_line)++;
            break;

        case REG:
            write_address(file, (int)cmd->addr, *index_in_line);
            write_registers(file,
                cmd->cmd_set.operation.source_arg.data.sep_data.left_4bit,
                cmd->cmd_set.operation.source_arg.data.sep_data.right_4bit);
            (*index_in_line)++;
            break;

        case IMDT:
        case DIRECT:
            write_address(file, (int)cmd->addr, *index_in_line);
            write_value(file, cmd->cmd_set.operation.source_arg.data.val_8bit, 4);
            write_ARE(file, cmd->cmd_set.operation.source_arg.ARE);
            (*index_in_line)++;
            break;
    }
}

/**
Writes the destination operand of an operation to the output file.
Supports matrix, register, immediate, and direct addressing modes,
writing the appropriate address, value, and ARE bits.
*/
void write_dest_argument(cmd_line *cmd, FILE *file, int *index_in_line)
{
    switch(cmd->cmd_set.operation.dest_addr)
    {
        case MTRX:
            /* First line: matrix label */
            write_address(file, (int)cmd->addr, *index_in_line);
            write_value(file, cmd->cmd_set.operation.dest_arg.data.val_8bit, 4);
            write_ARE(file, cmd->cmd_set.operation.dest_arg.ARE);
            (*index_in_line)++;

        /* Second line: matrix registers */
            write_address(file, (int)cmd->addr, *index_in_line);
            write_registers(file,
            cmd->cmd_set.operation.mtrx_regs_dest.data.sep_data.left_4bit,
            cmd->cmd_set.operation.mtrx_regs_dest.data.sep_data.right_4bit);
            (*index_in_line)++;
            break;

        case REG:
            write_address(file, (int)cmd->addr, *index_in_line);
            write_registers(file,
            cmd->cmd_set.operation.dest_arg.data.sep_data.left_4bit,
            cmd->cmd_set.operation.dest_arg.data.sep_data.right_4bit);
            (*index_in_line)++;
            break;

        case IMDT:
        case DIRECT:
            write_address(file, (int)cmd->addr, *index_in_line);
            write_value(file, cmd->cmd_set.operation.dest_arg.data.val_8bit, 4);
            write_ARE(file, cmd->cmd_set.operation.dest_arg.ARE);
            (*index_in_line)++;
            break;
    }
}

/**
Writes the address field to the output file.
It converts the address plus index offset to the custom base-4 format
and writes it followed by a tab.
*/
void write_address(FILE *file, int addr, int index)
{
    char *temp_str = convertToBase4Custom_NoPadding(addr + index);
    fputs(temp_str, file);
    free(temp_str);
    fputc('\t', file);
}

/**
Writes a value field to the output file in the custom base-4 format.
It converts the integer value to the base-4 string and writes it.
*/
void write_value(FILE *file, int value, int width)
{
    char *temp_str = convertToBase4Custom(value, width);
    fputs(temp_str, file);
    free(temp_str);
}

/**
Writes the ARE (Absolute/Relocatable/External) field to the output file.
It converts the ARE integer to base-4 and writes it followed by a newline.
*/
void write_ARE(FILE *file, int are)
{
    char *temp_str = convertToBase4Custom(are, 1);
    fputs(temp_str, file);
    free(temp_str);
    fputc('\n', file);
}

/**
Writes two register values and a trailing 'a' character to the output file,
representing the combined register operands in the encoded instruction.
*/
void write_registers(FILE *file, int left_reg, int right_reg)
{
    write_value(file, left_reg, 2);
    write_value(file, right_reg, 2);
    fputc('a', file); /* Register ARE is always absolute */
    fputc('\n', file);
}

/**
Creates the .ext file listing external label usages.
It writes each external label name and its usage address in base-4 format.
*/
void create_ext_file(char *ext_filename,externs_list *externs_head)
{
    FILE *write_file = fopen(ext_filename, "w");
    char* temp_str;

    while (externs_head != NULL)
    {
        fputs(externs_head->name, write_file);
        fputs("\t", write_file);
        temp_str = convertToBase4Custom_NoPadding((int)externs_head->usage_addr);
        fputs(temp_str, write_file);
        fputs("\n", write_file);

        free(temp_str);
        externs_head = externs_head->next;
    }

    fclose(write_file);
}

/**
Creates the .ent file listing entry labels.
It writes each entry label name and its address in base-4 format.
*/
void create_ent_file(char *ent_filename,Label *labels)
{
    FILE *write_file = fopen(ent_filename, "w");
    char* temp_str;

    while (labels != NULL)
    {
        if (labels->type == ENTRY_LBL)
        {
            fputs(labels->name, write_file);
            fputs("\t", write_file);
            temp_str = convertToBase4Custom_NoPadding((int)labels->addr);
            fputs(temp_str, write_file);
            fputs("\n", write_file);
            free(temp_str);
        }
        labels = labels->next;
    }
    fclose(write_file);
}

/**
Generates filenames for the output files (.obj, .ext, .ent)
based on the input filename by replacing the extension.
Allocates memory for each filename string.
*/
void create_filenames(const char* input_filename,
                      char** obj_filename,
                      char** ext_filename,
                      char** ent_filename)
{
    char* dot_position;
    ptrdiff_t base_length;
    char* base_name;

    /* Find last dot in filename */
    dot_position = strrchr(input_filename, '.');

    /* Calculate length of base name */
    base_length = dot_position - input_filename;

    /* Allocate base_name dynamically */
    base_name = (char*) malloc((size_t)base_length + 1); /* +1 for '\0' */
    if (!base_name) {
        /* Optional: handle allocation failure */
        return;
    }

    /* Copy base name (without extension) */
    strncpy(base_name, input_filename, (size_t)base_length);
    base_name[base_length] = '\0';

    /* Create the filenames using dynamic allocation */
    *obj_filename = combine_extension(base_name, ".obj");
    *ext_filename = combine_extension(base_name, ".ext");
    *ent_filename = combine_extension(base_name, ".ent");

    /* Free temporary buffer */
    free(base_name);
}

/**
Converts an integer number into a custom base-4 string representation
with fixed width, using characters 'a' to 'd' for digits 0-3.
Returns a dynamically allocated string.
*/
char* convertToBase4Custom(int num, int width)
{
    char mapping[] = "abcd"; /* mapping: 0->a, 1->b, 2->c, 3->d */
    int i;
    int temp_num = num;
    char* result;

    /* Allocate memory for result string */
    result = (char*)malloc((width + 1) * sizeof(char));
    if (result == NULL) {
        return NULL; /* allocation failed */
    }

    /* Fill string with 'a' characters (zeros) */
    memset(result, 'a', width);
    result[width] = '\0'; /* string terminator */

    /* Convert to base 4 from end to beginning */
    i = width - 1;
    while (temp_num > 0 && i >= 0) {
        result[i] = mapping[temp_num % 4];
        temp_num /= 4;
        i--;
    }

    return result;
}

/**
Converts an integer number into a custom base-4 string representation
without fixed width or padding, using characters 'a' to 'd' for digits 0-3.
Returns a dynamically allocated string.
*/
char* convertToBase4Custom_NoPadding(int num)
{
    char mapping[] = "abcd";
    int digits = 0, temp = num;
    char* result;
    int i;

    /* Special case for 0 */
    if (num == 0) {
        result = (char*)malloc(2);
        if (result) {
            result[0] = 'a';
            result[1] = '\0';
        }
        return result;
    }

    /* Count how many digits are needed in base 4 */
    while (temp > 0) {
        digits++;
        temp /= 4;
    }

    /* Allocate result string */
    result = (char*)malloc(digits + 1);
    if (!result) return NULL;

    /* Fill the string from end to start */
    result[digits] = '\0';
    for (i = digits - 1; i >= 0; i--) {
        result[i] = mapping[num % 4];
        num /= 4;
    }

    return result;
}
