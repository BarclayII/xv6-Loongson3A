/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <mm/hier/pgdir.h>
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

pgdir_t *pgdir_new(void)
{
	struct page *p = pgalloc();
	memset((void *)PAGE_TO_KVADDR(p), 0, PGSIZE);
	p->type = PGTYPE_PGDIR;
	p->entries = 0;
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
ptr_t pgdir_add_entry(pgdir_t *pgdir, unsigned short index, struct page *p)
{
	pgdir_check(pgdir);
	ptr_t *dir = (ptr_t *)PAGE_TO_KVADDR(pgdir);
	if (dir[index]) {
		/* Shouldn't allow two different entries having same index */
		printk("pgdir = %016x\r\n", dir);
		printk("index = %d\r\n", index);
		printk("content = %016x\r\n", dir[index]);
		panic("Adding entry to the same position twice.\r\n");
	}
	++(pgdir->entries);
	dir[index] = PAGE_TO_PFN(p);
	return PFN_TO_KVADDR(dir[index]);
}

/*
 * Allocate a new page and send it into the directory.
 */
ptr_t pgdir_add_page(pgdir_t *pgdir, unsigned short index)
{
	pgdir_check(pgdir);
	struct page *p = pgalloc();
	memset((void *)PAGE_TO_KVADDR(p), 0, PGSIZE);
	inc_pageref(p);
	return pgdir_add_entry(pgdir, index, p);
}

/*
 * Allocate a new page directory, usually an intermediate one, into
 * the page directory.
 */
ptr_t pgdir_add_pgdir(pgdir_t *pgdir, unsigned short index)
{
	pgdir_check(pgdir);
	pgdir_t *pd = pgdir_new();
	memset((void *)PAGE_TO_KVADDR(pd), 0, PGSIZE);
	return pgdir_add_entry(pgdir, index, pd);
}

/*
 * Remove an entry with index @index within directory @pgdir.
 * NOTE: Like pgdir_add_entry(), this function does not modify the page
 *       originally stored there in any way.
 */
ptr_t pgdir_remove_entry(pgdir_t *pgdir, unsigned short index)
{
	pgdir_check(pgdir);
	ptr_t result;
	--(pgdir->entries);
	ptr_t *dir = (ptr_t *)PAGE_TO_KVADDR(pgdir);
	result = dir[index];
	dir[index] = 0;
	return PFN_TO_KVADDR(result);
}

void pgdir_remove_page(pgdir_t *pgdir, unsigned short index)
{
	pgdir_check(pgdir);
	ptr_t result = pgdir_remove_entry(pgdir, index);
	page_unref(KVADDR_TO_PAGE(result));
}

/*
 * Remove an intermediate directory entry from the page directory.
 */
void pgdir_remove_pgdir(pgdir_t *pgdir, unsigned short index)
{
	pgdir_check(pgdir);
	ptr_t result = pgdir_remove_entry(pgdir, index);
	pgdir_t *pd = KVADDR_TO_PAGE(result);
	if (pd->entries == 0)
		pgdir_delete(pd);
}
