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
	li			x4, 0
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
___App:
	li	x4, 0x7 #{eie, tie, gie}
	wruie	x4
	lui sp, %hi(_fstack)
	addi sp, sp, %lo(_fstack)
	jal main
	li	a7, 10
	ecall

main:
	li t0, 0x80000000		 #to lui
polling:
	lw s0, 1(t0)
	and t1, s0, 0x1
	beq t1, zero, polling
	jr ra
