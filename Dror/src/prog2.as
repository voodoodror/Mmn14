add r2,STR
LOOP: jmp W
inc K
bne L3
.entry LENGTH
.extern L3
MAIN: mov ***, W
STR: .string "abcdef"
sub r1, r4
.extern W
prn #-5
K: .data 2
LENGTH: .data 6,-9,15
.entry LOOP
END: stop
mov **,r3