/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/cpu.h>
#include <asm/mm/tlb.h>
#include <asm/mm/page.h>
#include <asm/mipsregs.h>
#include <asm/cp0regdef.h>
#include <sys/types.h>
#include <stddef.h>
#include <sync.h>

void tlb_flush_all(void)
{
	int i;
	intr_flag_t flags;

	/* TODO: acquire per-CPU TLB lock */
	ENTER_CRITICAL_SECTION(NULL, flags);
	for (i = 0; i < current_cpu_data.tlbsize; ++i) {
		write_c0_index(i);
		write_c0_entryhi(ENTRYHI_DUMMY(i));
		write_c0_entrylo0(0);
		write_c0_entrylo1(0);
		tlbwi();
	}
	EXIT_CRITICAL_SECTION(NULL, flags);
}

void tlb_remove(ptr_t vaddr)
{
	intr_flag_t flags;

	/* TODO: acquire per-CPU TLB lock */
	ENTER_CRITICAL_SECTION(NULL, flags);
	/* get ASID */
	unsigned long asid = read_c0_entryhi() & ENTHI_ASID_MASK;
	/* write even VPN and ASID into ENTRYHI and query TLB */
	write_c0_entryhi(asid | ((vaddr >> (PGSHIFT + 1)) << (PGSHIFT + 1)));
	tlbp();

	/* check if the entry exists */
	unsigned int index = read_c0_index();
	unsigned long entrylo1 = read_c0_entrylo1();
	unsigned long entrylo0 = read_c0_entrylo0();

	if (index >= 0) {
		tlbr();
		/* remove requested virtual page first */
		if (vaddr & PGSIZE) {
			/* odd virtual page */
			write_c0_entrylo1(0);
			entrylo1 = 0;
		} else {
			/* even virtual page */
			write_c0_entrylo0(0);
			entrylo0 = 0;
		}
		/* If both entries are cleared, clean ENTRYHI as well.
		 * NOTE:
		 * ALWAYS clean ENTRYHI with ENTRYHI_DUMMY() macro.
		 * See arch/mips/include/asm/mm/tlbops.h for more comments. */
		if (!(entrylo1 || entrylo0))
			write_c0_entryhi(ENTRYHI_DUMMY(index));

		tlbwi();
	}

	EXIT_CRITICAL_SECTION(NULL, flags);
}
