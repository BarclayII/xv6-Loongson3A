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
#include <sync.h>
#include <sched.h>

mm_t kern_high_mm;
mm_t kern_low_mm;

struct asid_task_set asid_task_set;

void asid_flush(void)
{
	/* Unregister all online HPTs */
	memset(current_online_hpt, 0, sizeof(current_online_hpt));
	current_next_asid = ASID_MIN;
	current_online_hpt[ASID_KERNEL] = kern_mm.arch_mm.pgd;
	/* Revert all tasks with valid ASIDs */
	int i;
	for (i = ASID_MIN; i < ASID_MAX; ++i) {
		if (current_online_tasks[i] != NULL) {
			current_online_tasks[i]->asid = ASID_INVALID;
			current_online_tasks[i] = NULL;
		}
	}
}

/*
 * Allocate a page global directory for kernel, and initialize the memory
 * mapping.
 */
void pgtable_bootstrap(void)
{
	kern_mm.arch_mm.pgd = (pgd_t)PAGE_TO_KVADDR(pgdir_new(ASID_KERNEL));
	printk("Kernel page global directory initialized at %016x\r\n",
	    kern_mm.arch_mm.pgd);
	/* Have to clear current_online_tasks first to prevent asid_flush()
	 * exhibiting weird behavior */
	memset(current_online_tasks, 0, sizeof(current_online_tasks));

	asid_flush();

	printk("Kernel PGD at %016x registered as ASID %d\r\n",
	    kern_mm.arch_mm.pgd, ASID_KERNEL);
}

void dump_pagedesc(addr_t vaddr, struct pagedesc *pdesc)
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
int pgtable_get(void *pgtable, addr_t vaddr, bool create, void *result)
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
		if (!(pdesc.pmd = pde_get_entry(pdesc.pgd, pdesc.pgx))) {
			pdesc.pmd = (pmd_t)pde_add_pgdir(pdesc.pgd, pdesc.pgx);
			if (pdesc.pmd == NULL)
				goto nomem;
		}
#else
		if (!(pdesc.pud = pde_get_entry(pdesc.pgd, pdesc.pgx))) {
			pdesc.pud = (pud_t)pde_add_pgdir(pdesc.pgd, pdesc.pgx);
			if (pdesc.pud == NULL)
				goto nomem;
		}
		if (!(pdesc.pmd = pde_get_entry(pdesc.pud, pdesc.pux))) {
			pdesc.pmd = (pmd_t)pde_add_pgdir(pdesc.pud, pdesc.pux);
			if (pdesc.pmd == NULL)
				goto nomem;
		}
#endif
		if (!(pdesc.pte = pde_get_entry(pdesc.pmd, pdesc.pmx))) {
			pdesc.pte = (pte_t)pde_add_pgdir(pdesc.pmd, pdesc.pmx);
			if (pdesc.pte == NULL)
				goto nomem;
		}
	}
	goto ret;

nomem:
	ret = -ENOMEM;
	if (pdesc.pmd && pde_get_entry(pdesc.pmd, pdesc.pmx))
		pde_remove_pgdir(pdesc.pmd, pdesc.pmx);
#ifndef CONFIG_3LEVEL_PT
	if (pdesc.pud && pde_get_entry(pdesc.pud, pdesc.pux))
		pde_remove_pgdir(pdesc.pud, pdesc.pux);
#endif
	if (pdesc.pgd && pde_get_entry(pdesc.pgd, pdesc.pgx))
		pde_remove_pgdir(pdesc.pgd, pdesc.pgx);

ret:
	memcpy(result, &pdesc, sizeof(struct pagedesc));
	return ret;
}

/*
 * Fails only if the virtual address already exists and @replace is not set.
 */
