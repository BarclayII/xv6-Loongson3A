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

	/* I'm trying to put page arrays into high memory in order to support
	 * potentially larger physical memories */
	page_array = (struct page *)xkseg_top;

	printk("highmem_base_pfn = %016x\r\n", highmem_base_pfn);
	printk("page_array_base = %016x\r\n", page_array);
	printk("num_pages = %u\r\n", num_pages);
	printk("page_array_end = %016x\r\n", page_array + num_pages);
}

void mm_init(void)
{
	addrspace_init();
	setup_page_array();
}
