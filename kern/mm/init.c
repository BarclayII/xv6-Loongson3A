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
#include <mm/mmap.h>
#include <printk.h>

unsigned long highmem_base_pfn;

unsigned long base_pfn;

struct page *page_array;

static void setup_page_array(void)
{
	unsigned long lowmemsize = (memsize > MAX_LOW_MEM_MB) ?
	    memsize : MAX_LOW_MEM_MB;
	unsigned long lowmembytes = MB_TO_BYTES(lowmemsize);
	unsigned long highmembytes = MB_TO_BYTES(highmemsize);

	unsigned long num_pages = NR_PAGES(highmembytes);

	/* The lowest physical frame should be just above the lower memory. */
	highmem_base_pfn = NR_PAGES(lowmembytes);
	base_pfn = highmem_base_pfn;

	/* I'm trying to put page arrays into high memory in order to support
	 * potentially larger physical memories */
	page_array = (struct page *)PFN_TO_XKSEG(base_pfn);

	printk("highmem_base_pfn = %016x\r\n", highmem_base_pfn);
	printk("page_array_base = %016x\r\n", page_array);
	printk("num_pages = %u\r\n", num_pages);
	printk("page array bytes: %u\r\n", num_pages * sizeof(struct page));
	printk("pages needed for page array: %u\r\n",
	    NR_PAGES(num_pages * sizeof(struct page)));
	base_pfn += NR_PAGES(num_pages * sizeof(struct page));
}

void mm_init(void)
{
	addrspace_init();
	setup_page_array();
}
