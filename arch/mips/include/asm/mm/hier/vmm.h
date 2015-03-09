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

#define VMA_FREE	0x01
#define VMA_READ	0x02
#define VMA_WRITE	0x04
#define VMA_EXEC	0x08
#define VMA_VALID	0x10
#define VMA_DIRTY	0x20

void arch_mm_init(void);
void dump_pagedesc(addr_t vaddr, struct pagedesc *pdesc);

static inline void new_arch_mm(arch_mm_t *arch_mm)
{
}

static inline void destroy_arch_mm(arch_mm_t *arch_mm)
{
}

static inline int arch_map_page(arch_mm_t *arch_mm, addr_t vaddr,
    struct page *p, unsigned int perm)
{
	int retcode;
	struct page *replace = NULL;
	retcode = pgtable_insert(&(arch_mm->pgd), vaddr, p, perm, true,
	    &replace);
	if (retcode != 0)
		return retcode;
	if (replace != NULL)
		pgfree(replace);
	return 0;
}

static inline struct page *arch_unmap_page(arch_mm_t *arch_mm, addr_t vaddr)
{
	return pgtable_remove(&(arch_mm->pgd), vaddr);
}

static inline unsigned int page_perm(unsigned long vm_flags)
{
	return (vm_flags & VMA_WRITE) ? PTE_VALID|PTE_DIRTY : PTE_VALID;
}

#endif
