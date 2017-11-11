
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
	j		___App

	.org	16
ecall_vec:
	nop
	uret

	.org  32
ebreak_vec:
	nop
	uret

	.org	48
timer_vec:
	li x4, 30
	wrtime	x4
	uret

.org	64
eint_vec:
	j 0
	j 0
	j 0
	j 0
	j 0
	j 0
	j 0
	j 0
	j 0
	j 0
	j 0
	j 0
	j 0
	j 0
	j 0
	j 0

.org 128
___App:
	li	x4, 0x7 #{eie, tie, gie}
	wruie	x4
	lui sp, %hi(_fstack)
	addi sp, sp, %lo(_fstack)
	jal main
	li	a7, 10
	ecall
