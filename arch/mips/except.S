/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/asm.h>
#include <asm/ptrace.h>
#include <asm/stackframe.h>

NESTED(except_generic, TF_SIZE, sp)
	.set push
	.set noat
	.set noreorder
	/*
	 * I saved everything we need into a trapframe, then deal with the
	 * exception in C code.
	 */
	j	handle_exception_entry
	nop
	.set pop
	END(except_generic)

handle_exception_entry:
	SAVE_ALL
	mfc0	k0, CP0_STATUS
	ori	k0, ST_EXCM
	xori	k0, ST_EXCM
	mtc0	k0, CP0_STATUS
	move	a0, sp
	jal	handle_exception
	RESTORE_ALL_AND_RET

