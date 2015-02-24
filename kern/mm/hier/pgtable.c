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

#include <mm/hier/pgdir.h>
#include <mm/mmap.h>
#include <mm/vmm.h>
#include <printk.h>
#include <stddef.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

mm_t kern_high_mm;
mm_t kern_low_mm;

/*
 * Allocate a page global directory for kernel, and initialize the memory
 * manager.
 */
void pgtable_bootstrap(void)
{
	kern_mm.asid = 0;
	kern_mm.vma_head = NULL;
	kern_mm.pgd = (pgd_t)PAGE_TO_KVADDR(pgdir_new());
	printk("Kernel page global directory initialized at %016x\r\n",
	    kern_mm.pgd);
}

static void dump_pagedesc(ptr_t vaddr, struct pagedesc *pdesc)
{
	printk("VADDR\t%016x\r\n", vaddr);
	printk("PGD\t%016x:%d\r\n", pdesc->pgd, pdesc->pgx);
	printk("PUD\t%016x:%d\r\n", pdesc->pud, pdesc->pux);
	printk("PMD\t%016x:%d\r\n", pdesc->pmd, pdesc->pmx);
	printk("PTE\t%016x:%d\r\n", pdesc->pte, pdesc->ptx);
}

/*
 * See include/mm/vmm.h for meanings of parameters.
 * @pgtable is a pointer of pgd_t, while @result is another pointer to
 * pagedesc structure.
 */
int pgtable_get(void *pgtable, ptr_t vaddr, bool create, void *result)
{
	struct pagedesc pdesc;
	int ret = 0;
	memset(&pdesc, 0, sizeof(struct pagedesc));
	pdesc.pgd = *(pgd_t *)pgtable;

	VADDR_SPLIT(vaddr, pdesc.pgx, pdesc.pux, pdesc.pmx, pdesc.ptx);

	if (!create) {
		if (!(pdesc.pud = pdesc.pgd[pdesc.pgx]) ||
		    !(pdesc.pmd = pdesc.pud[pdesc.pux]) ||
		    !(pdesc.pte = pdesc.pmd[pdesc.pmx]))
			ret = -ENOENT;
	} else {
		if (!(pdesc.pud = pdesc.pgd[pdesc.pgx]))
			pdesc.pud = (pud_t)pde_add_page(pdesc.pgd, pdesc.pgx);
		if (!(pdesc.pmd = pdesc.pud[pdesc.pux]))
			pdesc.pmd = (pmd_t)pde_add_page(pdesc.pud, pdesc.pux);
		if (!(pdesc.pte = pdesc.pmd[pdesc.pmx]))
			pdesc.pte = (pte_t)pde_add_page(pdesc.pmd, pdesc.pmx);
	}

	printk("pgtable_get(%016x):\r\n", vaddr);
	dump_pagedesc(vaddr, &pdesc);
	memcpy(result, &pdesc, sizeof(struct pagedesc));
	return ret;
}

/*
 * Fails only if the virtual address already exists and @replace is not set.
 */
int pgtable_insert(void *pgtable, ptr_t vaddr, struct page *page, bool replace,
    struct page **replaced_page)
{
	struct pagedesc pdesc;
	struct page *p;

	pgtable_get(pgtable, vaddr, true, &pdesc);

	if (pdesc.pte[pdesc.ptx]) {
		if (!replace)
			return -EEXIST;
		else {
			printk("WARNING: replacing existing PT entry\r\n");
			printk("PGD = %016x, VADDR = %016x\r\n",
			    pdesc.pgd, vaddr);
			printk("OLD = %016x, NEW = %016x\r\n",
			    pdesc.pte[pdesc.ptx], PAGE_TO_KVADDR(page));
			p = KVADDR_TO_PAGE(pdesc.pte[pdesc.ptx]);
			pde_remove_entry(pdesc.pte, pdesc.ptx);
			if (replaced_page == NULL) {
				printk("WARNING: replaced_page = NULL\r\n");
			} else {
				*replaced_page = p;
			}
		}
	}

	pde_add_entry(pdesc.pte, pdesc.ptx, page);
	return 0;
}

struct page *pgtable_remove(void *pgtable, ptr_t vaddr)
{
	struct pagedesc pdesc;
	struct page *p;

	pgtable_get(pgtable, vaddr, false, &pdesc);
	p = KVADDR_TO_PAGE(pdesc.pte[pdesc.ptx]);

	if (pdesc.pte[pdesc.ptx])
		pde_remove_entry(pdesc.pte, pdesc.ptx);
	
	if (pde_empty(pdesc.pte))
		pde_remove_page(pdesc.pmd, pdesc.pmx);

	if (pde_empty(pdesc.pmd))
		pde_remove_page(pdesc.pud, pdesc.pux);

	if (pde_empty(pdesc.pud))
		pde_remove_page(pdesc.pgd, pdesc.pgx);

	return p;
}

#endif	/* CONFIG_HPT */
