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
#include <mm/mmap.h>
#include <sys/types.h>

int arch_map_page(arch_mm_t *arch_mm, addr_t vaddr, struct page *p,
    unsigned int perm)
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

int new_arch_mm(arch_mm_t *arch_mm)
{
	return 0;
}

void destroy_arch_mm(arch_mm_t *arch_mm)
{
}

struct page *arch_unmap_page(arch_mm_t *arch_mm, addr_t vaddr)
{
	return pgtable_remove(&(arch_mm->pgd), vaddr);
}

unsigned int page_perm(unsigned long vm_flags)
{
	return (vm_flags & VMA_WRITE) ? PTE_VALID|PTE_DIRTY : PTE_VALID;
}

