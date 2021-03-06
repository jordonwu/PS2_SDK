/*
# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
#
# $Id: sysmem.s 577 2004-09-14 14:41:46Z pixel $
# Memory Function import list.
*/

	.text
	.set	noreorder


/* ############################### SYSMEM STUB ######## */
	.local	sysmem_stub
sysmem_stub:
	.word	0x41e00000
	.word	0
	.word	0x00000101
	.ascii	"sysmem\0\0"
	.align	2

	.globl	AllocSysMemory			# 004
AllocSysMemory:
	j	$31
	li	$0, 0x04

	.globl	FreeSysMemory			# 005
FreeSysMemory:
	j	$31
	li	$0, 0x05

	.globl 	QueryMemSize			# 006
QueryMemSize:
	j	$31
	li	$0, 0x06

	.globl	QueryMaxFreeMemSize		# 007
QueryMaxFreeMemSize:
	j	$31
	li	$0, 0x07

	.globl	QueryTotalFreeMemSize		# 008
QueryTotalFreeMemSize:
	j	$31
	li	$0, 0x08

	.globl	QueryBlockTopAddress		# 009
QueryBlockTopAddress:
	j	$31
	li	$0, 0x09

	.globl	QueryBlockSize			# 00A
QueryBlockSize:
	j	$31
	li	$0, 0x0A

	.globl	Kprintf				# 0x0E
Kprintf:
	j	$31
	li	$0, 0x0E

	.globl	SetKprintf			# 0x0F
SetKprintf:
	j	$31
	li	$0, 0x0F

	.word	0
	.word	0


