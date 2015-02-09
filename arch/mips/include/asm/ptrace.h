/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_PTRACE_H
#define _ASM_PTRACE_H

#include <asm/asm_off.h>

struct trapframe {
	/* General Purpose Registers 
	 * NOTE: Normally k0 and k1 need not be saved. */
	unsigned long long gpr[32];
	/* Special registers LO/HI */
	unsigned long long lo, hi;
	/* Necessary CP0 Registers */
	unsigned long long cp0_status;
	unsigned long long cp0_cause;
	unsigned long long cp0_badvaddr;
	unsigned long long cp0_epc;
};


#endif
