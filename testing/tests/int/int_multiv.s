# this test cannot be handled by rv32sim

.macro wrcycle reg
csrw	cycle, \reg
.endm

.macro wrtime reg
csrw	time, \reg
.endm

.macro wruie reg
csrw	0x4, \reg
.endm


.section .text
.global _start

.org 0
_start:
wruie		x0
li			x4, 30
wrtime	x4
j       ___App

.org	16
ecall_vec:
nop
uret
j		 ecall_vec

.org  32
ebreak_vec:
li x6, 111
uret


.org	48
timer_vec:
li x4, 30
wrtime	x4
uret


.org	64
eint_vec:
j IRQ0
j IRQ1
j IRQ2
j IRQ3
j IRQ4
j IRQ5
j IRQ6
j IRQ7
j IRQ8
j IRQ9
j IRQ10
j IRQ11
j IRQ12
j IRQ13
j IRQ14
j IRQ15


.org 128

IRQ0:
 li x6, 1
 uret

___App:
li	    x4, 5
wruie	  x4
li x6, 16
li x9, 16
infloop:
li x7, 123
blt x6, x9, exit
jal infloop

exit:
li x7, 10
ecall

