//#define		EXT_MUL

#define r_type_insn(_f7, _rs2, _rs1, _f3, _rd, _opc) \
.word (((_f7) << 25) | ((_rs2) << 20) | ((_rs1) << 15) | ((_f3) << 12) | ((_rd) << 7) | ((_opc) << 0))

#define cust_mul(_rd, _rs1, _rs2) \
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
	li	x4, 7
	wruie	x4
	lui sp, %hi(_fstack)
	add sp, sp, %lo(_fstack)
	jal main
	li	a7, 10
	ecall

#ifdef EXT_MUL
.global __mulsi3
__mulsi3:
	cust_mul(10, 10, 11)
	ret
#endif
