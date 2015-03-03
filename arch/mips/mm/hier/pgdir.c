/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/cache.h>
#include <asm/mm/pgtable.h>
#include <asm/mm/hier/pgdir.h>
#include <mm/mmap.h>
#include <string.h>
#include <printk.h>
#include <panic.h>
#include <assert.h>

/*
 * Allocate a new page directory object
 */
void pgdir_check(pgdir_t *pgdir)
{
	assert(pgdir->type == PGTYPE_PGDIR);
}

pgdir_t *pgdir_new(asid_t asid)
{
	struct page *p = pgalloc();
	memset((void *)PAGE_TO_KVADDR(p), 0, PGSIZE);
	
	p->type = PGTYPE_PGDIR;
	p->pgdir.entries = 0;
	p->pgdir.asid = asid;
	return (pgdir_t *)p;
}

void pgdir_delete(pgdir_t *pgdir)
{
	pgdir_check(pgdir);
	pgfree((struct page *)pgdir);
}

/*
 * Add a page @p into directory @pgdir with index @index.
 * Returns the virtual address of newly added page.
 * NOTE: This function does *not* modify anything in @p.  Increasing
 *       reference counter or things like that should be done outside.
 */
addr_t pgdir_add_entry(pgdir_t *pgdir, unsigned short index, struct page *p,
    unsigned int flags)
{
	pgdir_check(pgdir);
	addr_t *dir = (addr_t *)PAGE_TO_KVADDR(pgdir);
	unsigned long entry;
	if (dir[index]) {
		/* Shouldn't allow two different entries having same index */
		printk("pgdir = %016x\r\n", dir);
		printk("index = %d\r\n", index);
		printk("content = %016x\r\n", dir[index]);
		panic("Adding entry to the same position twice.\r\n");
	}

	++(pgdir->pgdir.entries);

	if (!(flags & PTE_PHYS))
		entry = PAGE_TO_KVADDR(p);
	else
		entry = PAGE_TO_PADDR(p) | (flags & PTE_LOWMASK);
	dir[index] = entry;

	return PAGE_TO_KVADDR(p);
}

/*
 * Allocate a new page directory, usually an intermediate one, into
 * the page directory.
 */
addr_t pgdir_add_pgdir(pgdir_t *pgdir, unsigned short index)
{
	pgdir_check(pgdir);
	pgdir_t *pd = pgdir_new(pgdir->pgdir.asid);
	return pgdir_add_entry(pgdir, index, pd, 0);
}

/*
 * Remove an entry with index @index within directory @pgdir.
 * Return the virtual address of page corresponding to that entry.
 * NOTE: Like pgdir_add_entry(), this function does not modify the page
 *       originally stored there in any way.
 */
addr_t pgdir_remove_entry(pgdir_t *pgdir, unsigned short index)
{
	pgdir_check(pgdir);
	addr_t result;
	unsigned long entry;

	--(pgdir->pgdir.entries);

	addr_t *dir = (addr_t *)PAGE_TO_KVADDR(pgdir);
	entry = dir[index];
	dir[index] = 0;

	if (entry & PTE_PHYS) {
		result = PADDR_TO_KVADDR((entry | PTE_LOWMASK) ^ PTE_LOWMASK);
	} else
		result = entry;

	return result;
}

/*
 * Remove an intermediate directory entry from the page directory.
 */
void pgdir_remove_pgdir(pgdir_t *pgdir, unsigned short index)
{
	pgdir_check(pgdir);
	addr_t result = pgdir_remove_entry(pgdir, index);
	pgdir_t *pd = KVADDR_TO_PAGE(result);
	if (pgdir_empty(pd))
		pgdir_delete(pd);
}
