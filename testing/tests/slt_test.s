li  x1, 10
li  x2, 20
li  x3, -10

slt x4, x1, x2
slt x5, x2, x1
slti x6, x3, 0
slti x7, x2, 100
slt x8, x3, x1
sltu x9, x3, x1
sltiu x10, x3, -1

li a7, 10
ecall
