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
#include <stddef.h>
#include <string.h>
#include <printk.h>
#include <panic.h>
#include <assert.h>
#include <sync.h>
#include <mm/mmap.h>
#include <ds/list.h>

inline void shred_page(struct page *p)
{
	static const unsigned char junk = 0x01;
	memset((void *)PAGE_TO_KVADDR(p), junk, PGSIZE);
}

/*
 * Allocate @num may-be-non-contiguous physical pages
 */
struct page *alloc_pages(size_t num)
{
	struct page *p, *pfirst = NULL;
	size_t i;
	intr_flag_t flag;

	/* TODO: acquire free_page_list lock */
	ENTER_CRITICAL_SECTION(NULL, flag);

	list_node_t *pgentry, *cur_entry = list_next(free_page_list);

	if (nr_free_pages < num) {
		EXIT_CRITICAL_SECTION(NULL, flag);
		return NULL;
	}

	/* Concatenate desired number of free pages into a list, in
	 * ascending order. */
	for (i = num; i > 0; ) {
		pgentry = cur_entry;
		cur_entry = list_next(cur_entry);

		p = node_to_page(pgentry);
		if (!is_page_reserved(p)) {
			reserve_page(p);
			list_del_init(pgentry);
			if (pfirst == NULL)
				pfirst = p;
			else
				list_add_before(&(pfirst->list_node), pgentry);
			--i;
			pdebug("Allocated PFN %d\r\n", PAGE_TO_PFN(p));
			--nr_free_pages;
			p->page_count = num;
		}
	}

	/*
	 * IMPORTANT NOTE: the list of allocated pages, unlike free list,
	 * doesn't have a head node (or sentry node).  No idea how to fix
	 * this...
	 */
	EXIT_CRITICAL_SECTION(NULL, flag);
	return pfirst;
}

/*
 * Allocate @num contiguous pages
 */
struct page *alloc_cont_pages(size_t num)
{
	struct page *p = NULL, *np, *pfirst;
	int i;
	intr_flag_t flag;
	list_node_t *pgentry = NULL, *cur_entry;

	/* TODO: acquire free_page_list lock */
	ENTER_CRITICAL_SECTION(NULL, flag);

	if (nr_free_pages < num) {
		EXIT_CRITICAL_SECTION(NULL, flag);
		return NULL;
	}

	cur_entry = list_next(free_page_list);
	pfirst = NULL;

	for (i = num; (i > 0) || (cur_entry == free_page_list); ) {
		pgentry = cur_entry;
		cur_entry = list_next(cur_entry);
		p = node_to_page(pgentry);
		if (cur_entry != free_page_list)
			np = node_to_page(cur_entry);
		else
			np = NULL;

		if ((i > 1) && (p + 1 != np))
			i = num;
		else
			--i;
	}

	if (i > 0) {
		EXIT_CRITICAL_SECTION(NULL, flag);
		return NULL;
	}

	/* p is now the last page of @num contiguous pages */
	for (i = 0; i < num; ++i) {
		reserve_page(p);
		list_del_init(&(p->list_node));
		if (pfirst != NULL)
			list_add_before(&(pfirst->list_node), &(p->list_node));
		pdebug("Allocated continual PFN %d\r\n", PAGE_TO_PFN(p));
		pfirst = p;
		p->page_count = num;
		--p;
		--nr_free_pages;
	}

	EXIT_CRITICAL_SECTION(NULL, flag);
	return pfirst;
}

void free_pages(struct page *freep)
{	
	/*
	 * NOTE: see the last comment in alloc_pages()
	 */

	list_node_t *pfirst_entry, *cur_entry = NULL, *free_entry;
	struct page *p;
	bool last_page = false;
	intr_flag_t intr_flag;

	/* Find the first page in the allocated list */
	if (freep == NULL)
		return;
	for (pfirst_entry = &(freep->list_node);
	    pfirst_entry > list_prev(pfirst_entry);
	    pfirst_entry = list_prev(pfirst_entry))
		/* nothing */;

	/* TODO: acquire free_page_list lock */
	ENTER_CRITICAL_SECTION(NULL, intr_flag);

	free_entry = list_next(free_page_list);
	while (!last_page) {
		if (list_single(pfirst_entry))
			last_page = true;
		cur_entry = pfirst_entry;
		pfirst_entry = list_next(pfirst_entry);
		list_del(cur_entry);
		/* Locate suitable location for current allocated page */
		while (free_entry != free_page_list && free_entry < cur_entry)
			free_entry = list_next(free_entry);
		p = node_to_page(cur_entry);
		/* Checks */
		if (free_entry == cur_entry)
			panic("Page %d is both allocated and free?\r\n",
			    PAGE_TO_PFN(p));
		if (!is_page_reserved(p))
			panic("Page %d is allocated but not reserved\r\n",
			    PAGE_TO_PFN(p));
		/* Free the page and add the page back to free list */
		shred_and_release_page(p);
		list_add_before(free_entry, cur_entry);
		pdebug("Freed PFN %d\r\n", PAGE_TO_PFN(p));
		
		++nr_free_pages;
	}

	EXIT_CRITICAL_SECTION(NULL, intr_flag);
}
