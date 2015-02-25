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

pgd_t online_hpt[ASID_MAX + 1];

void arch_pgtable_bootstrap(pgd_t kernel_pgd)
{
	memset(online_hpt, 0, sizeof(online_hpt));
	online_hpt[ASID_KERNEL] = kernel_pgd;
	printk("Kernel PGD at %016x registered as ASID %d\r\n",
	    kernel_pgd, ASID_KERNEL);
}

#endif	/* CONFIG_HPT */