int pgtable_insert(void *pgtable, addr_t vaddr, struct page *page,
    unsigned int perm, bool replace, struct page **replaced_page)
{
	struct pagedesc pdesc;
	struct page *p;
	int retcode;

	/* Filter NULL address */
	vaddr = PGADDR_ROUNDDOWN(vaddr);
	if (vaddr == 0)
		return -EINVAL;

	/* TODO: acquire per-page-table lock. 
	 * Actually, this should be a per-mm_struct lock for page tables.
	 * Some day I'll move it elsewhere. */

	if ((retcode = pgtable_get(pgtable, vaddr, true, &pdesc)) != 0)
		return retcode;

	if (pdesc.pte[pdesc.ptx] &&
	    PADDR_TO_PAGE(pdesc.pte[pdesc.ptx]) != page) {
		if (!replace)
			return -EEXIST;
		else {
			pdebug("WARNING: replacing existing PT entry\r\n");
			p = KVADDR_TO_PAGE(pde_remove_entry(pdesc.pte,
			    pdesc.ptx));
			pdebug("PGD = %016x, VADDR = %016x\r\n",
			    pdesc.pgd, vaddr);
			pdebug("OLD = %016x, NEW = %016x\r\n",
			    PAGE_TO_KVADDR(p), PAGE_TO_KVADDR(page));
			if (replaced_page == NULL) {
				pdebug("WARNING: replaced_page = NULL\r\n");
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

struct page *pgtable_remove(void *pgtable, addr_t vaddr)
{
	struct pagedesc pdesc;
	struct page *p = NULL;

	/* TODO: acquire a per-page-table lock.  (ditto) */

	pgtable_get(pgtable, vaddr, false, &pdesc);

	/*pdebug("Removing PTE for %016x\r\n", vaddr);*/
	
	if (pdesc.pte && pdesc.pte[pdesc.ptx]) {
		p = KVADDR_TO_PAGE(pde_remove_entry(pdesc.pte, pdesc.ptx));
	}
	
	if (pdesc.pte && pde_empty(pdesc.pte)) {
		pde_remove_pgdir(pdesc.pmd, pdesc.pmx);
	}

#ifdef CONFIG_3LEVEL_PT
	if (pdesc.pmd && pde_empty(pdesc.pmd)) {
		pde_remove_pgdir(pdesc.pgd, pdesc.pgx);
	}
#else
	if (pdesc.pmd && pde_empty(pdesc.pmd)) {
		pde_remove_pgdir(pdesc.pud, pdesc.pux);
	}

	if (pdesc.pud && pde_empty(pdesc.pud)) {
		pde_remove_pgdir(pdesc.pgd, pdesc.pgx);
	}
#endif

	/* Blast TLB entries containing this virtual address away */
	tlb_remove(vaddr);

	return p;
}

int arch_map_page(struct arch_mm_struct *arch_mm, addr_t vaddr,
    struct page *p, unsigned int perm)
{
	int retcode;
	pdebug("Mapping physical %016x to virtual %016x (in PGD %016x)\r\n",
	    PAGE_TO_PADDR(p), vaddr, arch_mm->pgd);
	retcode = pgtable_insert(&(arch_mm->pgd), vaddr, p, perm, false, NULL);
	return retcode;
}

struct page *arch_unmap_page(arch_mm_t *arch_mm, addr_t vaddr)
{
	return pgtable_remove(&(arch_mm->pgd), vaddr);
}

unsigned long arch_mm_get_pfn(struct arch_mm_struct *arch_mm, addr_t vaddr)
{
	struct pagedesc pdesc;

	pgtable_get(&(arch_mm->pgd), vaddr, false, &pdesc);
	if (pdesc.pte)
		return PADDR_TO_PFN(pdesc.pte[pdesc.ptx]);
	else
		return 0;
}

int arch_mm_get_page(struct arch_mm_struct *arch_mm, addr_t vaddr,
    struct page **p, unsigned int *flags)
{
	struct pagedesc pdesc;
	pgtable_get(&(arch_mm->pgd), vaddr, false, &pdesc);
	if (pdesc.pte) {
		*p = PADDR_TO_PAGE(pdesc.pte[pdesc.ptx]);
		*flags = vma_perm(pdesc.pte[pdesc.ptx] & PTE_LOWMASK);
		return 0;
	} else {
		return -ENOENT;
	}
}

int arch_mm_new_pgtable(struct arch_mm_struct *arch_mm)
{
	/* Allocate a PGD here */
	pgdir_t *pgd = pgdir_new(ASID_INVALID);
	if (pgd == NULL)
		return -ENOMEM;
	arch_mm->pgd = (pgd_t)PAGE_TO_KVADDR(pgd);
	/* No need to set this PGD online here */
	return 0;
}

void arch_mm_destroy_pgtable(struct arch_mm_struct *arch_mm)
{
	pgdir_delete((pgdir_t *)KVADDR_TO_PAGE((addr_t)(arch_mm->pgd)));
}

#endif	/* CONFIG_HPT */
