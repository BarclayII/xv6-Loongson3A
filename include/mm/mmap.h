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
	size_t			ref_count;
	unsigned int		flags;
	list_node_t		list_node;
};

#define PAGE_RESERVED		0

/*
 * Macros below takes pointers to page structures, not the structure itself.
 */

#define is_page_reserved(p)	atomic_get_bit(PAGE_RESERVED, &((p)->flags))
#define reserve_page(p)		atomic_set_bit(PAGE_RESERVED, &((p)->flags))
#define release_page(p)		atomic_clear_bit(PAGE_RESERVED, &((p)->flags))

#define list_node_to_page(node)	member_to_struct(node, struct page, list_node)

struct free_page_group {
	list_node_t	head;
	unsigned long	count;
};
extern struct free_page_group free_pages;
#define free_page_list	(list_node_t *)(&(free_pages.head))
#define nr_free_pages	(free_pages.count)

extern size_t highmem_base_pfn;	/* The lowest PFN in high memory */

/*
 * The page array is sorted by physical address.
 */
extern struct page *page_array;

#define PAGE_TO_PFN(p)	\
	((unsigned long)((struct page *)(p) - (page_array)) + (highmem_base_pfn))
#define PAGE_TO_PADDR(p)	(PAGE_TO_PFN(p) << PGSHIFT)
#define PFN_TO_PADDR(n)		((n) << PGSHIFT)
#define PADDR_TO_PFN(addr)	(PGADDR_ROUNDDOWN(addr) >> PGSHIFT)
#define PFN_TO_PAGE(n)		({ \
	if ((n) < highmem_base_pfn) { \
		panic("Trying to access low memory through pages?\r\n"); \
	} \
	page_array[(n) - highmem_base_pfn]; \
})
#define PADDR_TO_PAGE(addr)	PFN_TO_PAGE(PADDR_TO_PFN(addr))

/*
 * Kernel can directly address physical pages via XKPHY
 */
#define KVADDR_TO_PADDR(kvaddr)	((kvaddr) - KERNBASE)
#define PADDR_TO_KVADDR(paddr)	((paddr) + KERNBASE)
#define KVADDR_TO_PFN(kvaddr)	PADDR_TO_PFN(KVADDR_TO_PADDR(kvaddr))
#define PFN_TO_KVADDR(n)	PADDR_TO_KVADDR(PFN_TO_PADDR(n))
#define KVADDR_TO_PAGE(kvaddr)	PADDR_TO_PAGE(KVADDR_TO_PADDR(kvaddr))
#define PAGE_TO_KVADDR(p)	PADDR_TO_KVADDR(PAGE_TO_PADDR(p))

void mm_init(void);

#ifdef CONFIG_INVERTED_PAGETABLE

struct inv_pgtable_entry {
	unsigned int	pid;
	unsigned char	asid;
	unsigned char	flags;
	unsigned short	reserved;
	unsigned long	vpn;
	unsigned long	hash_next;	/* next index for the same hash */
};

extern struct inv_pgtable_entry *inv_pgtable;

#else	/* !CONFIG_INVERTED_PAGETABLE */

extern pgd_t boot_pgd;		/* Bootstrap page global directory */

#endif	/* CONFIG_INVERTED_PAGETABLE */

#endif
