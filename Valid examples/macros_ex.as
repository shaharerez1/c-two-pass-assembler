MAIN: mov M1[r2][r7],LENGTH
 add r2,STR
LOOP: jmp END
prn #-5
	mcro a_mc
mov M1[r3][r3],r3
bne LOOP
	mcroend
sub r1, r4
inc K
	mcro b_mc
sub K,r1
rts
	mcroend
a_mc
b_mc
END: stop
STR: .string "abcdef"
LENGTH: .data 1,-5,17
K: .data -4
M1: .mat [2][2] 10,9,8,7 