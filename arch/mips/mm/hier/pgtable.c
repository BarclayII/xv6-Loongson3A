/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <config.h>

#ifdef CONFIG_HPT

#include <asm/cache.h>
#include <asm/mm/tlb.h>
#include <asm/mm/pgtable.h>
#include <asm/mm/hier/pgdir.h>
#include <mm/mmap.h>
#include <mm/vmm.h>
#include <printk.h>
#include <stddef.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

mm_t kern_high_mm;
mm_t kern_low_mm;

pgd_t online_hpt[ASID_MAX + 1];

/*
 * Allocate a page global directory for kernel, and initialize the memory
 * manager.
 */
void pgtable_bootstrap(void)
{
	kern_mm.asid = ASID_KERNEL;
	kern_mm.vma_head = NULL;
	kern_mm.pgd = (pgd_t)PAGE_TO_KVADDR(pgdir_new(ASID_KERNEL));
	printk("Kernel page global directory initialized at %016x\r\n",
	    kern_mm.pgd);

	memset(online_hpt, 0, sizeof(online_hpt));
	online_hpt[ASID_KERNEL] = kern_mm.pgd;
	printk("Kernel PGD at %016x registered as ASID %d\r\n",
	    kern_mm.pgd, ASID_KERNEL);
}

void dump_pagedesc(ptr_t vaddr, struct pagedesc *pdesc)
{
	printk("VADDR\t%016x\r\n", vaddr);
	printk("PGD\t%016x:%d\r\n", pdesc->pgd, pdesc->pgx);
#ifndef CONFIG_3LEVEL_PT
	printk("PUD\t%016x:%d\r\n", pdesc->pud, pdesc->pux);
#endif
	printk("PMD\t%016x:%d\r\n", pdesc->pmd, pdesc->pmx);
	printk("PTE\t%016x:%d\r\n", pdesc->pte, pdesc->ptx);
}

/*
 * See include/mm/vmm.h for meanings of parameters.
 * @pgtable is a pointer of pgd_t, while @result is another pointer to
 * pagedesc structure.
 *
 * @vaddr needn't be aligned to page size; it is automatically rounded
 * down to page size within the VADDR_SPLIT macro.
 */
int pgtable_get(void *pgtable, ptr_t vaddr, bool create, void *result)
{
	struct pagedesc pdesc;
	int ret = 0;
	memset(&pdesc, 0, sizeof(struct pagedesc));
	pdesc.pgd = *(pgd_t *)pgtable;

	VADDR_SPLIT(vaddr, pdesc);

	if (!create) {
		if (
#ifdef CONFIG_3LEVEL_PT
		    !(pdesc.pmd = pde_get_entry(pdesc.pgd, pdesc.pgx)) ||
#else
		    !(pdesc.pud = pde_get_entry(pdesc.pgd, pdesc.pgx)) ||
		    !(pdesc.pmd = pde_get_entry(pdesc.pud, pdesc.pux)) ||
#endif
		    !(pdesc.pte = pde_get_entry(pdesc.pmd, pdesc.pmx)))
			ret = -ENOENT;
	} else {
#ifdef CONFIG_3LEVEL_PT
		if (!(pdesc.pmd = pde_get_entry(pdesc.pgd, pdesc.pgx)))
			pdesc.pmd = (pmd_t)pde_add_pgdir(pdesc.pgd, pdesc.pgx);
#else
		if (!(pdesc.pud = pde_get_entry(pdesc.pgd, pdesc.pgx)))
			pdesc.pud = (pud_t)pde_add_pgdir(pdesc.pgd, pdesc.pgx);
		if (!(pdesc.pmd = pde_get_entry(pdesc.pud, pdesc.pux)))
			pdesc.pmd = (pmd_t)pde_add_pgdir(pdesc.pud, pdesc.pux);
#endif
		if (!(pdesc.pte = pde_get_entry(pdesc.pmd, pdesc.pmx)))
			pdesc.pte = (pte_t)pde_add_pgdir(pdesc.pmd, pdesc.pmx);
	}

	memcpy(result, &pdesc, sizeof(struct pagedesc));
	return ret;
}

/*
 * Fails only if the virtual address already exists and @replace is not set.
 */
int pgtable_insert(void *pgtable, ptr_t vaddr, struct page *page,
    unsigned int perm, bool replace, struct page **replaced_page)
{
	struct pagedesc pdesc;
	struct page *p;

	/* Filter NULL address */
	if (vaddr == 0)
		return -EINVAL;

	vaddr = PGADDR_ROUNDDOWN(vaddr);
	pgtable_get(pgtable, vaddr, true, &pdesc);

	if (pdesc.pte[pdesc.ptx] && (pdesc.pte[pdesc.ptx] != vaddr)) {
		if (!replace)
			return -EEXIST;
		else {
			printk("WARNING: replacing existing PT entry\r\n");
			p = KVADDR_TO_PAGE(pde_remove_entry(pdesc.pte,
			    pdesc.ptx));
			printk("PGD = %016x, VADDR = %016x\r\n",
			    pdesc.pgd, vaddr);
			printk("OLD = %016x, NEW = %016x\r\n",
			    PAGE_TO_KVADDR(p), PAGE_TO_KVADDR(page));
			if (replaced_page == NULL) {
				printk("WARNING: replaced_page = NULL\r\n");
			} else {
				*replaced_page = p;
			}
		}
	}

	unsigned int flags = perm & (PTE_LOWMASK - PTE_CACHE_MASK);
	flags |= PTE_CACHEABLE | PTE_PHYS;
	pde_add_entry(pdesc.pte, pdesc.ptx, page, flags);
	return 0;
}

struct page *pgtable_remove(void *pgtable, ptr_t vaddr)
{
	struct pagedesc pdesc;
	struct page *p = NULL;

	pgtable_get(pgtable, vaddr, false, &pdesc);

	/*pdebug("Removing PTE for %016x\r\n", vaddr);*/
	
	if (pdesc.pte[pdesc.ptx]) {
		p = KVADDR_TO_PAGE(pde_remove_entry(pdesc.pte, pdesc.ptx));
	}
	
	if (pde_empty(pdesc.pte)) {
		pde_remove_pgdir(pdesc.pmd, pdesc.pmx);
	}

#ifdef CONFIG_3LEVEL_PT
	if (pde_empty(pdesc.pmd)) {
		pde_remove_pgdir(pdesc.pgd, pdesc.pgx);
	}
#else
	if (pde_empty(pdesc.pmd)) {
		pde_remove_pgdir(pdesc.pud, pdesc.pux);
	}

	if (pde_empty(pdesc.pud)) {
		pde_remove_pgdir(pdesc.pgd, pdesc.pgx);
	}
#endif

	/* Blast TLB entries containing this virtual address away */
	tlb_remove(vaddr);

	return p;
}

#endif	/* CONFIG_HPT */
