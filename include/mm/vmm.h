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

#ifdef CONFIG_HPT
#include <asm/mm/hier/vmm.h>
#endif

#ifdef CONFIG_IPT
#include <asm/mm/inv/vmm.h>
#endif

/*
 * Virtual memory area structure which maintains a segment of virtual
 * address space.
 */

struct mm_struct;

typedef struct vm_area_struct {
	addr_t		start;		/* starting address (inclusive) */
	addr_t		end;		/* ending address (exclusive) */
	/* values of this field are defined in <asm/mm/[imp]/vmm.h> */
	unsigned long	flags;		/* various flags */
	struct mm_struct *mm;		/* memory management structure */
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
 * (Per-process) Memory management structure
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
#define next_vma(vma)	node_to_vma(list_next(&((vma)->list_node)))
#define vma_add_before(vma, new_vma) \
	list_add_before(&((vma)->node), &((new_vma)->node))
#define vma_add_after(vma, new_vma) \
	list_add_after(&((vma)->node), &((new_vma)->node))
#define vma_delete(vma)	list_del_init(&((vma)->node))

extern mm_t kern_high_mm;		/* High memory manager */
extern mm_t kern_low_mm;		/* Low memory manager */
#define kern_mm	kern_high_mm

#endif
