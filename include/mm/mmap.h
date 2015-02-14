#ifndef _MM_MMAP_H
#define _MM_MMAP_H

#include <asm/mm/page.h>
#include <asm/addrspace.h>
#include <sys/types.h>
#include <ds/list.h>
#include <mathop.h>

#define NR_PAGES(bytes)		RSHIFT_ROUNDUP(bytes, PGSHIFT)

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

struct page {
	size_t		ref_count;
	unsigned int	reserved;
	list_node_t	page_list_node;
};

struct page_list {
	list_node_t	*head;
	size_t		count;
};

extern struct page_list free_page_list;

extern size_t highmem_base_pfn;	/* The lowest PFN in high memory */

/*
 * The page array is sorted by physical address.
 */
extern struct page *page_array;

#define PAGE_TO_PFN(p)		((p) - (page_array) + (highmem_base_pfn))
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
 * I'm going to directly map every physical page to XKSEG
 */
#define XKSEG_TO_PADDR(vaddr)	((vaddr) - XKSEG)
#define PADDR_TO_XKSEG(paddr)	((paddr) + XKSEG)
#define XKSEG_TO_PFN(vaddr)	PADDR_TO_PFN(XKSEG_TO_PADDR(vaddr))
#define PFN_TO_XKSEG(n)		PADDR_TO_XKSEG(PFN_TO_PADDR(n))

#ifdef CONFIG_INVERTED_PAGETABLE

struct invert_pte {
	unsigned short	pid;
	unsigned char	asid;
	unsigned char	flags;
	unsigned int	reserved;
	uint64		vpn;
	uint64		hash_next;
};

#endif /* CONFIG_INVERTED_PAGETABLE */

void mm_init(void);

#endif
