/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/trap.h>
#include <asm/mipsregs.h>
#include <time.h>

int handle_clock(struct trapframe *tf)
{
	unsigned int cmp = read_c0_compare();
	/* Repeatedly write into CP0_COMPARE until it is ahead, but
	 * not too far from CP0_COUNT */
	do {
		cmp += 0x10000000;
		write_c0_compare(cmp);
	} while (cmp - read_c0_count() > 0x10000000);
	generic_cpu_tick();
	return 0;
}
