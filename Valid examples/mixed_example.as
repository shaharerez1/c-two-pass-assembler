mcro PRINT_X  
    prn r1  
    prn r2  
mcroend  

mcro DOUBLE_ADD  
    add r1, r2  
    add r2, r3  
mcroend  

.extern EXT_A  
.extern EXT_FUNC  
.entry MAIN  
.entry FINISH  

MAIN:       mov r2, EXT_A  
            jsr EXT_FUNC  
            PRINT_X  
            DOUBLE_ADD  
            jmp LOOP  

M1:         .mat [2][2] 1,2,3,4  
S1:         .string "HI"  
D1:         .data 5, -2  

LOOP:       sub r2, r1  
            bne FINISH  

FINISH:     prn r3  
            stop  
