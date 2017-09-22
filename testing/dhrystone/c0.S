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
	nop
	uret

	.org 80
___App:
	lui a0,0x80000
	addi a1,zero,'S'
	addi a2,zero,'T'
	addi a3,zero,'A'
	addi a4,zero,'R'
	addi a5,zero,'\n'
	sw a1,0(a0)
	sw a2,0(a0)
	sw a3,0(a0)
	sw a4,0(a0)
	sw a2,0(a0)
	sw a5,0(a0)

	li	x4, 0x0 #{eie, tie, gie}
	wruie	x4
	lui sp, %hi(_fstack)
	addi sp, sp, %lo(_fstack)


	/* execute some insns for "make timing" */
	lui a0,0
	auipc a0,0
	slli a0,a0,0
	slli a0,a0,31
	addi a1,zero,0
	sll a0,a0,a1
	addi a1,zero,31
	sll a0,a0,a1

	/* jump to main C code */
	jal main

	lui a0,0x80000
	addi a1,zero,'D'
	addi a2,zero,'O'
	addi a3,zero,'N'
	addi a4,zero,'E'
	addi a5,zero,'\n'
	sw a1,0(a0)
	sw a2,0(a0)
	sw a3,0(a0)
	sw a4,0(a0)
	sw a5,0(a0)

	li	a7, 10
	ecall
