/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <mm/slab.h>
#include <mm/mmap.h>
#include <mm/kmalloc.h>
#include <ds/list.h>
#include <sys/types.h>
#include <string.h>
#include <mathop.h>
#include <printk.h>
#include <assert.h>

/*
 * This slab implementation is not strictly fragmentation-free.
 *
 * We first assume that all requests are aligned to 8 bytes.
 *
 * Cache list:
 * Cache #	Lower Bound		Upper Bound
 * 1		8			8
 * 2		16			16
 * 3		24			32
 * 4		40			64
 * 5		72			96
 * 6		104			128
 * 7		136			192
 * 8		200			256
 * n>8		2^(n-1)+8		2^n
 *
 * Requested size greater than half the page size is processed by a separate
 * large chunk allocator, which is a simple and brutal page allocator.
 */

/* @bytes is always rounded to multiplier of 8 */
static const unsigned short lbound[32] = {
	8,	16,	24,	24,
	40,	40,	40,	40,
	72,	72,	72,	72,
	104,	104,	104,	104,
	136,	136,	136,	136,
	136,	136,	136,	136,
	200,	200,	200,	200,
	200,	200,	200,	200
};
static const unsigned short ubound[32] = {
	8,	16,	32,	32,
	64,	64,	64,	64,
	96,	96,	96,	96,
	128,	128,	128,	128,
	192,	192,	192,	192,
	192,	192,	192,	192,
	256,	256,	256,	256,
	256,	256,	256,	256,
};
#define NR_TINY_SIZES	8
static const unsigned short tiny_sizes[NR_TINY_SIZES] = {
	8, 16, 32, 64, 96, 128, 192, 256
};

struct kmem_cache_group kmcache_group;

#define LBOUND(x)	\
	(((x) > 256) ? (1 << (fls((x) - 1) - 1)) + 8 : lbound[(x >> 3) - 1])
#define UBOUND(x)	\
	(((x) > 256) ? (1 << fls((x) - 1)) : ubound[(x >> 3) - 1])
#define KMCACHE_ALMOST_FIT(n, c)	\
	(((c) != NULL) && ((n) >= LBOUND((c)->size)) && ((n) <= (c)->size))

static kmem_cache_t *find_kmcache(size_t bytes)
{
	static kmem_cache_t *last_accessed = NULL;
	int i;

	if (KMCACHE_ALMOST_FIT(bytes, last_accessed)) {
		pdebug("Result already cached\r\n");
		return last_accessed;
	}
	pdebug("Finding kmem_cache with %d bytes\r\n", bytes);
	for (i = 0; i < NR_SLAB_ORDERS; ++i)
		if (KMCACHE_ALMOST_FIT(bytes, &(kmcache[i]))) {
			pdebug("\tFound cache with %d bytes\r\n",
			    kmcache[i].size);
			last_accessed = &(kmcache[i]);
			return &(kmcache[i]);
		}

	panic("Failed to find kmem_cache with %d bytes\r\n", bytes);
	return NULL;
}

static inline kmem_slab_t *find_available_slab(kmem_cache_t *cache)
{
	return cache->avail;
}

static inline void set_slab_available(kmem_slab_t *slab)
{
	pdebug("Marking slab %016x available\r\n", slab);
	list_del_init(&(slab->node));
	/* Check if it's the only slab in full list */
	if (slab->cache->full == slab && single_slab(slab))
		slab->cache->full = NULL;
	if (slab->cache->avail == NULL)
		slab->cache->avail = slab;
	else
		list_add_before(&(slab->cache->avail->node), &(slab->node));
	pdebug("Slab %016x is set available\r\n", slab);
}

static inline void set_slab_full(kmem_slab_t *slab)
{
	pdebug("Marking slab %016x full\r\n", slab);
	list_del_init(&(slab->node));
	/* Check if it's the only slab in available list */
	if (slab->cache->avail == slab && single_slab(slab))
		slab->cache->avail = NULL;
	if (slab->cache->full == NULL)
		slab->cache->full = slab;
	else
		list_add_before(&(slab->cache->full->node), &(slab->node));
	pdebug("Slab %016x is set full\r\n", slab);
}

static inline void set_slab_empty(kmem_slab_t *slab)
{
	pdebug("Marking slab %016x empty\r\n", slab);
	list_del_init(&(slab->node));
	/* Enforce both checks */
	if (slab->cache->avail == slab && single_slab(slab))
		slab->cache->avail = NULL;
	if (slab->cache->full == slab && single_slab(slab))
		slab->cache->full = NULL;
	pdebug("Slab %016x is set empty\r\n", slab);
}

