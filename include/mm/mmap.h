/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _MM_MMAP_H
#define _MM_MMAP_H

#include <asm/mm/page.h>
#include <asm/addrspace.h>
#include <asm/bitops.h>
#include <sys/types.h>
#include <ds/list.h>
#include <mathop.h>
#include <stddef.h>
#include <panic.h>

#define NR_PAGES_NEEDED(bytes)	RSHIFT_ROUNDUP(bytes, PGSHIFT)
#define NR_PAGES_AVAIL(bytes)	RSHIFT_ROUNDDOWN(bytes, PGSHIFT)
#define PAGES_TO_BYTES(n)	((n) << PGSHIFT)

#define BYTES_TO_KB(x)		RSHIFT_ROUNDUP(x, 10)
#define KB_TO_BYTES(x)		((x) << 10)
#define BYTES_TO_MB(x)		RSHIFT_ROUNDUP(x, 20)
#define MB_TO_BYTES(x)		((x) << 20)
#define BYTES_TO_GB(x)		RSHIFT_ROUNDUP(x, 30)
#define GB_TO_BYTES(x)		((x) << 30)
#define BYTES_TO_TB(x)		RSHIFT_ROUNDUP(x, 40)
#define TB_TO_BYTES(x)		((x) << 40)

#define PGADDR_ROUNDDOWN(addr)	POW2_ROUNDDOWN(addr, PGSHIFT)
#define PGADDR_ROUNDUP(addr)	POW2_ROUNDUP(addr, PGSHIFT)

#define MAX_LOW_MEM_MB		256
#define MAX_LOW_MEM		MB_TO_BYTES(MAX_LOW_MEM_MB)

/*
 * Physical page structure
 */
struct page {
	unsigned int	ref_count;
	/* This field should be aligned to 4 bytes, or an address error might
	 * occur. */
	unsigned short	flags;
	/*
	 * flags are represented by bit position index, and are manipulated by
	 * atomic_bit_set/clear/change macros.
	 */
#define PAGE_RESERVED	0
	unsigned short	type;
#define PGTYPE_GENERIC	0	/* generic */
#define PGTYPE_PGSTRUCT	1	/* page structure storage */
#define PGTYPE_PGDIR	2	/* page directory */
#define PGTYPE_SLAB	3	/* kernel object slabs */
	list_node_t	list_node;
	/* shortcut to first page of the allocated set */
	struct page	*first_page;
	/* number of pages belong to the same allocated page set */
	unsigned int	page_count;
	union {
		/* For page directories */
		struct {
			unsigned short	entries;
			unsigned short	asid;
		} pgdir;
	};
};

/*
 * Macros below takes pointers to page structures, not the structure itself.
 */

/*
 * The page array is sorted by physical address.
 */
extern struct page *page_array;

extern size_t highmem_base_pfn;	/* The lowest PFN in high memory */

/*
 * Someday I'll wrap all these macros outside inline functions...
 */
#define PAGE_TO_PFN(p)	\
	((unsigned long)((struct page *)(p) - (page_array)) + (highmem_base_pfn))
#define PAGE_TO_PADDR(p)	(PAGE_TO_PFN(p) << PGSHIFT)
#define PFN_TO_PADDR(n)		((n) << PGSHIFT)
#define PADDR_TO_PFN(addr)	(PGADDR_ROUNDDOWN(addr) >> PGSHIFT)

static inline struct page *__pfn_to_page(unsigned long n)
{
	if ((n) < highmem_base_pfn)
		panic("Trying to access PFN %d through pages?\r\n", n);
	return &(page_array[n - highmem_base_pfn]);
}

#define PFN_TO_PAGE(n)		__pfn_to_page(n)
#define PADDR_TO_PAGE(addr)	PFN_TO_PAGE(PADDR_TO_PFN(addr))

/*
 * Kernel can directly address physical pages via XKPHY
 * NOTE: These macros are only for high memory
 */
#define KVADDR_TO_PADDR(kvaddr)	((kvaddr) - KERNBASE)
#define PADDR_TO_KVADDR(paddr)	((paddr) + KERNBASE)
#define KVADDR_TO_PFN(kvaddr)	PADDR_TO_PFN(KVADDR_TO_PADDR(kvaddr))
#define PFN_TO_KVADDR(n)	PADDR_TO_KVADDR(PFN_TO_PADDR(n))

static inline struct page *__kvaddr_to_page(addr_t kvaddr)
{
	return !kvaddr ? NULL : PADDR_TO_PAGE(KVADDR_TO_PADDR(kvaddr));
}

static inline addr_t __page_to_kvaddr(struct page *p)
{
	return !p ? 0 : PADDR_TO_KVADDR(PAGE_TO_PADDR(p));
}

#define KVADDR_TO_PAGE(kvaddr)	__kvaddr_to_page(kvaddr)
#define PAGE_TO_KVADDR(p)	__page_to_kvaddr(p)

/*
 * Physical page manipulation
 */
#define is_page_reserved(p) \
	atomic_get_bit(PAGE_RESERVED, (unsigned int *)&((p)->flags))
#define reserve_page(p)	\
	atomic_set_bit(PAGE_RESERVED, (unsigned int *)&((p)->flags))
inline void shred_page(struct page *p);
#define release_page(p)	\
	atomic_clear_bit(PAGE_RESERVED, (unsigned int *)&((p)->flags))
#define shred_and_release_page(p) \
	do { \
		shred_page(p); \
		release_page(p); \
	} while (0)

#define inc_pageref(p)		(++((p)->ref_count))
#define dec_pageref(p)		(--((p)->ref_count))
#define page_unref(p)	\
	do { \
		dec_pageref(p); \
		if ((p)->ref_count == 0) \
			pgfree(p); \
	} while (0)

#define node_to_page(node)	member_to_struct(node, struct page, list_node)
#define next_page(p)		node_to_page(list_next(&((p)->list_node)))
#define prev_page(p)		node_to_page(list_prev(&((p)->list_node)))
#define first_free_page()	node_to_page(list_next(free_page_list))
/* Actually not a valid page.  It's just a hack. */
#define end_free_page()		node_to_page(free_page_list)
#define page_add_before(p, newp) \
	list_add_before(&((p)->list_node), &((newp)->list_node))
#define page_add_after(p, np) \
	list_add_after(&((p)->list_node), &((newp)->list_node))
#define page_delete(p)		list_del_init(&((p)->list_node))
#define single_page(p)		((p) == next_page(p))

struct free_page_group {
	list_node_t	head;
	unsigned long	count;
};
extern struct free_page_group free_page_group;
#define free_page_list	(list_node_t *)(&(free_page_group.head))
#define nr_free_pages	(free_page_group.count)

void mm_init(void);
struct page *alloc_pages(size_t num);
struct page *alloc_cont_pages(size_t num);
#define alloc_page()	alloc_pages(1)
#define pgalloc()	alloc_page()
void free_pages(struct page *freep);
#define pgfree(p)	free_pages(p)

/* Test routines */
void test_mm(void);
void test2_mm(void);

#endif
