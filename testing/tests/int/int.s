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
  nop
  uret


.org 80
___App:
  li	    x4, 7
  wruie	  x4
  li      x2, 100
  li      x3, 100
  li      x2, 200
  addi      x2, x2, 400
  addi      x2, x2, 600
  addi      x2, x2, 700
  ebreak
  Addi    x2, x2, 100
  ecall
