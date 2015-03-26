/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_MM_PGTABLE_H
#define _ASM_MM_PGTABLE_H

#include <config.h>
#include <asm/mm/page.h>
#include <asm/addrspace.h>

#ifdef CONFIG_HPT

/*
 * HPT structure goes as follows:
 *
 * The entries for all directories except leaf page tables are kernel virtual
 * addresses pointing to next-level directories:
 * 6666555555555544444444443333333333222222222211111111110000000000
 * 3210987654321098765432109876543210987654321098765432109876543210
 * +----------Virtual Address aligned to page size (4K)-----------+
 *
 * While the leaf page tables stores PFN and additional information in the
 * following format:
 * 6666555555555544444444443333333333222222222211111111 110 0 0 0 000000
 * 3210987654321098765432109876543210987654321098765432 109 8 7 6 543210
 * +-------0------++------Physical Frame Number-------+ CCF D V G 00X00P
 *
 * The two types of entries could be distinguished by checking the P bit.
 *
 * Refilling the TLB is then simple: we dive into the HPT and fetch the
 * page table entry, right rotate the value by 6 bits, and then pass the
 * results into CP0.
 */

#define PTE_LOWMASK		0xfff
/* These bits match the mode bits in TLB entries */
#define PTE_CACHE_MASK		0xe00
# define PTE_CACHEABLE		0x600
# define PTE_UNCACHED		0x400
#define PTE_DIRTY		0x100
#define PTE_VALID		0x080
#define PTE_GLOBAL		0x040
/* Loongson 2F has this bit for buffer overflow protection.  Not sure whether
 * Loongson 3A has it. */
#define PTE_NOEXEC		0x008
/* Extra page table entry flags not needed by hardware */
#define PTE_SOFT_SHIFT		6
#define PTE_PHYS		0x001	/* physical address marker */

#ifndef __ASSEMBLER__

#ifdef CONFIG_HPT
#include <asm/mm/hier/pgtable.h>
#endif

void asid_flush(void);

/* Determine page table flags from virtual memory area flags */
unsigned int page_perm(unsigned long vm_flags);

struct arch_mm_struct;
struct page;
struct trapframe;

int arch_map_page(struct arch_mm_struct *arch_mm, addr_t vaddr,
    struct page *p, unsigned int perm);
struct page *arch_unmap_page(struct arch_mm_struct *arch_mm, addr_t vaddr);
unsigned long arch_mm_get_pfn(struct arch_mm_struct *arch_mm, addr_t vaddr);
int arch_mm_new_pgtable(struct arch_mm_struct *arch_mm);
void arch_mm_destroy_pgtable(struct arch_mm_struct *arch_mm);

int handle_pgfault(struct trapframe *tf);
#endif	/* !__ASSEMBLER__ */

#endif	/* CONFIG_HPT */

#endif
