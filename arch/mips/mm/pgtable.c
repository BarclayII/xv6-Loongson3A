/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <mm/vmm.h>
#include <mm/mmap.h>
#include <sys/types.h>

int new_arch_mm(arch_mm_t *arch_mm)
{
	return 0;
}

void destroy_arch_mm(arch_mm_t *arch_mm)
{
}

unsigned int page_perm(unsigned long vm_flags)
{
	return (vm_flags & VMA_WRITE) ? PTE_VALID|PTE_DIRTY : PTE_VALID;
}

