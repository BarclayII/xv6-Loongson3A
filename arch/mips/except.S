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
	move	a0, sp
	jal	handle_exception
	RESTORE_ALL_AND_RET

