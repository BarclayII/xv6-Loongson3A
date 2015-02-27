/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/mm/pgtable.h>
#include <asm/mipsregs.h>
#include <mm/mmap.h>
#include <mm/vmm.h>
#include <printk.h>
#include <ds/list.h>
#include <asm/memrw.h>
#include <string.h>
#include <assert.h>

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
	/*struct page *p = pgalloc();
	struct page *p0 = pgalloc();*/
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
		"dli	$16, 0x0000000001000000;"
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
	printk("ENTRYHI = %016x\r\n", read_c0_entryhi());
	printk("ENTRYLO0 = %016x\r\n", read_c0_entrylo0());
	printk("ENTRYLO1 = %016x\r\n", read_c0_entrylo1());
	memset((char *)PFN_TO_KVADDR(pfn1), '2', 4096);
	printk("phys = %016x\r\n", read_mem_long(PFN_TO_KVADDR(pfn1)));
	struct test *sample_va = (struct test *)0x0000000001000ff8;
	sample_va->a = 0x1111111111111111;
	sample_va->b = 0x2222222222222222;
	printk("sample_va = %016x\r\n", sample_va->a);
	printk("sample_shm = %016x\r\n", sample_va->b);
	asm volatile (
		"dmtc0	$0, $2;"
		"dmtc0	$0, $3;"
		"tlbwi"
	);
	printk("phys = %016x\r\n", read_mem_long(PFN_TO_KVADDR(pfn1) + 0xff8));
	printk("phys = %016x\r\n", read_mem_long(PFN_TO_KVADDR(pfn2)));
	/*pgfree(p);*/
	pgfree(p1);
	pgfree(p2);
	/*pgfree(p0);*/
	printk("phys = %016x\r\n", read_mem_long(PFN_TO_KVADDR(pfn1)));
}

void test_pgtable(void)
{
	pgd_t pgd = kern_mm.pgd;
	struct page *p1 = pgalloc();
	struct page *p2 = pgalloc();

	assert(!pgtable_insert(&pgd, 0x100000, p1, PTE_VALID, false, NULL));
	assert(!pgtable_insert(&pgd, 0x200000, p2, PTE_VALID, false, NULL));

	struct page *p3 = pgtable_remove(&pgd, 0x100000);
	assert(p1 == p3);
	struct page *p4 = pgtable_remove(&pgd, 0x200000);
	assert(p2 == p4);
	assert(pde_entries(pgd) == 0);

	pgfree(p3);
	pgfree(p4);
}

void test_tlb(void)
{
	struct page *p1 = pgalloc(), *p2 = pgalloc();
	unsigned long pfn1 = PAGE_TO_PFN(p1), pfn2 = PAGE_TO_PFN(p2);
	struct pagedesc pdesc1, pdesc2;
	volatile unsigned long *a = (volatile unsigned long *)0x500000;
	volatile unsigned long *b = (volatile unsigned long *)0x501000;

	printk("PFN1 = %016x, ENTRY = %016x\r\n",
	    pfn1, (pfn1 << 6) + 0x1e);
	printk("PFN2 = %016x, ENTRY = %016x\r\n",
	    pfn2, (pfn2 << 6) + 0x1e);
	/*pgtable_insert(&(kern_mm.pgd), (ptr_t)a, p1, PTE_VALID | PTE_DIRTY,
	    false, NULL);
	pgtable_insert(&(kern_mm.pgd), (ptr_t)b, p2, PTE_VALID | PTE_DIRTY,
	    false, NULL);
	pgtable_get(&(kern_mm.pgd), (ptr_t)a, false, &pdesc1);
	pgtable_get(&(kern_mm.pgd), (ptr_t)b, false, &pdesc2);
	dump_pagedesc((ptr_t)a, &pdesc1);
	dump_pagedesc((ptr_t)b, &pdesc2);
	printk("PTE1 = %016x\r\n", pdesc1.pte[pdesc1.ptx]);
	printk("PTE2 = %016x\r\n", pdesc2.pte[pdesc2.ptx]);*/
#if 1
	asm volatile (
		".set	mips64r2;"
		"dli	$16, 0x0000000000500000;"
		"dmtc0	$16, $10;"
		"move	$17, %0;"
		"move	$16, %1;"
		"dsll	$17, 6;"
		"dsll	$16, 6;"
		"daddiu	$17, 0x1e;"
		"daddiu	$16, 0x1e;"
		"dmtc0	$17, $2;"
		"dmtc0	$16, $3;"
		"tlbwr"
		: /* no output */
		: "r"(pfn1), "r"(pfn2)
		: "$16", "$17"
	);
#endif
	*a = 0xabcdef;
	*b = 0x123456;

	printk("ENTRYHI = %016x\r\n", read_c0_entryhi());
	printk("ENTRYLO0 = %016x\r\n", read_c0_entrylo0());
	printk("ENTRYLO1 = %016x\r\n", read_c0_entrylo1());
	
	printk("TEST VADDR(a): %016x\r\n", *a);
	printk("TEST PADDR(a): %016x\r\n", read_mem_ulong(PAGE_TO_KVADDR(p1)));
	printk("TEST VADDR(b): %016x\r\n", *b);
	printk("TEST PADDR(b): %016x\r\n", read_mem_ulong(PAGE_TO_KVADDR(p2)));
	
	write_mem_ulong(PAGE_TO_KVADDR(p1), 0x654321);
	write_mem_ulong(PAGE_TO_KVADDR(p2), 0xfedcba);
	printk("TEST VADDR(a): %016x\r\n", *a);
	printk("TEST PADDR(a): %016x\r\n", read_mem_ulong(PAGE_TO_KVADDR(p1)));
	printk("TEST VADDR(b): %016x\r\n", *b);
	printk("TEST PADDR(b): %016x\r\n", read_mem_ulong(PAGE_TO_KVADDR(p2)));

	pgfree(p1);
	pgfree(p2);
}
