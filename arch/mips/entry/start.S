/*
 * Copyright (C) 2001 MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 *
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <regdef.h>
#include <cp0regdef.h>
#include <asm.h>

#define RAMSIZE	0x100000

LEAF(_start)
	
	.set	mips3
	.set	noreorder

	/* Disable interrupts */
	mfc0	t0, CP0_STATUS
	or	t0, ST_KX|ST_CU0
	and	t0, ~(ST_IE)
	mtc0	t0, CP0_STATUS

        /* Disable watch exception. */
        mtc0    zero, CP0_WATCHLO
        mtc0    zero, CP0_WATCHHI

	/* set up stack */
	li	sp, 0xffffffff802c0000 + RAMSIZE - 16
	
	/* jump to main */
	jal	main
			
loop:
	j	loop
	nop
END(_start)

