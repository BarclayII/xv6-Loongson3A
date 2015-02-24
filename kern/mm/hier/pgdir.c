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

/*
 * Allocate a new page directory object
 */
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
	pgfree((struct page *)pgdir);
}

/*
 * Add a page @p into directory @pgdir with index @index.
 * NOTE: This function does *not* modify anything in @p.  Increasing
 *       reference counter or things like that should be done outside.
 */
ptr_t pgdir_add_entry(pgdir_t *pgdir, unsigned short index, struct page *p)
{
	ptr_t *dir = (ptr_t *)PAGE_TO_KVADDR(pgdir);
	if (dir[index]) {
		/* Shouldn't allow two different entries having same index */
		printk("pgdir = %016x\r\n", dir);
		printk("index = %d\r\n", index);
		printk("content = %016x\r\n", dir[index]);
		panic("Adding entry to the same position twice.\r\n");
	}
	++(pgdir->entries);
	dir[index] = PAGE_TO_KVADDR(p);
	return dir[index];
}

/*
 * Allocate a new page and send it into the directory.
 */
ptr_t pgdir_add_page(pgdir_t *pgdir, unsigned short index)
{
	struct page *p = pgalloc();
	memset((void *)PAGE_TO_KVADDR(p), 0, PGSIZE);
	inc_pageref(p);
	return pgdir_add_entry(pgdir, index, p);
}

/*
 * Remove an entry with index @index within directory @pgdir.
 * NOTE: Like pgdir_add_entry(), this function does not modify the page
 *       originally stored there in any way.
 */
ptr_t pgdir_remove_entry(pgdir_t *pgdir, unsigned short index)
{
	ptr_t result;
	--(pgdir->entries);
	ptr_t *dir = (ptr_t *)PAGE_TO_KVADDR(pgdir);
	result = dir[index];
	dir[index] = 0;
	return result;
}

void pgdir_remove_page(pgdir_t *pgdir, unsigned short index)
{
	ptr_t result = pgdir_remove_entry(pgdir, index);
	page_unref(KVADDR_TO_PAGE(result));
}

