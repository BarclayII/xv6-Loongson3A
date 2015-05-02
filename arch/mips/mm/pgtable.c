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
#if 0
	arch_mm->asid = ASID_INVALID;
#endif
	return arch_mm_new_pgtable(arch_mm);
}

void destroy_arch_mm(arch_mm_t *arch_mm)
{
	arch_mm_destroy_pgtable(arch_mm);
}

unsigned int page_perm(unsigned long vm_flags)
{
	unsigned int perm = PTE_VALID;
	if (vm_flags & VMA_WRITE)
		perm |= PTE_DIRTY;
	if (!(vm_flags & VMA_EXEC))
		perm |= PTE_NOEXEC;
	if (vm_flags & VMA_COW)
		perm |= PTE_COW;
	return perm;
}

unsigned int vma_perm(unsigned long pte_flags)
{
	unsigned int perm = VMA_READ;
	if (pte_flags & PTE_DIRTY)
		perm |= VMA_WRITE;
	if (!(pte_flags & PTE_NOEXEC))
		perm |= VMA_EXEC;
	if (pte_flags & PTE_COW)
		perm |= VMA_COW;
	return perm;
}
