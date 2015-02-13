#ifndef _MM_PAGE_H
#define _MM_PAGE_H

#include <sys/types.h>

/*
 * I first thought about using inverted page tables to manage memory, but
 * maybe that's too difficult.  So I decided to continue use hierarchical
 * page table structure.
 *
 * Two-level page table is nice for 32-bit architectures, but for true 64-bit
 * addressing, as many as 6 levels are needed (like in UltraSPARC).  Luckily,
 * Loongson 3A, as well as many other 64-bit CPUs (like AMD64) only allows
 * 48-bit virtual addressing, which means that a 4-level one is sufficient.
 *
 * Page size is still 4KB since allocating 16MB for a process just for a global
 * address directory is a waste of space.  Besides, even in case of 16MB pages,
 * we still need a two-level hierarchy.
 *
 * In 64-bit architecture, the size of one page directory entry should be 9 bits
 * for fitting one page directory into a page, because 3 bits are needed for
 * storing 8-byte addresses.
 *
 * Therefore, we can obtain the following paging model:
 * 6               4        3        2        2        1          0
 * 3               7        8        9        0        1          0
 * +---------------+--------+--------+--------+--------+----------+
 * |       0       |  PGD   |  PUD   |  PMD   |   PT   |  OFFSET  |
 * +---------------+--------+--------+--------+--------+----------+
 * 0th - 11th	: Page offset
 * 12th - 20th	: Page Table Index
 * 21st - 29th	: Page Middle Directory Index
 * 30th - 38th	: Page Upper Directory Index
 * 39th - 47th	: Page Global Directory Index
 * 48th - 63rd	: Unused
 */

typedef ptr_t pgd_t, pud_t, pmd_t, pte_t;

#define PDE_MASK	0x1ffULL		/* Page directory entry bits */

#define PGD_OFFSET	39
#define PUD_OFFSET	30
#define PMD_OFFSET	21
#define PTE_OFFSET	12

#define PGD_MASK	(PDE_MASK << PGD_OFFSET)
#define PUD_MASK	(PDE_MASK << PUD_OFFSET)
#define PMD_MASK	(PDE_MASK << PMD_OFFSET)
#define PTE_MASK	(PDE_MASK << PTE_OFFSET)
#define PAGE_OFF_MASK	((1ULL << PTE_OFFSET) - 1ULL)

#define PGD(vaddr)	(((vaddr) & PGD_MASK) >> PGD_OFFSET)
#define PUD(vaddr)	(((vaddr) & PUD_MASK) >> PUD_OFFSET)
#define PMD(vaddr)	(((vaddr) & PMD_MASK) >> PMD_OFFSET)
#define PTE(vaddr)	(((vaddr) & PTE_MASK) >> PTE_OFFSET)
#define PAGE_OFF(vaddr)	((vaddr) & PAGE_OFF_MASK)

#endif
