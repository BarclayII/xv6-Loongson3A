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
#include <asm/mm/tlb.h>
#include <asm/mipsregs.h>
#include <mm/mmap.h>
#include <mm/vmm.h>
#include <mm/kmalloc.h>
#include <printk.h>
#include <ds/list.h>
#include <asm/memrw.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

static void test_mm(void)
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
	    i < page_count(p1);
	    ++i, node = list_next(node)) {
		printk("\tAllocated PFN: %d\r\n",
		    PAGE_TO_PFN(node_to_page(node)));
		assert(page_count(node_to_page(node)) == 3);
	}
	printk("First free PFN after alloc: %d\r\n", first_free_pfn);
	p2 = alloc_pages(3);
	for (i = 0, node = &(p2->list_node);
	    i < page_count(p2);
	    ++i, node = list_next(node)) {
		printk("\tAllocated PFN: %d\r\n",
		    PAGE_TO_PFN(node_to_page(node)));
		assert(page_count(node_to_page(node)) == 3);
	}
	printk("First free PFN after 2nd alloc: %d\r\n", first_free_pfn);
	free_all_pages(p1);
	printk("First free PFN after 1st free: %d\r\n", first_free_pfn);
	p3 = alloc_cont_pages(5);
	printk("PFN for p3: %d\r\n", PAGE_TO_PFN(p3));
	for (i = 0, node = &(p3->list_node);
	    i < page_count(p3);
	    ++i, node = list_next(node)) {
		printk("\tAllocated PFN: %d\r\n",
		    PAGE_TO_PFN(node_to_page(node)));
		assert(page_count(node_to_page(node)) == 5);
	}
	printk("First free PFN after 3rd alloc: %d\r\n", first_free_pfn);
	free_all_pages(p2);
	printk("First free PFN after 2nd free: %d\r\n", first_free_pfn);
	free_all_pages(p3);
	printk("First free PFN after 3rd free: %d\r\n", first_free_pfn);
	printk("Current free pages: %d\r\n", nr_free_pages);
}

static void test_mm2(void)
{
	printk("**********test_mm2**********\r\n");
	size_t freepages = nr_free_pages;
	printk("Current free pages: %d\r\n", nr_free_pages);
	struct page *first = alloc_pages(20);
	struct page *base, *last;
	int i;
	printk("After allocation: %d\r\n", nr_free_pages);
	for (i = 0, base = first; i < 6; ++i, base = next_page(base))
		/* nothing */;
	last = free_pages(base, 7);
	printk("After freeing middle pages: %d\r\n", nr_free_pages);
	assert(page_count(first) == 6);
	assert(page_count(last) == 7);
	free_pages(last, 7);
	printk("After freeing tail pages: %d\r\n", nr_free_pages);
	first = free_pages(first, 1);
	printk("After freeing first single page: %d\r\n", nr_free_pages);
	first = free_pages(prev_page(first), 1);
	printk("After freeing last single page: %d\r\n", nr_free_pages);
	free_all_pages(first);
	printk("After freeing all pages: %d\r\n", nr_free_pages);
	assert(nr_free_pages == freepages);
}

