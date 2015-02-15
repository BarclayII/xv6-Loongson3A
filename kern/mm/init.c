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
#include <ds/list.h>
#include <printk.h>
#include <string.h>

unsigned long highmem_base_pfn;

unsigned long base_pfn;

/*
 * An array of page structures tracking information of (high memory) physical
 * pages.
 * NOTE: This array locates in high memory.
 */
struct page *page_array;

struct free_page_group free_pages;

static void init_free_page_list(size_t nr_pages, size_t nr_occupied)
{
	size_t i;

	list_init(free_page_list);

	for (i = nr_occupied; i < nr_pages; ++i) {
		list_add_before(free_page_list, &(page_array[i].list_node));
	}

	nr_free_pages = nr_pages - nr_occupied;
	
	printk("Built free page list of %d pages\r\n", nr_free_pages);
}

static void init_page_array(size_t nr_pages)
{
	unsigned long page_array_bytes = nr_pages * sizeof(struct page);
	unsigned long page_array_pages = NR_PAGES_NEEDED(page_array_bytes);

	memset(page_array, 0, page_array_bytes);
	/*
	 * Since the page array itself is inside high memory, we need to
	 * count and reserve the first not-so-few pages the page array
	 * occupies.
	 */
	unsigned long i;

	for (i = 0; i < page_array_pages; ++i) {
		reserve_page(&(page_array[i]));
	}

	printk("%d pages initialized.\r\n", nr_pages);
	printk("%d pages reserved for page structures.\r\n", page_array_pages);

	init_free_page_list(nr_pages, page_array_pages);
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
	printk("Highmem base PFN: %d\r\n", highmem_base_pfn);

	page_array = (struct page *)PFN_TO_KVADDR(highmem_base_pfn);
	init_page_array(num_pages);

	printk("First free PFN: %d\r\n",
	    PAGE_TO_PFN(list_node_to_page(free_page_list->next)));
}

void mm_init(void)
{
	setup_page_array();
}