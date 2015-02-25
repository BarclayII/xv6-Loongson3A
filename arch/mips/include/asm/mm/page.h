/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_MM_PAGE_H
#define _ASM_MM_PAGE_H

#include <config.h>
#include <sys/types.h>

/*
 * Two-level page table is nice for 32-bit architectures, but for true 64-bit
 * addressing, as many as 6 levels are needed (like in UltraSPARC).  Luckily,
 * Loongson 3A, as well as many other 64-bit CPUs (like AMD64) only allows
 * 48-bit virtual addressing, which means that a 4-level one is sufficient.
 *
 * Here I'm going to use classic 4KB-sized pages.
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
 *
 * Likewise, we can obtain hierarchy under other page sizes:
 * Page size	Offset	Directory Entry Bits	Levels	Address Bits
 * 4KB		12	9			4	48
 * 16KB		14	11			3	47
 * 64KB		16	13			2	42
 * 256KB	18	15			2	48
 * 1MB		20	17			1	37
 * 4MB		22	19			1	41
 * 16MB		24	21			1	45
 *
 * Looks like 4KB and 256KB pages fully utilize virtual address space bits, and
 * 16KB pages, as well as 16MB pages nearly utilize all bits.  But note that
 * each process have to preserve one page global directory as well as several other
 * page directories.
 */

#define PGSHIFT		12
#define PGSIZE		(1ULL << PTE_OFFSET)

#define PDE_BITS	(PGSHIFT - 3)
#define PDE_MASK	((1ULL << PDE_BITS) - 1ULL)

#ifdef CONFIG_HPT

#define PGD_OFFSET	(PUD_OFFSET + PDE_BITS)
#define PUD_OFFSET	(PMD_OFFSET + PDE_BITS)
#define PMD_OFFSET	(PTE_OFFSET + PDE_BITS)
#define PTE_OFFSET	PGSHIFT

#define PGD_MASK	(PDE_MASK << PGD_OFFSET)
#define PUD_MASK	(PDE_MASK << PUD_OFFSET)
#define PMD_MASK	(PDE_MASK << PMD_OFFSET)
#define PTE_MASK	(PDE_MASK << PTE_OFFSET)
#define PAGE_OFF_MASK	((1ULL << PTE_OFFSET) - 1ULL)

#define PGX(vaddr)	(((vaddr) & PGD_MASK) >> PGD_OFFSET)
#define PUX(vaddr)	(((vaddr) & PUD_MASK) >> PUD_OFFSET)
#define PMX(vaddr)	(((vaddr) & PMD_MASK) >> PMD_OFFSET)
#define PTX(vaddr)	(((vaddr) & PTE_MASK) >> PTE_OFFSET)
#define PAGE_OFF(vaddr)	((vaddr) & PAGE_OFF_MASK)

#ifndef __ASSEMBLER__
typedef ptr_t *pte_t, *pmd_t, *pud_t, *pgd_t;

#define VADDR_SPLIT(vaddr, pgx, pux, pmx, ptx) \
	do { \
		pgx = PGX(vaddr); \
		pux = PUX(vaddr); \
		pmx = PMX(vaddr); \
		ptx = PTX(vaddr); \
	} while (0)

#endif	/* !__ASSEMBLER__ */

#endif	/* !CONFIG_HPT */

#endif
