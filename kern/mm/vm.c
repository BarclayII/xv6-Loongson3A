
mm_t *mm_new(void)
{
	mm_t *mm = kmalloc(sizeof(mm_t));
	memset(mm, 0, sizeof(mm_t));
	if (mm != NULL) {
		list_init(&(mm->mmap_list));
	}

	new_arch_mm(&(mm->arch_mm));

	return mm;
}

vm_area_t *vm_area_new(addr_t start, addr_t end, unsigned long flags)
{
	if (start >= end)
		return NULL;

	assert(PAGE_OFF(start) == 0);
	assert(PAGE_OFF(end) == 0);

	vm_area_t *vma = kmalloc(sizeof(vm_area_t));

	if (vma != NULL) {
		vma->start = start;
		vma->end = end;
		vma->flags = flags;
	}

	return vma;
}

static inline void vm_area_remove(mm_t *mm, vm_area_t *vma)
{
	assert(vma->mm == mm);
	vma_delete(vma);
}

static inline void vm_area_destroy(vm_area_t *vma)
{
	kfree(vma);
}

void mm_destroy(mm_t *mm)
{
	/* Destroy hardware-specific memory mapping */
	destroy_arch_mm(&(mm->arch_mm));
	/* Traverse virtual memory area list and free each node */
	vm_area_t *vma, *next_vma;
	for (vma = first_vma(mm); vma != end_vma(mm); vma = next_vma) {
		vma_delete(vma);
		next_vma = list_next(vma);
		kfree(vma);
	}
	/* Destroy @mm itself */
	kfree(mm);
}

vm_area_t *vm_area_find(mm_t *mm, addr_t addr)
{
	if (mm == NULL)
		return NULL;

	vm_area_t *vma = mm->vma_last_accessed;
	if (vma != NULL && vm_area_fit(vma, addr))
		return vma;

	for (vma = first_vma(mm); vma != end_vma(mm); vma = next_vma(vma)) {
		if (vm_area_fit(vma, addr)) {
			mm->vma_last_accessed = vma;
			return vma;
		}
	}

	return NULL;
}

/* Currently always returns 0 */
int vm_area_insert(mm_t *mm, vm_area_t *new_vma)
{
	vm_area_t *vma;
	if (mm != NULL && new_vma != NULL) {
		new_vma->mm = mm;
		for (vma = first_vma(mm);
		    vma != end_vma(mm) && vm_area_prec(vma, new_vma);
		    vma = next_vma(vma))
			assert(!vm_area_overlap(vma, new_vma));
		vma_add_before(vma, new_vma);
	}

	return 0;
}

/* 
 * Map (incontiguous) page list @p to virtual address @vaddr of memory
 * mapping structure @mm.
 * @vaddr should be page-aligned.
 */
int map_pages(mm_t *mm, addr_t vaddr, struct page *p,
    unsigned long flags)
{
	assert(PAGE_OFF(vaddr) == 0);

	struct page *curp = first_page(p);
	int i, retcode;
	addr_t cur_vaddr;

	/* Create a new virtual memory area entry first */
	cur_vaddr = vaddr;
	addr_t vaddr_end = cur_vaddr + PGSIZE * page_count(p);
	vm_area_t *vma = vm_area_new(cur_vaddr, vaddr_end, flags);

	/* Add it to virtual memory area list */
	if ((retcode = vm_area_insert(mm, vma)) != 0)
		goto rollback_vma;

	/* Add relations between virtual address sections and physical pages
	 * into the corresponding page table. */
	for (i = 0; i < page_count(p); ++i, curp = next_page(curp)) {
		if ((retcode = arch_map_page(&(mm->arch_mm), cur_vaddr, curp,
		    page_perm(flags))) != 0)
			goto rollback_map;
		cur_vaddr += PGSIZE;
	}

	return 0;

rollback_map:
	cur_vaddr = vaddr;
	for (i = 0, curp = first_page(p);
	    i < page_count(p);
	    ++i, curp = next_page(curp)) {
		/* No need to free pages here. */
		arch_unmap_page(&(mm->arch_mm), cur_vaddr);
		cur_vaddr += PGSIZE;
	}

rollback_vma:
	vm_area_destroy(vma);

ret:
	return retcode;
}

/* 
 * Unmapping pages is somewhat complicated compared to mapping:
 * UNIX munmap(2) system call allows to unmap only some part of the previously
 * mapped pages, so we probably need to shrink the virtual address area
 * containing the given address, or decompose it into two smaller pieces.
 * Here, unmap_pages() unmaps and _frees_ @nr_pages virtual pages starting from
 * (page-aligned) virtual address @vaddr.  The actual region requested may span
 * across several virtual memory areas, and even contain unmapped pages.
 */
