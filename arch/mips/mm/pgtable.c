
int arch_map_page(arch_mm_t *arch_mm, addr_t vaddr, struct page *p,
    unsigned int perm)
{
	int retcode;
	struct page *replace = NULL;
	retcode = pgtable_insert(&(arch_mm->pgd), vaddr, p, perm, true,
	    &replace);
	if (retcode != 0)
		return retcode;
	if (replace != NULL)
		pgfree(replace);
	return 0;
}

void new_arch_mm(arch_mm_t *arch_mm)
{
}

void destroy_arch_mm(arch_mm_t *arch_mm)
{
}

struct page *arch_unmap_page(arch_mm_t *arch_mm, addr_t vaddr)
{
	return pgtable_remove(&(arch_mm->pgd), vaddr);
}

unsigned int page_perm(unsigned long vm_flags)
{
	return (vm_flags & VMA_WRITE) ? PTE_VALID|PTE_DIRTY : PTE_VALID;
}

