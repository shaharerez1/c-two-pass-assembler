THIS_IS_A_VERY_LONG_LINE__________________________________________________________

mcro .data
5, 6, 7
mcroend

mcro NAME extra words
mov r1, r2
mcroend trailing

mcro .string
"hello"
mcroend

mcro DUP
add r3, r4
mcroend

mcro DUP
sub r1, r2
mcroend

mcro entry bad
clr r5
mcroend

mcro .mat
1,2,3
mcroend

mcro .extern more
jmp LOOP
mcroend
