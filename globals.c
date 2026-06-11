#include "globals.h"

/* The following charts implement a boolean flag matrix
 * each row represents the operation's argument valid addressing methods.
 * The row's indexes correspond with the opcode of each operations
 * divided into 2 separate matrices to avoid another data structure
*/


/* SOURCE ARGUMENT */
const int valid_src_addressing[NUM_OF_OPERATIONS][NUM_OF_ADDR_TYPES] =
{
    {1,1,1,1},  /* 0: mov */
    {1,1,1,1},  /* 1: cmp */
    {1,1,1,1},  /* 2: add */
    {1,1,1,1},  /* 3: sub */
    {0,0,0,0},  /* 4: not */
    {0,0,0,0},  /* 5: clr */
    {0,1,1,0},  /* 6: lea */
    {0,0,0,0},  /* 7: inc */
    {0,0,0,0},  /* 8: dec */
    {0,0,0,0},  /* 9: jmp */
    {0,0,0,0},  /* 10: bne */
    {0,0,0,0},  /* 11: red */
    {0,0,0,0},  /* 12: prn */
    {0,0,0,0},  /* 13: jsr */
    {0,0,0,0},  /* 14: rts */
    {0,0,0,0}   /* 15: stop */
};

/* DEST ARGUMENT */
const int valid_dst_addressing[NUM_OF_OPERATIONS][NUM_OF_ADDR_TYPES] =
{
    {0,1,1,1},  /* 0: mov */
    {1,1,1,1},  /* 1: cmp */
    {0,1,1,1},  /* 2: add */
    {0,1,1,1},  /* 3: sub */
    {0,1,1,1},  /* 4: not */
    {0,1,1,1},  /* 5: clr */
    {0,1,1,1},  /* 6: lea */
    {0,1,1,1},  /* 7: inc */
    {0,1,1,1},  /* 8: dec */
    {0,1,1,1},  /* 9: jmp */
    {0,1,1,1},  /* 10: bne */
    {0,1,1,1},  /* 11: red */
    {1,1,1,1},  /* 12: prn */
    {0,1,1,1},  /* 13: jsr */
    {0,0,0,0},  /* 14: rts */
    {0,0,0,0}   /* 15: stop */
};
