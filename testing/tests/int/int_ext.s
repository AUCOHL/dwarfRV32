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
  li x6, 111
  uret


.org 80
___App:
  li	    x4, 5
  wruie	  x4
  li x9, 111
infloop:
  li x7, 123
  beq x9, x6, exit
  jal x5, infloop
 
exit:
  li x7, 10
  ecall
