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
 * (Pre-allocated statically)
 * 1		8			8
 * 2		16			16
 * 3		24			32
 * 4		40			64
 * 5		72			96
 * 6		104			128
 * 7		136			192
 * 8		200			256
 * (Dynamically allocated)
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
static const unsigned short tiny_sizes[8] = {
	8, 16, 32, 64, 96, 128, 192, 256
};
#define NR_TINY_SIZES	8
#define MAX_TINY_SIZE	256

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
	kmem_cache_t *cache;

	pdebug("Finding kmem_cache with %d bytes\r\n", bytes);

	/* Shortcut: cache the last accessed kmem_cache_t */
	if (KMCACHE_ALMOST_FIT(bytes, last_accessed)) {
		pdebug("\tResult already cached\r\n");
		return last_accessed;
	}

	for (cache = first_kmcache();
	    cache != end_kmcache();
	    cache = next_kmcache(cache))
		if (KMCACHE_ALMOST_FIT(bytes, cache)) {
			pdebug("\tFound cache with %d bytes\r\n", cache->size);
			last_accessed = cache;
			return cache;
		}

	pdebug("\tFailed to find kmem_cache with %d bytes\r\n", bytes);
	return NULL;
}

static kmem_cache_t *kmcache_new(size_t bytes)
{
	pdebug("Allocating kmem_cache with size %d\r\n", bytes);
	/* Request a new cache by calling kmalloc().
	 * NOTE: This requires that a cache for kmem_cache_t to be established
	 *       prior to everything, which is achieved in slab_bootstrap(). */
	kmem_cache_t *cache = kmalloc(sizeof(kmem_cache_t));
	if (cache == NULL)
		/* Failed to allocate a new cache */
		return NULL;
	/* Fill in the members */
	cache->avail = cache->full = NULL;
	cache->size = UBOUND(bytes);
	/* Add the cache into the cache list */
	kmem_cache_t *c;
	for (c = first_kmcache();
	    c != end_kmcache() && c->size < cache->size;
	    c = next_kmcache(c))
		/* nothing */;
	kmcache_add_before(c, cache);
	pdebug("Allocated kmem_cache with size %d\r\n", bytes);
	if (c != end_kmcache())
		pdebug("\tAdded before size %d\r\n", c->size);

	return cache;
}

static inline kmem_slab_t *find_available_slab(kmem_cache_t *cache)
{
	return cache->avail;
}

static inline void set_slab_available(kmem_slab_t *slab)
{
	pdebug("Marking slab %016x available\r\n", slab);
	/* Check if it's the only slab in full list */
	if (slab->cache->full == slab && single_slab(slab))
		slab->cache->full = NULL;
	list_del_init(&(slab->node));
	if (slab->cache->avail == NULL)
		slab->cache->avail = slab;
	else
		list_add_before(&(slab->cache->avail->node), &(slab->node));
}

static inline void set_slab_full(kmem_slab_t *slab)
{
	pdebug("Marking slab %016x full\r\n", slab);
	/* Check if it's the only slab in available list */
	if (slab->cache->avail == slab && single_slab(slab))
		slab->cache->avail = NULL;
	list_del_init(&(slab->node));
	if (slab->cache->full == NULL)
		slab->cache->full = slab;
	else
		list_add_before(&(slab->cache->full->node), &(slab->node));
}

static inline void set_slab_empty(kmem_slab_t *slab)
{
	pdebug("Marking slab %016x empty\r\n", slab);
	/* Enforce both checks */
	if (slab->cache->avail == slab && single_slab(slab))
		slab->cache->avail = NULL;
	if (slab->cache->full == slab && single_slab(slab))
		slab->cache->full = NULL;
	list_del_init(&(slab->node));
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
		if ((cache = kmcache_new(bytes)) == NULL)
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
	/*
	 * The bootstrap routine for slab allocation is rather primitive as
	 * we manually construct kmem_cache_t data inside a page here.
	 */
	pdebug("Bootstrapping slab allocation\r\n");

	/* Initialize cache list */
	list_init(kmcache_list);

	/* Allocate a slab for storing caches.  slab_new() shouldn't be called
	 * because it assumes that a suitable cache has been already found,
	 * and we're *creating* caches here. */
	struct page *p = pgalloc();
	p->type = PGTYPE_SLAB;
	kmem_slab_t *slab = &(p->slab);

	/* Find what size category kmem_cache_t falls into */
	size_t kmem_cache_size = UBOUND(sizeof(kmem_cache_t));

	/* Fill in the slab */
	list_init(&(slab->node));
	slab->capacity = PGSIZE * p->page_count / kmem_cache_size;
	fill_slab(slab, kmem_cache_size, slab->capacity);

	/* Fill in the initial cache structure */
	kmem_cache_t init_cache;
	list_init(&(init_cache.node));
	init_cache.avail = init_cache.full = NULL;
	init_cache.size = kmem_cache_size;

	/* Copy the structure into the first chunk of slab */
	memcpy((void *)PAGE_TO_KVADDR(p), &init_cache, sizeof(kmem_cache_t));

	/* Relate the slab and the copied cache */
	slab->cache = (kmem_cache_t *)PAGE_TO_KVADDR(p);

	/* Fill in other members of the slab */
	slab->index = 1;
	slab->count = slab->capacity - 1;

	/* Mark it available */
	set_slab_available(slab);

	/* Add the cache into cache list */
	kmcache_add_before(end_kmcache(), slab->cache);
}
