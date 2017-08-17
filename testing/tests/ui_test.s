nop
li x0, 2
li  x1, 40
li  x2, 32
li  x3, 3
lui x4, 0x444

auipc x5, 0
auipc x6, 10

lui     x7,0x10
addi    x8,x7,-1348

Add x9, x8, x2
Sub x10, x9, x3
Addi x11, x10, 20

li a7, 10
ecall
