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
#include <asm/mm/pgtable.h>
#include <asm/mm/tlb.h>
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
	printk("**********test_mm**********\r\n");
#define first_free_pfn \
	PAGE_TO_PFN(node_to_page(list_next(free_page_list)))
	struct page *p1, *p2, *p3;
	list_node_t *node;
	int i;

	printk("First free PFN: %d\r\n", first_free_pfn);
	p1 = alloc_pages(3);
	for (i = 0, node = &(p1->list_node);
	    i < p1->page_count;
	    ++i, node = list_next(node)) {
		printk("\tAllocated PFN: %d\r\n",
		    PAGE_TO_PFN(node_to_page(node)));
		assert(node_to_page(node)->page_count == 3);
	}
	printk("First free PFN after alloc: %d\r\n", first_free_pfn);
	p2 = alloc_pages(3);
	for (i = 0, node = &(p2->list_node);
	    i < p2->page_count;
	    ++i, node = list_next(node)) {
		printk("\tAllocated PFN: %d\r\n",
		    PAGE_TO_PFN(node_to_page(node)));
		assert(node_to_page(node)->page_count == 3);
	}
	printk("First free PFN after 2nd alloc: %d\r\n", first_free_pfn);
	free_pages(p1);
	printk("First free PFN after 1st free: %d\r\n", first_free_pfn);
	p3 = alloc_cont_pages(5);
	printk("PFN for p3: %d\r\n", PAGE_TO_PFN(p3));
	for (i = 0, node = &(p3->list_node);
	    i < p3->page_count;
	    ++i, node = list_next(node)) {
		printk("\tAllocated PFN: %d\r\n",
		    PAGE_TO_PFN(node_to_page(node)));
		assert(node_to_page(node)->page_count == 5);
	}
	printk("First free PFN after 3rd alloc: %d\r\n", first_free_pfn);
	free_pages(p2);
	printk("First free PFN after 2nd free: %d\r\n", first_free_pfn);
	free_pages(p3);
	printk("First free PFN after 3rd free: %d\r\n", first_free_pfn);
	printk("Current free pages: %d\r\n", nr_free_pages);
}

struct test {
	unsigned long a;
	unsigned long b;
};

/*
 * I found that the secondary cache and main memory may not notice the
 * changes happening in the primary (data) caches, so this test routine
 * is somehow meaningless until I add some manual cache blasters(?)
 */

#if 0
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
#endif

void test_pgtable(void)
{
	printk("**********test_pgtable**********\r\n");
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
	printk("Current free pages: %d\r\n", nr_free_pages);
}

