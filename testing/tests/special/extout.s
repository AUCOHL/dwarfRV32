
lui a0, 0x80000
li  x1, 'h'
li  x2, 'e'
li  x3, 'l'
li  x4, 'l'
li  x5, 'o'
li  x6, '\n'

sb x1, 0(a0)
sb x2, 0(a0)
sb x3, 0(a0)
sb x4, 0(a0)
sb x5, 0(a0)
sb x6, 0(a0)


li a7, 10
ecall
