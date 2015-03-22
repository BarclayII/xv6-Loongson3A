/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_MM_VMM_H
#define _ASM_MM_VMM_H

#ifdef CONFIG_HPT
#include <asm/mm/hier/vmm.h>
#endif

#ifdef CONFIG_IPT
#include <asm/mm/inv/vmm.h>
#endif

#include <sys/types.h>

void arch_mm_init(void);
int arch_map_page(arch_mm_t *arch_mm, addr_t vaddr, struct page *p,
    unsigned int perm);
int new_arch_mm(arch_mm_t *arch_mm);
void destroy_arch_mm(arch_mm_t *arch_mm);

#endif
