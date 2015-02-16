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
#include <asm/mipsregs.h>
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

struct free_page_group free_page_group;

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

	printk("PAGEARRAY: %016x\r\n", page_array);
	memset(page_array, 0, page_array_bytes);
	/*
	 * Since the page array itself is inside high memory, we need to
	 * count and reserve the first not-so-few pages the page array
	 * occupies.
	 */
	unsigned long i;

	for (i = 0; i < nr_pages; ++i) {
		list_init(&(page_array[i].list_node));
	}
	printk("%d pages initialized.\r\n", nr_pages);

	for (i = 0; i < page_array_pages; ++i) {
		reserve_page(&(page_array[i]));
	}

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
	highmem_base_pfn = NR_PAGES_NEEDED(memlimit + lowmembytes);
	printk("Highmem base PFN: %016x\r\n", highmem_base_pfn);

	page_array = (struct page *)PFN_TO_KVADDR(highmem_base_pfn);
	init_page_array(num_pages);
}

static void test_mm(void)
{
#define first_free_pfn \
	PAGE_TO_PFN(list_node_to_page(list_next(free_page_list)))
	struct page *p1, *p2, *p3, *q;
	list_node_t *node;

	printk("First free PFN: %d\r\n", first_free_pfn);
	p1 = alloc_pages(3);
	printk("First free PFN after alloc: %d\r\n", first_free_pfn);
	node = &(p1->list_node);
	printk("Allocated PFN: %d\r\n", PAGE_TO_PFN(p1));
	node = list_next(node);
	q = list_node_to_page(node);
	printk("Allocated PFN: %d\r\n", PAGE_TO_PFN(q));
	node = list_next(node);
	q = list_node_to_page(node);
	printk("Allocated PFN: %d\r\n", PAGE_TO_PFN(q));
	node = list_next(node);
	q = list_node_to_page(node);
	printk("Allocated PFN: %d\r\n", PAGE_TO_PFN(q));
	p2 = alloc_pages(3);
	printk("First free PFN after 2nd alloc: %d\r\n", first_free_pfn);
	free_pages(p1);
	printk("First free PFN after 1st free: %d\r\n", first_free_pfn);
	p3 = alloc_pages(5);
	printk("First free PFN after 3rd alloc: %d\r\n", first_free_pfn);
	free_pages(p2);
	printk("First free PFN after 2nd free: %d\r\n", first_free_pfn);
	free_pages(p3);
	printk("First free PFN after 3rd free: %d\r\n", first_free_pfn);
#undef first_free_pfn
}

static void test2_mm(void)
{
	struct page *p = pgalloc();
	unsigned long pfn = PAGE_TO_PFN(p);
	printk("PFN = %d\r\n", pfn);
	printk("KVADDR = %016x\r\n", PFN_TO_KVADDR(pfn));
	asm volatile (
		".set	mips64r2;"
		"dli	$26, 0xc000000000000000;"
		"dmtc0	$26, $10;"
		"dli	$27, 0x447801e;"
		"dmtc0	$27, $2;"
		"dmtc0	$27, $3;"
		"tlbwr"
		: /* no output */
		: "r"(pfn)
		: "$26", "$27"
	);
	printk("ENTRYHI = %016x\r\n", read_c0_entryhi());
	asm volatile (
		"tlbp"
	);
	printk("ENTRYLO0 = %016x\r\n", read_c0_entrylo0());
	printk("ENTRYLO1 = %016x\r\n", read_c0_entrylo1());
	memset((char *)PFN_TO_KVADDR(pfn), '2', 4096);
	printk("phys = %016x\r\n", *(long *)PFN_TO_KVADDR(pfn));
	long *sample_va = (long *)0xc000000000000000;
	memset(sample_va, '0', 4096);
	printk("sample_va = %016x\r\n", *sample_va);
	printk("phys = %016x\r\n", *(long *)PFN_TO_KVADDR(pfn));
	memset((char *)sample_va + 4096, '1', 4096);
	/* FIXME: sample_shm should be equal to phys...? */
	printk("sample_shm = %016x\r\n", *(long *)((char *)sample_va + 4096));
	printk("phys = %016x\r\n", *(long *)PFN_TO_KVADDR(pfn));
	pgfree(p);
	printk("phys = %016x\r\n", *(long *)PFN_TO_KVADDR(pfn));
}

void mm_init(void)
{
	setup_page_array();
	test_mm();
	test2_mm();
}
