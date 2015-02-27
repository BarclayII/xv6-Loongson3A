/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <mm/vmm.h>
#include <asm/mm/pgtable.h>
#include <asm/mm/tlb.h>
#include <asm/mipsregs.h>

static void init_tlb(void)
{
	tlb_flush_all();
}

void arch_mm_init(void)
{
	pgtable_bootstrap();
	init_tlb();
}
