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
#include <mm/vmm.h>
#include <printk.h>
#include <ds/list.h>
#include <asm/memrw.h>
#include <string.h>

void test_mm(void)
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

struct test {
	unsigned long a;
	unsigned long b;
};

void test2_mm(void)
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
		"dmtc0	$16, $3;"
		"dmtc0	$0, $0;"
		"tlbwi"
		: /* no output */
		: "r"(pfn1), "r"(pfn2)
		: "$16", "$17"
	);
	memset((char *)PFN_TO_KVADDR(pfn1), '2', 4096);
	printk("phys = %016x\r\n", read_mem_long(PFN_TO_KVADDR(pfn1)));
	struct test *sample_va = (struct test *)0xc000000000000ff8;
	sample_va->a = 0x1111111111111111;
	printk("sample_va = %016x\r\n", sample_va->a);
	printk("phys = %016x\r\n", read_mem_long(PFN_TO_KVADDR(pfn1) + 0xff8));
	sample_va->b = 0x2222222222222222;
	printk("sample_shm = %016x\r\n", sample_va->b);
	printk("phys = %016x\r\n", read_mem_long(PFN_TO_KVADDR(pfn2)));
	pgfree(p1);
	pgfree(p2);
	printk("phys = %016x\r\n", read_mem_long(PFN_TO_KVADDR(pfn1)));
}

void test_pgtable(void)
{
	pgd_t pgd = kern_mm.pgd;
	struct page *p1 = pgalloc();
	struct page *p2 = pgalloc();

	if (pgtable_insert(&pgd, 0x100000, p1, false, NULL) != 0)
		panic("Inserting p1 failed\r\n");
	if (pgtable_insert(&pgd, 0x200000, p2, false, NULL) != 0)
		panic("Inserting p2 failed\r\n");

	struct page *p3 = pgtable_remove(&pgd, 0x100000);
	if (p1 != p3)
		panic("Assertation failed p1 != p3\r\n");
	struct page *p4 = pgtable_remove(&pgd, 0x200000);
	if (p2 != p4)
		panic("Assertation failed p2 != p4\r\n");
	if (pde_entries(pgd) != 0)
		panic("Assertation failed entries != 0\r\n");

	pgfree(p3);
	pgfree(p4);
}

