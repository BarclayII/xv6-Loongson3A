/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _MM_HIER_VMM_H
#define _MM_HIER_VMM_H

#include <asm/mm/page.h>
#include <mm/hier/pgdir.h>

struct mm_struct;

typedef struct vm_area_struct {
	ptr_t			start;	/* starting address (inclusive) */
	size_t			size;	/* area size (in bytes) */
	unsigned long		flags;	/* various flags... */
#define VMA_FREE	0x01
#define VMA_READ	0x02
#define VMA_WRITE	0x04
#define VMA_EXEC	0x08
#define VMA_VALID	0x10
#define VMA_DIRTY	0x20
	struct mm_struct 	*mm;	/* memory management structure */
	list_node_t		node;	/* list node */
} vm_area_t;

typedef struct mm_struct {
	pgd_t			pgd;		/* PGD */
	unsigned short		asid;
	vm_area_t		*vma_head;
} mm_t;

/*
 * Each directory stores a list of physical frame numbers corresponding to
 * pages (or next-level page directories).
 */

/* Page table hierarchy path retrieved by pgtable_get() function */
struct pagedesc {
	/* NOTE: these directory members are kernel virtual addresses, not
	 *       PFNs. */
	pgd_t		pgd;
	pud_t		pud;
	pmd_t		pmd;
	pte_t		pte;
	unsigned short	pgx;
	unsigned short	pux;
	unsigned short	pmx;
	unsigned short	ptx;
};

extern mm_t kern_high_mm;		/* High memory manager */
extern mm_t kern_low_mm;		/* Low memory manager */
#define kern_mm	kern_high_mm

#endif
