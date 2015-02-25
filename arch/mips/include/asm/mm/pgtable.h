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

#ifndef __ASSEMBLER__
extern pgd_t online_hpt[];

void arch_pgtable_bootstrap(pgd_t kernel_pgd);
#endif	/* !__ASSEMBLER__ */

#endif	/* CONFIG_HPT */

#endif
