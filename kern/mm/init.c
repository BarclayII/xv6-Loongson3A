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
#include <asm/memrw.h>
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
	p3 = alloc_cont_pages(5);
	printk("PFN for p3: %d\r\n", PAGE_TO_PFN(p3));
	printk("First free PFN after 3rd alloc: %d\r\n", first_free_pfn);
	free_pages(p2);
	printk("First free PFN after 2nd free: %d\r\n", first_free_pfn);
	free_pages(p3);
	printk("First free PFN after 3rd free: %d\r\n", first_free_pfn);
#undef first_free_pfn
}

static void test2_mm(void)
{
	struct page *p1 = pgalloc();
	struct page *p2 = pgalloc();
	unsigned long pfn1 = PAGE_TO_PFN(p1);
	unsigned long pfn2 = PAGE_TO_PFN(p2);
	printk("PFN = %d\r\n", pfn1);
	printk("KVADDR = %016x\r\n", PFN_TO_KVADDR(pfn1));
	printk("PFN = %d\r\n", pfn2);
	printk("KVADDR = %016x\r\n", PFN_TO_KVADDR(pfn2));
	asm volatile (
		".set	mips64r2;"
		"dli	$16, 0xc000000000000000;"
		"dmtc0	$16, $10;"
		"move	$17, %0;"
		"move	$16, %1;"
		"dsll	$17, 6;"
		"dsll	$16, 6;"
		"daddiu	$17, 0x1e;"
		"daddiu	$16, 0x1e;"
		"dmtc0	$17, $2;"
		"dmtc0	$17, $3;"
		"dmtc0	$0, $0;"
		"tlbwi"
		: /* no output */
		: "r"(pfn1), "r"(pfn2)
		: "$16", "$17"
	);
	memset((char *)PFN_TO_KVADDR(pfn1), '2', 4096);
	printk("phys = %016x\r\n", read_mem_long(PFN_TO_KVADDR(pfn1)));
	long *sample_va = (long *)0xc000000000000000;
	write_mem_long(sample_va, 0x1111111111111111);
	printk("sample_va = %016x\r\n", *sample_va);
	printk("phys = %016x\r\n", read_mem_long(PFN_TO_KVADDR(pfn1)));
	asm volatile (
		".set	mips64r2;"
		"dmtc0	$0, $2;"
		"dmtc0	$0, $3;"
		"tlbwi;"
		"move	$17, %0;"
		"move	$16, %1;"
		"dsll	$17, 6;"
		"dsll	$16, 6;"
		"daddiu	$17, 0x1e;"
		"daddiu	$16, 0x1e;"
		"dmtc0	$17, $2;"
		"dmtc0	$17, $3;"
		"dmtc0	$0, $0;"
		"tlbwi"
		: /* no output */
		: "r"(pfn1), "r"(pfn2)
		: "$16", "$17"
	);
	/* FIXME: sample_shm should be equal to phys...? */
	sample_va = (long *)0xc000000000001000;
	write_mem_long(sample_va, 0x2222222222222222);
	printk("sample_shm = %016x\r\n", *sample_va);
	printk("phys = %016x\r\n", read_mem_long(PFN_TO_KVADDR(pfn1)));
	pgfree(p1);
	pgfree(p2);
	printk("phys = %016x\r\n", read_mem_long(PFN_TO_KVADDR(pfn1)));
}

void mm_init(void)
{
	setup_page_array();
	test_mm();
	test2_mm();
}