int unmap_pages(mm_t *mm, addr_t vaddr, size_t nr_pages)
{
	assert(PAGE_OFF(vaddr) == 0)
	/* Find the starting virtual address area */
	vm_area_t *vma_start = vm_area_find(mm, vaddr);
	addr_t vaddr_end = vaddr + nr_pages * PGSIZE;
	vm_area_t *vma_end = vm_area_find(mm, vaddr_end - 1);
	vm_area_t *head, *tail;

	if (vma_start == NULL || vma_end == NULL)
		return -ENOENT;

	/* Update virtual memory area list */
	if (vma_start == vma_end) {
		/* The region is inside one virtual memory area entry */
		head = vm_area_new(vma_start->start, vaddr, vma_start->flags);
		tail = vm_area_new(vaddr_end, vma_start->end, vma_start->flags);
		/* Rollback if failed to allocate a non-NULL structure */
		if ((head == NULL && vma_start->start != vaddr) ||
		    (tail == NULL && vaddr_end != vma_start->end))
			goto rollback_vma;
		vm_area_remove(mm, vma_start);
		vm_area_destroy(vma_start);
		/* NULL cases handled in vm_area_insert() */
		vm_area_insert(mm, head);
		vm_area_insert(mm, tail);
	} else {
		/* The region ends reside in different entries */
		vm_area_t *vma;
		/* Destroy all entries in the middle */
		for (vma = next_vma(vma_start);
		    vma != vma_end;
		    vma = next_vma(vma)) {
			vm_area_remove(mm, vma);
			vm_area_destroy(vma);
		}
		/* Update entries at both ends */
		vma_start->end = vaddr;
		vma_end->start = vaddr + nr_pages * PGSIZE;
	}

	/* Unmap and free physical pages */
	struct page *p;
	addr_t cur_vaddr;
	int i;
	for (i = 0; i < nr_pages; ++i) {
		/* Unmapped pages are automatically handled in the following
		 * two functions.  No special handling is needed. */
		p = arch_unmap_page(mm, cur_vaddr);
		cur_vaddr += PGSIZE;
		pgfree(p);
	}

	return 0;

rollback_vma:
	vm_area_destroy(head);
	vm_area_destroy(tail);
	return -ENOMEM;
}

int mm_create_uvm(mm_t *mm, void *addr, size_t len, unsigned long vm_flags)
{
	if (len == 0)
		return 0;

	addr_t start = PGADDR_ROUNDDOWN((addr_t)addr);
	addr_t end = PGADDR_ROUNDDOWN((addr_t)addr + len);
	if (start == 0)
		return 0;

	size_t nr_pages = NR_PAGES_NEEDED(end - start);
	struct page *p = alloc_pages(nr_pages);
	if (p == NULL)
		return -ENOMEM;

	int retcode = map_pages(mm, start, p, vm_flags);
	if (retcode != 0)
		goto rollback_pages;

	return 0;

rollback_pages:
	free_all_pages(p);
	return retcode;
}

/* Destroy an entire virtual memory area which contains @addr */
int mm_destroy_uvm(mm_t *mm, void *addr)
{
	vm_area_t *vma = vma_find(mm, addr);
	size_t len = vma->end - vma->start;
	size_t pages = NR_PAGES_NEEDED(len);
	return unmap_pages(mm, vma->start, pages);
}

/*
 * Should not be called directly.  There are cleaner wrappers called
 * copy_to_uvm() and copy_from_uvm().
 */
static inline int copy_uvm(mm_t *mm, void *uvaddr, void *kvaddr, size_t len,
    bool to)
{
	if (!USERSPACE((addr_t)uvaddr))
		return -EFAULT;

	size_t i, nr_pages = NR_PAGES_SPANNED((addr_t)uvaddr, len);
	addr_t start = 0, end = 0;
	void *kuvaddr;
	for (i = 0; i < nr_pages; ++i) {
		start = (addr_t)((i == 0) ? uvaddr : end);
		end = (addr_t)((i == nr_pages - 1) ? uvaddr + len :
		    PGEND(start));
		kuvaddr = (void *)UVADDR_TO_KVADDR(mm, start);
		if (to)
			memcpy(kvaddr, kuvaddr, end - start);
		else
			memcpy(kuvaddr, kvaddr, end - start);
		kvaddr += (end - start);
	}
	return 0;
}

/* Copy data from kernel to user space */
int copy_to_uvm(mm_t *mm, void *uvaddr, void *kvaddr, size_t len)
{
	return copy_uvm(mm, uvaddr, kvaddr, len, true);
}

/* Copy data from user space to kernel */
int copy_from_uvm(mm_t *mm, void *uvaddr, void *kvaddr, size_t len)
{
	return copy_uvm(mm, uvaddr, kvaddr, len, false);
}
