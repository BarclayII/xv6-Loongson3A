#ifndef _MM_MMAP_H
#define _MM_MMAP_H

#include <asm/mm/page.h>
#include <asm/addrspace.h>
#include <asm/bitops.h>
#include <sys/types.h>
#include <ds/list.h>
#include <mathop.h>

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
	union {
		unsigned long	free_len;
	};
	list_node_t		page_list_node;
};

#define PAGE_RESERVED		0

/*
 * Macros below takes pointers to page structures, not the structure itself.
 */

#define is_page_reserved(p)	atomic_get_bit(PAGE_RESERVED, &((p)->flags))
#define reserve_page(p)		atomic_set_bit(PAGE_RESERVED, &((p)->flags))
#define release_page(p)		atomic_clear_bit(PAGE_RESERVED, &((p)->flags))

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
 * Kernel can directly address physical pages via XKPHY
 */
#define KVADDR_TO_PADDR(kvaddr)	((kvaddr) - KERNBASE)
#define PADDR_TO_KVADDR(paddr)	((paddr) + KERNBASE)
#define KVADDR_TO_PFN(kvaddr)	PADDR_TO_PFN(KVADDR_TO_PADDR(kvaddr))
#define PFN_TO_KVADDR(n)	PADDR_TO_KVADDR(PFN_TO_PADDR(n))
#define KVADDR_TO_PAGE(kvaddr)	PADDR_TO_PAGE(KVADDR_TO_PADDR(kvaddr))
#define PAGE_TO_KVADDR(p)	PADDR_TO_KVADDR(PAGE_TO_PADDR(p))

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
