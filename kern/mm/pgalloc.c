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
	struct page *p, *pfirst = NULL, *np;
	size_t i;
	intr_flag_t flag;

	/* TODO: acquire free_page_list lock */
	ENTER_CRITICAL_SECTION(NULL, flag);

	if (nr_free_pages < num) {
		EXIT_CRITICAL_SECTION(NULL, flag);
		return NULL;
	}
	p = first_free_page();

	/* Concatenate desired number of free pages into a list, in
	 * ascending order. */
	for (i = num; i > 0; ) {
		np = next_page(p);
		if (!is_page_reserved(p)) {
			reserve_page(p);
			page_delete(p);
			if (pfirst == NULL)
				pfirst = p;
			else {
				page_add_before(pfirst, p);
				p->type = PGTYPE_TAIL;
				p->first_page = pfirst;
			}
			--i;
			pdebug("Allocated PFN %d\r\n", PAGE_TO_PFN(p));
			--nr_free_pages;
			p->page_count = num;
		} else
			panic("Allocated page %d inside free list\r\n",
			    PAGE_TO_PFN(p));
		p = np;
	}

	/*
	 * IMPORTANT NOTE: the list of allocated pages, unlike free list,
	 * doesn't have a head node (or sentry node).  No idea how to fix
	 * this...
	 */
	EXIT_CRITICAL_SECTION(NULL, flag);
	pfirst->type = PGTYPE_GENERIC;
	return pfirst;
}

/*
 * Allocate @num contiguous pages
 */
struct page *alloc_cont_pages(size_t num)
{
	struct page *p = NULL, *np, *pfirst;
	size_t i;
	intr_flag_t flag;

	/* TODO: acquire free_page_list lock */
	ENTER_CRITICAL_SECTION(NULL, flag);

	if (nr_free_pages < num) {
		EXIT_CRITICAL_SECTION(NULL, flag);
		return NULL;
	}

	np = first_free_page();
	pfirst = NULL;

	for (i = num; (i > 0) || (np == end_free_page()); ) {
		p = np;
		np = next_page(p);

		if ((i > 1) && (p + 1 != np) && (np != end_free_page()))
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
		page_delete(p);
		if (pfirst != NULL)
			page_add_before(pfirst, p);
		pdebug("Allocated continual PFN %d\r\n", PAGE_TO_PFN(p));
		pfirst = p;
		p->page_count = num;
		--p;
		--nr_free_pages;
	}

	p = next_page(pfirst);
	for (i = 1; i < num; ++i) {
		p->type = PGTYPE_TAIL;
		p->first_page = pfirst;
		p = next_page(p);
	}
	assert(p == pfirst);

	EXIT_CRITICAL_SECTION(NULL, flag);
	pfirst->type = PGTYPE_GENERIC;
	return pfirst;
}

void free_all_pages(struct page *fp)
{	
	/*
	 * NOTE: see the last comment in alloc_pages()
	 */
	struct page *p, *freep, *np, *pfirst;
	size_t i, num = fp->page_count;
	intr_flag_t intr_flag;

	/* Find the first page in the allocated list */
	if (fp == NULL)
		return;

	/* TODO: acquire free_page_list lock */
	ENTER_CRITICAL_SECTION(NULL, intr_flag);

	p = pfirst = first_page(fp);
	freep = first_free_page();
	for (i = 0; i < num; ++i) {
		pdebug("Freeing PFN %d\r\n", PAGE_TO_PFN(p));
		np = next_page(p);
		page_delete(p);
		/* Locate suitable location for p */
		while ((freep != end_free_page()) && (freep < p))
			freep = next_page(freep);
		/* Checks */
		assert(freep != p);
		assert(is_page_reserved(p));
		/* Free the page and add it back to list */
		shred_and_release_page(p);
		page_add_before(freep, p);
		p->first_page = NULL;
		p->page_count = 0;
		pdebug("Freed PFN %d\r\n", PAGE_TO_PFN(p));
		++nr_free_pages;
		p = np;
	}

	EXIT_CRITICAL_SECTION(NULL, intr_flag);
}
