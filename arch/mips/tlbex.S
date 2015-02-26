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
#include <asm/addrspace.h>
#include <asm/ptrace.h>
#include <asm/stackframe.h>
#include <asm/mm/page.h>
#include <asm/mm/pgtable.h>

LEAF(except_tlb)
	.set	push
	.set	noat
	/*
	 * Jump to a longer routine as I'm not sure whether I could really
	 * finish refilling within 32 instructions (128 bytes).
	 * ......
	 * I surely couldn't.
	 */
	 j	xtlb_refill
	 nop
	.set	pop
END(except_tlb)

xtlb_refill:
	.set	push
	.set	noat
	.set	mips64r2
	dmfc0	k0, CP0_ENTRYHI
	andi	k1, k0, ENTHI_ASID_MASK		/* k1 = ASID */
	dla	k0, online_hpt
	dsll	k1, 3
	daddu	k1, k0
	dmfc0	k0, CP0_BADVADDR
	beqz	k1, error
	ld	k1, (k1)			/* k1 = PGD */
	dsrl	k0, PGD_OFFSET
	andi	k0, PDE_MASK			/* k0 = PGX */
	dsll	k0, 3
	daddu	k1, k0				/* k1 = &PGD[PGX] */
	dmfc0	k0, CP0_BADVADDR
	beqz	k1, error
	ld	k1, (k1)			/* k1 = PGD[PGX] = PUD */
	dsrl	k0, PUD_OFFSET
	andi	k0, PDE_MASK			/* k0 = PUX */
	dsll	k0, 3
	daddu	k1, k0				/* k1 = &PUD[PUX] */
	dmfc0	k0, CP0_BADVADDR
	beqz	k1, error
	ld	k1, (k1)			/* k1 = PUD[PUX] = PMD */
	dsrl	k0, PMD_OFFSET
	andi	k0, PDE_MASK			/* k0 = PMX */
	dsll	k0, 3
	daddu	k1, k0				/* k1 = &PMD[PMX] */
	dmfc0	k0, CP0_BADVADDR
	beqz	k1, error
	ld	k1, (k1)			/* k1 = PMD[PMX] = PTE */
	dsrl	k0, PTE_OFFSET
	andi	k0, PDE_MASK - 1		/* k0 = EVEN(PTX) */
	dsll	k0, 3
	daddu	k1, k0				/* k1 = &PTE[EVEN(PTX)] */
	daddu	k0, k1, 8			/* k0 = &PTE[ODD(PTX)] */
	beqz	k1, error
	ld	k1, (k1)			/* k1 = PTE[EVEN(PTX)] */
	beqz	k0, error
	ld	k0, (k0)			/* k0 = PTE[ODD(PTX)] */
	dsrl	k1, PTE_SOFT_SHIFT
	dsrl	k0, PTE_SOFT_SHIFT
	dmtc0	k1, CP0_ENTRYLO0
	dmtc0	k0, CP0_ENTRYLO1
	tlbwr
	j	success
error:
	/* Jumps into failsafe generic handler */
	j	except_generic
	nop
success:
	eret
	.set	pop
