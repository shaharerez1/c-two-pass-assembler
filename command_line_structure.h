
#ifndef COMMAND_LINE_STRUCTURE_H
#define COMMAND_LINE_STRUCTURE_H
#include "globals.h"

typedef enum
{
    CODE = 0,
    DATA = 1,
    ERROR = -1
} line_type; /* Type of command line - used for identifying ways of code behavior  */

typedef enum
{
    DATA_INST = 0,
    STRING_INST,
    MAT_INST,
    ENTRY_INST,
    EXTERN_INST
} inst_type; /* type of instruction - note : .data / .extern are not saved in
                the main commands array, but in the label chart  */

typedef enum
{
    MOV_OP = 0,
    CMP_OP,
    ADD_OP,
    SUB_OP,
    NOT_OP,
    CLR_OP,
    LEA_OP,
    INC_OP,
    DEC_OP,
    JMP_OP,
    BNE_OP,
    RED_OP,
    PRN_OP,
    JSR_OP,
    RTS_OP,
    STOP_OP
} op_type; /* Opcodes matching the given order */

typedef enum
{
    IMDT = 0,
    DIRECT,
    MTRX,
    REG
} addr_type; /* Types of addressing methods */

typedef struct /* Struct representing the encoding of an OPERATION argument */
{
    unsigned int ARE : 2;

    union               /* Two ways to store the remaining 8 bits
                        1 - All 8 bits together - representing numeric value or address
                        2 - Seperated 8 bits, distinction between the right and left 4 bits.
                      */
    {
        unsigned int val_8bit : 8;
        struct
        {
            int right_4bit : 4; /* Possible to use int, because all inserted values will hold 0-7 */
            int left_4bit : 4;
        } sep_data; /* for arguments such as registers, using only 4 bits */
    } data;

} op_arg;

typedef struct /* Main database to store command line encoding data */
{
    line_type type; /* Command line's association */
    unsigned int addr; /* Decimal address - using ICF/DCF */
    int req_lines; /* required number of encoded lines for this command */

    union
    {
        struct
        {
            inst_type type; /* enum for type of instruction */
            int array[MAX_LINE_LENGTH]; /* array to store data for all data instructions -> .data, .mat, .string */
        } instruction;

        struct
        {
            op_type type; /* type of operation */
            addr_type source_addr, dest_addr; /* addressing method for each argument */
            op_arg source_arg, dest_arg; /* data of actual arguments - maximum 2 */

            /* next values will be in use only if one of the line's arguments is a matrix */
            op_arg mtrx_regs_src,mtrx_regs_dest ; /* Values to store registers info of source/dest matrix argument */
        } operation;

    } cmd_set; /* Written command must be operation/instruction */
} cmd_line;

#endif