static void fill_slab(kmem_slab_t *slab, size_t size, unsigned int capacity)
{
	pdebug("Filling slab %016x\r\n", slab);
	int i;
	addr_t entry = PAGE_TO_KVADDR(slab_to_page(slab));
	for (i = 0; i < capacity; ++i) {
		if (i == capacity - 1)
			*(int *)entry = INDEX_INVALID;
		else
			*(int *)entry = i + 1;
		entry += size;
	}
}

static kmem_slab_t *slab_new(kmem_cache_t *cache)
{
	pdebug("Allocating slab with size %d\r\n", cache->size);
	/* Allocate a new slab page */
	struct page *p = pgalloc();
	if (p == NULL)
		/* Failed to allocate a page */
		return NULL;
	p->type = PGTYPE_SLAB;

	/* Fill in the slab field */
	kmem_slab_t *slab = &(p->slab);
	list_init(&(slab->node));
	slab->cache = cache;
	slab->index = 0;
	slab->count = slab->capacity = PGSIZE * p->page_count / cache->size;

	/* Mark it available */
	set_slab_available(slab);

	/* Fill in the page and build the free index list */
	fill_slab(slab, cache->size, slab->capacity);

	pdebug("Allocated size %d slab at %016x\r\n", cache->size, slab);

	return slab;
}

static void slab_delete(kmem_slab_t *slab)
{
	pdebug("Freeing slab %016x with size %d\r\n", slab, slab->cache->size);
	pgfree(slab_to_page(slab));
	pdebug("Freed slab %016x\r\n", slab);
}

void *slab_alloc(size_t bytes)
{
	pdebug("slab_alloc(%d)\r\n", bytes);
	kmem_cache_t *cache;
	kmem_slab_t *slab;
	addr_t slab_start;
	void *result;
	/* Find a suitable cache first.  If such cache could not be found,
	 * create one.  If the creation also fails, then there's nothing
	 * we can really do. */
	cache = find_kmcache(bytes);
	if (!cache) {
		/* (usually) NOTREACHED */
		return NULL;
	}

	/* Find an available slab, or allocate a new one if there's no such
	 * slab. */
	slab = find_available_slab(cache);
	if (!slab) {
		if ((slab = slab_new(cache)) == NULL)
			return NULL;
	}

	/* Get & update first free index */
	assert(slab->index >= 0);
	slab_start = PAGE_TO_KVADDR(slab_to_page(slab));
	result = (void *)(slab_start + slab->index * cache->size);
	slab->index = *(unsigned int *)result;

	/* Update free chunk count and slab state */
	--(slab->count);
	if (slab_full(slab))
		/* Slab no longer available, add it to full list. */
		set_slab_full(slab);

#define JUNK	0x02
	/* Shred what's inside the chunk to avoid revealing original content
	 * (the free index) to caller. */
	memset(result, JUNK, bytes);
	pdebug("\treturning %016x\r\n", result);
	return result;
}

void slab_free(void *ptr)
{
	pdebug("slab_free(%016x)\r\n", ptr);
	/* Check if the pointer is really from a slab */
	assert(is_slab(KVADDR_TO_PAGE((addr_t)ptr)));

	/* Find out which chunk the given pointer resides in.
	 * First, we calculate the offset from the beginning of the slab.
	 * The index could be obtained by dividing the offset by cache object
	 * size. */
	struct page *p = first_page(KVADDR_TO_PAGE((addr_t)ptr));
	addr_t slab_start = PAGE_TO_KVADDR(p);
	kmem_slab_t *slab = &(p->slab);
	int index = ((addr_t)ptr - slab_start) / slab->cache->size;
	/* TODO: double free check */

	/* Fill in the chunk with the index of current object */
	*(int *)(slab_start + slab->cache->size * index) = slab->index;
	slab->index = index;

	/* Update free chunk count and slab state */
	if (slab_full(slab))
		set_slab_available(slab);
	++(slab->count);

	/* If the slab is empty then free the underlying pages immediately */
	if (slab_empty(slab)) {
		set_slab_empty(slab);
		slab_delete(slab);
	}
}

void slab_bootstrap(void)
{
	pdebug("Bootstrapping slab allocation\r\n");
	int i;
	for (i = 0; i < NR_TINY_SIZES; ++i) {
		kmcache[i].avail = kmcache[i].full = NULL;
		kmcache[i].size = tiny_sizes[i];
	}
	for (i = NR_TINY_SIZES; i < NR_SLAB_ORDERS; ++i) {
		kmcache[i].avail = kmcache[i].full = NULL;
		kmcache[i].size = 1 << (i + 1);
	}
}