void test_tlb(void)
{
	printk("**********test_tlb**********\r\n");
	struct page *p1 = pgalloc(), *p2 = pgalloc(), *p3 = pgalloc();
	unsigned long pfn1 = PAGE_TO_PFN(p1), pfn2 = PAGE_TO_PFN(p2);
	struct pagedesc pdesc1, pdesc2;
	volatile unsigned long *a = (volatile unsigned long *)0x500000;
	volatile unsigned long *b = (volatile unsigned long *)0x504000;

	printk("PFN1 = %016x\r\n", pfn1);
	printk("PFN2 = %016x\r\n", pfn2);
	pgtable_insert(&(kern_mm.pgd), (addr_t)a, p1, PTE_VALID | PTE_DIRTY,
	    false, NULL);
	pgtable_insert(&(kern_mm.pgd), (addr_t)b, p2, PTE_VALID | PTE_DIRTY,
	    false, NULL);
	pgtable_get(&(kern_mm.pgd), (addr_t)a, false, &pdesc1);
	pgtable_get(&(kern_mm.pgd), (addr_t)b, false, &pdesc2);
	dump_pagedesc((addr_t)a, &pdesc1);
	dump_pagedesc((addr_t)b, &pdesc2);
	printk("PTE1 = %016x\r\n", pdesc1.pte[pdesc1.ptx]);
	printk("PTE2 = %016x\r\n", pdesc2.pte[pdesc2.ptx]);

	/* Currently, without cache blasters, we can only verify correctness
	 * of page tables by writing into physical memory, and reading the
	 * data by means of virtual addresses. */
	write_mem_ulong(PAGE_TO_KVADDR(p1), 0x654321);
	write_mem_ulong(PAGE_TO_KVADDR(p2), 0xfedcba);
	write_mem_ulong(PAGE_TO_KVADDR(p3), 0xeeeeee);
	unsigned long pa = read_mem_ulong(PAGE_TO_KVADDR(p1));
	unsigned long pb = read_mem_ulong(PAGE_TO_KVADDR(p2));
	assert(*a == pa);
	assert(*b == pb);

	*a = 0x123456;
	*b = 0xabcdef;
	pa = read_mem_ulong(PAGE_TO_KVADDR(p1));
	pb = read_mem_ulong(PAGE_TO_KVADDR(p2));
	assert(*a == pa);
	assert(*b == pb);

	assert(pgtable_remove(&(kern_mm.pgd), (addr_t)a) == p1);
#if 0
	printk("TEST: %016x\r\n", *b);	/* should not panic */
	printk("TEST: %016x\r\n", *a);	/* should panic */
#endif
	assert(pgtable_remove(&(kern_mm.pgd), (addr_t)b) == p2);
#if 0
	printk("TEST: %016x\r\n", *a);	/* should panic */
	printk("TEST: %016x\r\n", *b);	/* should panic */
#endif
	printk("Inserting back...\r\n");
	pgtable_insert(&(kern_mm.pgd), (addr_t)a, p1, PTE_VALID,
	    false, NULL);
	pgtable_get(&(kern_mm.pgd), (addr_t)a, false, &pdesc1);
	dump_pagedesc((addr_t)a, &pdesc1);
	assert(*a == pa);
#if 0
	printk("TEST: %016x\r\n", *b);	/* should panic */
#endif
	assert(pgtable_remove(&(kern_mm.pgd), (addr_t)a) == p1);

#undef first_free_pfn

	pgfree(p1);
	pgfree(p2);
	pgfree(p3);

	printk("After freeing: %d %016x  %d %016x\r\n",
	    PAGE_TO_PFN(p1), read_mem_ulong(PAGE_TO_KVADDR(p1)),
	    PAGE_TO_PFN(p2), read_mem_ulong(PAGE_TO_KVADDR(p2)));
}

void test_shm(void)
{
	printk("**********test_shm**********\r\n");

	pgd_t *pgd = &(kern_mm.pgd);
	struct page *p = pgalloc();
	printk("PFN = %d\r\n", PAGE_TO_PFN(p));
	unsigned long vaddr1 = 0x800000, vaddr2 = 0x1000000;
	struct pagedesc pd1, pd2;

	assert(!pgtable_insert(pgd, vaddr1, p, PTE_VALID, 
	    false, NULL));
	assert(!pgtable_insert(pgd, vaddr2, p, PTE_VALID,
	    false, NULL));

	pgtable_get(pgd, vaddr1, false, &pd1);
	pgtable_get(pgd, vaddr2, false, &pd2);
	dump_pagedesc(vaddr1, &pd1);
	printk("PFN1 ENTRY: %016x\r\n", pd1.pte[pd1.ptx]);
	dump_pagedesc(vaddr2, &pd2);
	printk("PFN2 ENTRY: %016x\r\n", pd2.pte[pd2.ptx]);

	unsigned long value = 0x12345678;
	write_mem_ulong(PAGE_TO_KVADDR(p), value);

	printk("TEST: %016x\r\n", read_mem_ulong(PAGE_TO_KVADDR(p)));
	printk("TEST: %016x\r\n", read_mem_ulong(vaddr1));
	printk("TEST: %016x\r\n", read_mem_ulong(vaddr2));

	write_c0_entryhi(vaddr1);
	tlbp();
	unsigned int index = read_c0_index();
	printk("INDEX = %d, ", index);
	if (index >= 0) {
		tlbr();
		printk("LO0 = %016x, LO1 = %016x\r\n",
		    read_c0_entrylo0(),
		    read_c0_entrylo1());
	}
	write_c0_entryhi(vaddr2);
	tlbp();
	index = read_c0_index();
	printk("INDEX = %d, ", index);
	if (index >= 0) {
		tlbr();
		printk("LO0 = %016x, LO1 = %016x\r\n",
		    read_c0_entrylo0(),
		    read_c0_entrylo1());
	}

	assert(read_mem_ulong(vaddr1) == value);
	assert(read_mem_ulong(vaddr2) == value);

	assert(pgtable_remove(pgd, vaddr1) == p);
	assert(pgtable_remove(pgd, vaddr2) == p);
	pgfree(p);
}
