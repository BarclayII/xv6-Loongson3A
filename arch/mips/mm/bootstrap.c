/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/mm/pgtable.h>
#include <asm/addrspace.h>
#include <string.h>
#include <printk.h>
#include <config.h>

#ifdef CONFIG_HPT


void arch_pgtable_bootstrap(pgd_t kernel_pgd)
{
}

#endif	/* CONFIG_HPT */
