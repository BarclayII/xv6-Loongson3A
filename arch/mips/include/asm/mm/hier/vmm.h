/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_MM_HIER_VMM_H
#define _ASM_MM_HIER_VMM_H

#include <sys/types.h>
#include <mm/pgtable.h>
#include <asm/mm/page.h>
#include <asm/mm/pgtable.h>
#include <asm/mm/hier/pgdir.h>

typedef struct arch_mm_struct {
	pgd_t	pgd;		/* PGD */
} arch_mm_t;

/* Page table hierarchy path retrieved by pgtable_get() function */
struct pagedesc {
	/* NOTE: these directory members are kernel virtual addresses, not
	 *       PFNs. */
	pgd_t		pgd;
#ifndef CONFIG_3LEVEL_PT
	pud_t		pud;
#endif
	pmd_t		pmd;
	pte_t		pte;
	unsigned short	pgx;
#ifndef CONFIG_3LEVEL_PT
	unsigned short	pux;
#endif
	unsigned short	pmx;
	unsigned short	ptx;
};

void arch_mm_init(void);
void dump_pagedesc(addr_t vaddr, struct pagedesc *pdesc);

#endif
