#define		EXT_MUL

#define r_type_insn(_f7, _rs2, _rs1, _f3, _rd, _opc) \
.word (((_f7) << 25) | ((_rs2) << 20) | ((_rs1) << 15) | ((_f3) << 12) | ((_rd) << 7) | ((_opc) << 0))

#define ext_mul(_rd, _rs1, _rs2) \
r_type_insn(0b0000000, _rs2, _rs1, 0b000, _rd, 0b1000111)

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

#ifdef EXT_MUL
.global __mulsi3
__mulsi3:
	ext_mul(10, 10, 11)
	ret
#endif
