mcro MACRO1
    mov R1, R2
    add R3, R4
mcroend

mcro MACRO2
    cmp R5, R6
    jmp END_MACRO
mcroend

DATA: .data 5, 10, 15
STR: .string "HELLO"
M1: .mat [2][2] 1,2,3,4

.entry MISSING_LABEL
.entry UNUSED_ENTRY
.entry END
.extern GOOD_LABEL
.extern ANOTHER_EXTERN

MAIN:       mov r1, r2
            add r3, UNKNOWN_LABEL
	    sub r2, r1
            jmp NON_EXISTENT
            jsr GOOD_LABEL
LOOP:       inc M1[r1][r2]
            dec R2
            cmp r1, #-16
            bne LOOP
            jsr ANOTHER_EXTERN

END: 	       mov r4, r5
            add r1, r3
            stop

.extern MISSING_LABEL
.entry GOOD_LABEL
.entry NOT_WRITTEN
