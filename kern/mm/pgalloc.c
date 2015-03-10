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
	pfirst->page_count = num;
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
	pfirst->page_count = num;
	return pfirst;
}

/*
 * Freeing given number of pages is astonishing when the allocated page list
 * is incontiguous.  We may have to shrink the list, or split it into two
 * parts (the "head" list and the "tail" list).
 *
 * Returns the first page following the freed page section.
 */
struct page *free_pages(struct page *base, size_t num)
{
	if (num == 0 || base == NULL)
		return NULL;

	struct page *first = first_page(base);
	struct page *last;
	struct page *p;
	size_t i, head_len;
	size_t total = page_count(first);

	/* Find out the length of "head" list */
	for (head_len = 0, p = first; p != base; ++head_len, p = next_page(p))
		assert(is_page_reserved(p));

	/* Find last page (exclusive) */
	for (i = 0, last = base; i < num; ++i, last = next_page(last))
		/* Check whether the number given exceeds the number of pages
		 * available to free */
		assert((last != first || last == base) &&
		    is_page_reserved(last));

	/* Mark @last the first page of "tail" list */
	/* Change the page type first */
	last->type = base->type = first->type;

	/* Change page count field and first page field in all sublists */
#define update_page_list(i, from, to, count) \
	do { \
		for ((i) = (from); (i) != (to); (i) = next_page(i)) \
			if (is_tail(i)) \
				(i)->first_page = (from); \
			else \
				(i)->page_count = (count); \
	} while (0)
	update_page_list(p, first, base, head_len);
	update_page_list(p, base, last, num);
	update_page_list(p, last, first, total - head_len - num);
#undef update_page_list

	/* Split the page list into two (or three) parts */
	if (last != first)
		page_split(first, prev_page(last), last);
	if (base != first)
		page_split(first, prev_page(base), base);
	/* From now on the page list is splitted into @base, @first (possibly)
	 * and (also possibly) @last. */

	/* Free the pages */
	free_all_pages(base);

	return last;
}

void free_all_pages(struct page *fp)
{	
	/*
	 * NOTE: see the last comment in alloc_pages()
	 */
	struct page *p, *freep, *np, *pfirst;
	size_t i, num = page_count(fp);
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
		pdebug("Freed PFN %d\r\n", PAGE_TO_PFN(p));
		++nr_free_pages;
		p = np;
	}

	EXIT_CRITICAL_SECTION(NULL, intr_flag);
}
