/*
  _____     ___ ____ 
   ____|   |    ____|      PSX2 OpenSource Project
  |     ___|   |____       (C)2002, David Ryan ( oobles@hotmail.com ) 
  ------------------------------------------------------------------------
  ps2dexp.s		   PS2DEBUG EXPORT FUNCTIONS.
*/



	.text
	.set	noreorder
	.global func_dec
   	.global iop_module

        .extern lock_printf
 	.extern lock_set_out
        .extern start

iop_module:
	.word	0x41c00000
	.word	0
	.word	0x00000101
	.ascii	"ps2debug"

	.align	2

func_dec:
	.word   _start
        .word   do_nothing
	.word   do_nothing
	.word   do_nothing
        .word   lock_printf 
	.word	lock_set_out
	.word   0

do_nothing:
	.word   0x03e00008
	.word   0


