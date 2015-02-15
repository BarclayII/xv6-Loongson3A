/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/ld.h>
#include <asm/bootinfo.h>
#include <asm/addrspace.h>
#include <asm/mm/page.h>
#include <mm/mmap.h>
#include <printk.h>

unsigned long highmem_base_pfn;

/*
 * An array of page structures tracking information of (high memory) physical
 * pages.
 * NOTE: This array locates in high memory.
 */
struct page *page_array;

static void init_page_array(size_t nr_pages)
{
	unsigned long page_array_bytes = nr_pages * sizeof(struct page);
	memset(page_array, 0, page_array_bytes);
	/*
	 * Since the page array itself is inside high memory, we need to
	 * count and reserve the first not-so-few pages the page array occupies.
	 */
	unsigned long page_array_pages = NR_PAGES_NEEDED(page_array_bytes);
	unsigned long i;

	for (i = 0; i < page_array_pages; ++i) {
		reserve_page(&(page_array[i]));
	}

	printk("0 page reserved: %d\r\n", is_page_reserved(&(page_array[0])));
}

static void setup_page_array(void)
{
	unsigned long lowmemsize = (memsize > MAX_LOW_MEM_MB) ?
	    memsize : MAX_LOW_MEM_MB;
	unsigned long lowmembytes = MB_TO_BYTES(lowmemsize);
	unsigned long highmembytes = MB_TO_BYTES(highmemsize);
	unsigned long num_pages = NR_PAGES_AVAIL(highmembytes);

	/* The lowest physical frame should be just above the lower memory. */
	highmem_base_pfn = NR_PAGES_NEEDED(lowmembytes);

	page_array = (struct page *)PFN_TO_KVADDR(highmem_base_pfn);
	init_page_array(num_pages);
}

void mm_init(void)
{
	setup_page_array();
}