static void test_pgtable(void)
{
	printk("**********test_pgtable**********\r\n");
	pgd_t pgd = kern_mm.arch_mm.pgd;
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

static void test_tlb(void)
{
	printk("**********test_tlb**********\r\n");
	pgd_t *pgd = &(kern_mm.arch_mm.pgd);
	struct page *p1 = pgalloc(), *p2 = pgalloc(), *p3 = pgalloc();
	unsigned long pfn1 = PAGE_TO_PFN(p1), pfn2 = PAGE_TO_PFN(p2);
	struct pagedesc pdesc1, pdesc2;
	volatile unsigned long *a = (volatile unsigned long *)0x500000;
	volatile unsigned long *b = (volatile unsigned long *)0x504000;

	printk("PFN1 = %016x\r\n", pfn1);
	printk("PFN2 = %016x\r\n", pfn2);
	pgtable_insert(pgd, (addr_t)a, p1, PTE_VALID | PTE_DIRTY,
	    false, NULL);
	pgtable_insert(pgd, (addr_t)b, p2, PTE_VALID | PTE_DIRTY,
	    false, NULL);
	pgtable_get(pgd, (addr_t)a, false, &pdesc1);
	pgtable_get(pgd, (addr_t)b, false, &pdesc2);
	dump_pagedesc((addr_t)a, &pdesc1);
	dump_pagedesc((addr_t)b, &pdesc2);
	printk("PGD1 = %016x\r\n", pdesc1.pgd[pdesc1.pgx]);
	printk("PMD1 = %016x\r\n", pdesc1.pmd[pdesc1.pmx]);
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

	assert(pgtable_remove(pgd, (addr_t)a) == p1);
#if 0
	printk("TEST: %016x\r\n", *b);	/* should not panic */
	printk("TEST: %016x\r\n", *a);	/* should panic */
#endif
	assert(pgtable_remove(pgd, (addr_t)b) == p2);
#if 0
	printk("TEST: %016x\r\n", *a);	/* should panic */
	printk("TEST: %016x\r\n", *b);	/* should panic */
#endif
	printk("Inserting back...\r\n");
	pgtable_insert(pgd, (addr_t)a, p1, PTE_VALID,
	    false, NULL);
	pgtable_get(pgd, (addr_t)a, false, &pdesc1);
	dump_pagedesc((addr_t)a, &pdesc1);
	assert(*a == pa);
#if 0
	printk("TEST: %016x\r\n", *b);	/* should panic */
#endif
	assert(pgtable_remove(pgd, (addr_t)a) == p1);

#undef first_free_pfn

	pgfree(p1);
	pgfree(p2);
	pgfree(p3);

	printk("After freeing: %d %016x  %d %016x\r\n",
	    PAGE_TO_PFN(p1), read_mem_ulong(PAGE_TO_KVADDR(p1)),
	    PAGE_TO_PFN(p2), read_mem_ulong(PAGE_TO_KVADDR(p2)));
}

static void test_shm(void)
{
	printk("**********test_shm**********\r\n");

	pgd_t *pgd = &(kern_mm.arch_mm.pgd);
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

/* randomized slab test */
#define NR_SLAB_TESTS	32
static void test_slab(void)
{
	printk("**********test_slab**********\r\n");
	printk("Free pages before test_slab(): %d\r\n", nr_free_pages);
	void *ptr[NR_SLAB_TESTS];
	size_t size[NR_SLAB_TESTS];
	int seq[2 * NR_SLAB_TESTS], state[NR_SLAB_TESTS];
	memset(state, 0, sizeof(state));
	int i;
	for (i = 0; i < NR_SLAB_TESTS; ++i) {
		if ((read_c0_random() & 7) == 0)
			size[i] = 1 + rand() % 0x10000;
		else
			size[i] = 1 + rand() % 0x100;
		seq[i * 2] = seq[i * 2 + 1] = i;
	}
	printk("Allocating object sizes:\r\n");
	for (i = 0; i < NR_SLAB_TESTS; ++i) {
		printk("%u ", size[i]);
		if (i % 8 == 7)
			printk("\r\n");
	}
	printk("\r\n");
	for (i = 0; i < NR_SLAB_TESTS * 2 - 1; ++i) {
		int t = seq[i];
		int u = i + rand() % (2 * NR_SLAB_TESTS - i);
		seq[i] = seq[u];
		seq[u] = t;
	}
	printk("Alloc-free sequence:\r\n");
	for (i = 0; i < NR_SLAB_TESTS * 2; ++i) {
		printk("%d ", seq[i]);
		if (i % 8 == 7)
			printk("\r\n");
	}
	memset(ptr, 0, sizeof(ptr));
	for (i = 0; i < NR_SLAB_TESTS * 2; ++i) {
		++state[seq[i]];
		assert(state[seq[i]] <= 2);
		if (ptr[seq[i]]) {
			pdebug("FREEING OBJECT %d WITH SIZE %d\r\n",
			    seq[i], size[seq[i]]);
			kfree(ptr[seq[i]]);
		} else {
			pdebug("ALLOCATING OBJECT %d WITH SIZE %d\r\n",
			    seq[i], size[seq[i]]);
			ptr[seq[i]] = kmalloc(size[seq[i]]);
		}
	}
	printk("Allocated addresses:\r\n");
	for (i = 0; i < NR_SLAB_TESTS; ++i)
		printk("%016x\r\n", ptr[i]);
	printk("Free pages after test_slab(): %d\r\n", nr_free_pages);
}

/* free index list/stack handling */
static void test_slab2(void)
{
	printk("**********test_slab2()**********\r\n");
	ptr_t ptr0, ptr1, ptr2, ptr3;
	ptr0 = kmalloc(32);
	ptr1 = kmalloc(32);
	ptr2 = kmalloc(32);
	kfree(ptr1);
	kfree(ptr2);
	ptr1 = kmalloc(32);
	ptr2 = kmalloc(32);
	ptr3 = kmalloc(32);
	assert(ptr2 < ptr1);
	assert(ptr1 < ptr3);
	kfree(ptr0);
	kfree(ptr1);
	kfree(ptr2);
	kfree(ptr3);
	printk("succeeded\r\n");
}

/* available & full queue handling */
static void test_slab3(void)
{
	printk("**********test_slab3()**********\r\n");
	ptr_t ptr[5];
	int i;
	for (i = 0; i < 5; ++i) {
		ptr[i] = kmalloc(PGSIZE >> 1);
		printk("%016x\r\n", ptr[i]);
	}
	kfree(ptr[1]);
	kfree(ptr[3]);
	ptr[1] = kmalloc(PGSIZE >> 1);
	ptr[3] = kmalloc(PGSIZE >> 1);
	printk("%016x\r\n", ptr[1]);
	printk("%016x\r\n", ptr[3]);
	kfree(ptr[0]);
	kfree(ptr[1]);
	kfree(ptr[2]);
	kfree(ptr[3]);
	ptr[0] = kmalloc(PGSIZE >> 1);
	ptr[1] = kmalloc(PGSIZE >> 1);
	ptr[2] = kmalloc(PGSIZE >> 1);
	printk("%016x %016x %016x\r\n", ptr[0], ptr[1], ptr[2]);
	kfree(ptr[0]);
	kfree(ptr[1]);
	kfree(ptr[2]);
	kfree(ptr[4]);
	printk("succeeded\r\n");
}

void test_uvm(void)
{
	printk("**********test_uvm**********\r\n");
	mm_t *mm = mm_new();
	assert(mm != NULL);
	printk("PGD = %016x\r\n", mm->arch_mm.pgd);

	int ret;
	char *msg = "Hello from UVM\r\n";
	char msg2[20];

	ret = mm_create_uvm(mm, (void *)0x400000, 5 * PGSIZE,
	    VMA_READ | VMA_WRITE);
	assert(ret == 0);
	assert(!unmap_pages(mm, 0x400000 + 2 * PGSIZE, 1));
	assert(!copy_to_uvm(mm, (void *)0x400080, msg, strlen(msg) + 1));
	assert(!copy_from_uvm(mm, (void *)0x400080, msg2, strlen(msg) + 1));
	printk("%s", msg2);
	assert(strcmp(msg, msg2) == 0);
	assert(!mm_destroy_uvm(mm, (void *)0x400000));
	assert(!mm_destroy_uvm(mm, (void *)(0x400000 + 3 * PGSIZE)));

	mm_destroy(mm);
	printk("free pages: %d\r\n", nr_free_pages);
}

void mm_test(void)
{
	test_mm();
	test_mm2();

	test_pgtable();
	test_tlb();
	test_shm();

	test_slab();
	test_slab2();
	test_slab3();

	test_uvm();
}
