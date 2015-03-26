/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <config.h>
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
	/*
	 * Goals:
	 * 1. Refill the TLB if page table maintains a valid entry.
	 * 2. Turn to generic handler for swapping if such an entry does not
	 *    exist.  This requires the handler to maintain ENTRYHI and
	 *    BADVADDR contents while switching context, which is achieved
	 *    by a series of (inefficient) checks.
	 */
	.set	push
	.set	noat
	.set	noreorder
	.set	mips64r2
	dmfc0	k0, CP0_ENTRYHI
	andi	k1, k0, ENTHI_ASID_MASK		/* k1 = ASID */
	dla	k0, asid_task_set
	dsll	k1, 3
	daddu	k1, k0
	dmfc0	k0, CP0_BADVADDR
	/*
	 * Branch to generic handler if an entry is zero.
	 * NOTE:
	 * Looks like a memory load should not directly precede a dependent
	 * branch.  Is it a hazard or something MIPS specific?
	 */
	ld	k1, (k1)			/* k1 = PGD */
	beqz	k1, 1f
	dsrl	k0, PGD_OFFSET
	andi	k0, PDE_MASK			/* k0 = PGX */
	dsll	k0, 3
	daddu	k1, k0				/* k1 = &PGD[PGX] */
	dmfc0	k0, CP0_BADVADDR
	ld	k1, (k1)			/* k1 = PGD[PGX] = PUD/PMD */
	beqz	k1, 1f
#ifndef CONFIG_3LEVEL_PT
	dsrl	k0, PUD_OFFSET
	andi	k0, PDE_MASK			/* k0 = PUX */
	dsll	k0, 3
	daddu	k1, k0				/* k1 = &PUD[PUX] */
	dmfc0	k0, CP0_BADVADDR
	ld	k1, (k1)			/* k1 = PUD[PUX] = PMD */
	beqz	k1, 1f
#endif
	dsrl	k0, PMD_OFFSET
	andi	k0, PDE_MASK			/* k0 = PMX */
	dsll	k0, 3
	daddu	k1, k0				/* k1 = &PMD[PMX] */
	dmfc0	k0, CP0_BADVADDR
	ld	k1, (k1)			/* k1 = PMD[PMX] = PTE */
	beqz	k1, 1f
	dsrl	k0, PTE_OFFSET
	andi	k0, PDE_MASK - 1		/* k0 = EVEN(PTX) */
	dsll	k0, 3
	daddu	k1, k0				/* k1 = &PTE[EVEN(PTX)] */
	daddu	k0, k1, 8			/* k0 = &PTE[ODD(PTX)] */
	ld	k1, (k1)			/* k1 = PTE[EVEN(PTX)] */
	bnez	k1, 2f
	ld	k0, (k0)			/* k0 = PTE[ODD(PTX)] */
	bnez	k0, 2f
	nop
1:
	/* Jumps into failsafe generic handler.
	 * This happens if a mapping from that virtual address is not
	 * established (i.e. page faults). */
	j	except_generic
	nop
2:
	/* Successfully found at least one mapped entry.  Write both. */
	/* Clear the P bit first */
	ori	k1, PTE_PHYS
	ori	k0, PTE_PHYS
	xori	k1, PTE_PHYS
	xori	k0, PTE_PHYS
	/* Right rotate */
	drotr	k1, PTE_SOFT_SHIFT
	drotr	k0, PTE_SOFT_SHIFT
	dmtc0	k1, CP0_ENTRYLO0
	dmtc0	k0, CP0_ENTRYLO1
	tlbwr
	eret
	.set	pop
