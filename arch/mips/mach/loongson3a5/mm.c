/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
 
#include <mm/mmap.h>
#include <printk.h>

static void setup_page_array(void)
{
	unsigned long lowmemsize = (memsize > MAX_LOW_MEM_MB) ?
	    memsize : MAX_LOW_MEM_MB;
	unsigned long lowmembytes = MB_TO_BYTES(lowmemsize);
	unsigned long highmembytes = MB_TO_BYTES(highmemsize);
	unsigned long num_pages = NR_PAGES_AVAIL(highmembytes);

	/* The lowest physical frame should be just above the lower memory. */
	highmem_base_pfn = NR_PAGES_NEEDED(memlimit + lowmembytes);
	printk("Highmem base PFN: %016x\r\n", highmem_base_pfn);

	page_array = (struct page *)PFN_TO_KVADDR(highmem_base_pfn);
	init_page_array(num_pages);
}

void mach_mm_init()
{
        setup_page_array();
}
