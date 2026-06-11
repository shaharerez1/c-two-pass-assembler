#include "defined_strings.h"

/* Define operations array with number of arguments per operation */
const Operation operations[] = {
    { MOV_STR, 2 },   /* 0 */
    { CMP_STR, 2 },   /* 1 */
    { ADD_STR, 2 },   /* 2 */
    { SUB_STR, 2 },   /* 3 */
    { NOT_STR, 1 },   /* 4 */
    { CLR_STR, 1 },   /* 5 */
    { LEA_STR, 2 },   /* 6 */
    { INC_STR, 1 },   /* 7 */
    { DEC_STR, 1 },   /* 8 */
    { JMP_STR, 1 },   /* 9 */
    { BNE_STR, 1 },   /* 10 */
    { RED_STR, 1 },   /* 11 */
    { PRN_STR, 1 },   /* 12 */
    { JSR_STR, 1 },   /* 13 */
    { RTS_STR, 0 },   /* 14 */
    { STOP_STR, 0 }   /* 15 */
};

const int num_operations = sizeof(operations) / sizeof(operations[0]);

/* Define instructions array - indexes are corresponding with ENUM inst_type ('command_line_structure.h') */
const char * const instruction_names[] = {
    DATA_STR,
    STRING_STR,
    MAT_STR,
    ENTRY_STR,
    EXTERN_STR
};

const int num_instructions = sizeof(instruction_names) / sizeof(instruction_names[0]);
