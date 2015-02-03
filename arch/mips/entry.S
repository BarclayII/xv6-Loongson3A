/*
 * Copyright (C) 1994, 1995 Waldorf Electronics
 * Written by Ralf Baechle and Andreas Busse
 * Copyright (C) 1994 - 99, 2003, 06 Ralf Baechle
 * Copyright (C) 1996 Paul M. Antoine
 * Modified for DECStation and hence R3000 support by Paul M. Antoine
 * Further modifications by David S. Miller and Harald Koerfgen
 * Copyright (C) 1999 Silicon Graphics, Inc.
 * Kevin Kissell, kevink@mips.com and Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 2000 MIPS Technologies, Inc.  All rights reserved.
 *
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

#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>

#define RAMSIZE	0x10000000

LEAF(_start)
	
	.set	mips64
	.set	noreorder

	mfc0	t0, CP0_STATUS
	or	t0, ST_KX|ST_CU0
	and	t0, ~(ST_IE)
	mtc0	t0, CP0_STATUS

        /* Disable watch exception. */
        mtc0    zero, CP0_WATCHLO
        mtc0    zero, CP0_WATCHHI

	/* Load PMON arguments into external firmware argument variables */
	sd	a0, fw_arg0
	sd	a1, fw_arg1
	sd	a2, fw_arg2
	sd	a3, fw_arg3

	/* set up stack */
	li	sp, 0xffffffff802c0000 + RAMSIZE - 16
	
	/* jump to main */
	jal	main
			
loop:
	j	loop
	nop
END(_start)

