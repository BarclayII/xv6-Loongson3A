/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _MM_VMM_H
#define _MM_VMM_H

#include <config.h>
#include <assert.h>
#include <mm/pgtable.h>
#include <asm/mm/vmm.h>

/*
 * Virtual memory area structure which maintains a segment of virtual
 * address space.
 */

struct mm_struct;

typedef struct vm_area_struct {
	/* @start and @end should be page aligned */
	addr_t		start;		/* starting address (inclusive) */
	addr_t		end;		/* ending address (exclusive) */
	/* values of this field are defined in <asm/mm/[imp]/vmm.h> */
	unsigned long	flags;		/* various flags */
/* These flags match ELF segment flags */
#define VMA_EXEC	0x01
#define VMA_WRITE	0x02
#define VMA_READ	0x04
	struct mm_struct *mm;		/* memory mapping structure */
	list_node_t	node;		/* list node */
} vm_area_t;

vm_area_t *vm_area_new(addr_t start, addr_t end, unsigned long flags);

static inline vm_area_t *
vm_area_new_size(addr_t start, size_t size, unsigned long flags)
{
	return vm_area_new(start, start + size, flags);
}

static inline bool vm_area_valid(vm_area_t *vma)
{
	return vma->start < vma->end;
}

static inline bool vm_area_fit(vm_area_t *vma, addr_t addr)
{
	assert(vm_area_valid(vma));
	return vma->start <= addr && vma->end > addr;
}

/* Test if @vma1 precedes @vma2 */
static inline bool vm_area_prec(vm_area_t *vma1, vm_area_t *vma2)
{
	assert(vm_area_valid(vma1));
	assert(vm_area_valid(vma2));
	return vma1->end <= vma2->start;
}

/* Test if @vma1 succeeds @vma2 */
static inline bool vm_area_succ(vm_area_t *vma1, vm_area_t *vma2)
{
	assert(vm_area_valid(vma1));
	assert(vm_area_valid(vma2));
	return vma2->end <= vma1->start;
}

static inline bool vm_area_overlap(vm_area_t *vma1, vm_area_t *vma2)
{
	assert(vm_area_valid(vma1));
	assert(vm_area_valid(vma2));
	return !(vm_area_prec(vma1, vma2) || vm_area_succ(vma1, vma2));
}

/*
 * (Per-process) Memory mapping structure
 */

typedef struct mm_struct {
	arch_mm_t	arch_mm;
	list_node_t	mmap_list;
	vm_area_t	*vma_last_accessed;
	size_t		mmap_count;
	size_t		ref_count;
	/* TODO: add lock */
} mm_t;

#define node_to_vma(n)	member_to_struct(n, vm_area_t, node)
#define first_vma(mm)	node_to_vma(list_next(&((mm)->mmap_list)))
#define end_vma(mm)	node_to_vma(&((mm)->mmap_list))
#define next_vma(vma)	node_to_vma(list_next(&((vma)->node)))
#define vma_add_before(vma, new_vma) \
	list_add_before(&((vma)->node), &((new_vma)->node))
#define vma_add_after(vma, new_vma) \
	list_add_after(&((vma)->node), &((new_vma)->node))
#define vma_delete(vma)	list_del_init(&((vma)->node))

/* Map user address to kernel address.
 * Note that a user address area may be incontiguous physically. */
static inline addr_t __uvaddr_to_kvaddr(mm_t *mm, addr_t uvaddr)
{
	unsigned long pfn = arch_mm_get_pfn(&(mm->arch_mm), uvaddr);
	if (pfn == 0)
		return 0;
	else
		return PFN_TO_KVADDR(pfn) + PAGE_OFF(uvaddr);
}
#define UVADDR_TO_KVADDR(mm, uvaddr)	__uvaddr_to_kvaddr(mm, uvaddr)

extern mm_t kern_high_mm;		/* High memory mapping */
extern mm_t kern_low_mm;		/* Low memory mapping */
#define kern_mm	kern_high_mm

mm_t *mm_new(void);
void mm_destroy(mm_t *mm);
vm_area_t *vm_area_find(mm_t *mm, addr_t addr);
int vm_area_insert(mm_t *mm, vm_area_t *new_vma);

int map_pages(mm_t *mm, addr_t vaddr, struct page *p, unsigned long flags);
int unmap_pages(mm_t *mm, addr_t vaddr, size_t nr_pages);
int mm_create_uvm(mm_t *mm, void *addr, size_t len, unsigned long vm_flags);
int mm_destroy_uvm(mm_t *mm, void *addr);
int copy_to_uvm(mm_t *mm, void *uvaddr, void *kvaddr, size_t len);
int copy_from_uvm(mm_t *mm, void *uvaddr, void *kvaddr, size_t len);

#endif
