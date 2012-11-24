/*
  _____     ___ ____ 
   ____|   |    ____|      PSX2 OpenSource Project
  |     ___|   |____       (C)2002, David Ryan (Oobles@hotmail.com)
  ------------------------------------------------------------------------
  ps2debug.s		   ps2debug Import Library
*/

	.text
	.set	noreorder


	.local	ps2debug_stub
ps2debug_stub:
	.word	0x41e00000
	.word	0
	.word	0x00000101
	.ascii	"ps2debug"
	.align	2

	.globl	lock_printf			# 004
    	.globl	lock_set_out			# 005

lock_printf:
	j	$31
	li	$0, 4
lock_set_out:
	j	$31
	li	$0, 5

	.word	0
	.word	0


