li x8, 0x80f
li x9, 0x8ff
bltu x8, x9, case1
j exit

case1:
li x8, 123
exit:
    li a7, 10 #...and 10 is the program termination service number!
    ecall
