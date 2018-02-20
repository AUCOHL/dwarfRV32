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
  nop
  uret


.org	48
timer_vec:
  li x4, 0
  wrtime x4
  uret


.org	64
eint_vec:
  nop
  uret


.org 80
___App:
  li		x4, 0xfffff
  li 		x3, 101
  wruie		x4

  csrrci		x5, 0x4, 0xf
  csrrw		    x6, 0x4, x3
  csrr     	    x7, 0x4

  ecall
