/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_MM_TLBOPS_H
#define _ASM_MM_TLBOPS_H

#include <asm/addrspace.h>
#include <asm/mm/page.h>
#include <sys/types.h>

#define tlbp()		asm volatile ("tlbp")
#define tlbr()		asm volatile ("tlbr")
#define tlbwr()		asm volatile ("tlbwr")
#define tlbwi()		asm volatile ("tlbwi")

/*
 * IMPORTANT NOTE:
 * MIPS TLB doesn't allow duplicate virtual pages, so one should always
 * clean TLB entries by filling ENTRYHI with this macro.
 *
 * Side Note:
 * Loongson 3A is fine with such situation but results in undefined behavior.
 * Other MIPS CPUs may shutdown TLB, or, in the extreme, HCF(?).
 */
#define ENTRYHI_DUMMY(idx) (CKSEG0 + ((idx) << (PGSHIFT + 1)))

void tlb_flush_all(void);
void tlb_remove(ptr_t vaddr);

#endif
