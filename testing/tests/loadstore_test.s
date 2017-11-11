li  x1, 50
li  x2, 32
li  x3, 3
li x4, 200

sw  x1, 0(x4)
sw  x2, 4(x4)

lw  x5, 0(x4)
lw  x6, 4(x4)

sh  x1, 8(x4)
sh  x2, 12(x4)

lh  x7, 8(x4)
lh  x8, 12(x4)

li  x9, 0xf
Slli x9, x9, 12

sh  x9, 16(x4)
lhu  x10, 16(x4)
lh  x11, 16(x4)

li x12, 0xff
sb  x1, 20(x4)
sb  x2, 24(x4)
sb  x12, 28(x4)

lb  x13, 20(x4)
lb  x14, 24(x4)
lbu  x15, 28(x4)
lb x15, 28(x4)

li a7, 10
ecall
