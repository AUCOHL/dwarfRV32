li  x1, 50
li  x2, 32
li  x3, 2
sll  x4, x1, x3
slli x5, x2, 2
Srl  x6, x1, x3
srli x7, x2, 4
li x8, 1
slli x12, x12, 31
sra x9, x12, x3
srai x10, x12, 5
sra x11, x12, x3
li x12, -20
srai x13, x12, 2
sra x14, x12, x3
srai x15, x12, 2


li a7, 10
ecall
