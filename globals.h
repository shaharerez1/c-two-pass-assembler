#ifndef GLOBALS_H
#define GLOBALS_H

#define TRUE 1
#define FALSE 0

#define ABSOLUTE 0
#define EXTERNAL 1
#define RELOCATABLE 2

#define EMPTY 0

#define STARTING_INDEX 100 /* As instructed */

#define MAX_MACHINE_LINES 120

#define NUM_OF_OPERATIONS 16

#define MAX_VAL_10BIT 512 /* Each .data/.mat value is represented by 10 bits, with 2's complement method its 1024/2 */
#define MAX_VAL_8BIT 128 /* To store immediate values */

#define NUM_OF_ADDR_TYPES 4

#define MAX_CMD_ARGS 2

#define MAX_LINE_LENGTH 82 /* As instructed + \n + null terminator */

#define MAX_LABEL_LENGTH 31 /* As instructed + null terminator */

#define _10BIT 10
#define _8BIT 8

extern const int valid_src_addressing[NUM_OF_OPERATIONS][NUM_OF_ADDR_TYPES];

extern const int valid_dst_addressing[NUM_OF_OPERATIONS][NUM_OF_ADDR_TYPES];

#endif
