#ifndef DEFINED_STRINGS_H
#define DEFINED_STRINGS_H

/* Define operations names */
#define MOV_STR "mov"
#define CMP_STR "cmp"
#define ADD_STR "add"
#define SUB_STR "sub"
#define NOT_STR "not"
#define CLR_STR "clr"
#define LEA_STR "lea"
#define INC_STR "inc"
#define DEC_STR "dec"
#define JMP_STR "jmp"
#define BNE_STR "bne"
#define RED_STR "red"
#define PRN_STR "prn"
#define JSR_STR "jsr"
#define RTS_STR "rts"
#define STOP_STR "stop"

/* Define instructions names */
#define DATA_STR ".data"
#define STRING_STR ".string"
#define MAT_STR ".mat"
#define ENTRY_STR ".entry"
#define EXTERN_STR ".extern"

/* Define macros safe words */
#define MACRO_START_STR "mcro"
#define MACRO_END_STR "mcroend"

/* Define operation struct */
typedef struct {
    const char *name;
    int num_args;
} Operation;

/* Extern declaration for the arrays of operations and instructions */
extern const Operation operations[];
extern const int num_operations;

extern const char * const instruction_names[];
extern const int num_instructions;

#endif
