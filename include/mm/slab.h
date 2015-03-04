/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _MM_SLAB_H
#define _MM_SLAB_H

#include <sys/types.h>
#include <ds/list.h>
#include <stddef.h>

/*
 * A simplified, coarse and not-so-efficient slab allocation implementation.
 */

/*
 * A slab manages one or more pages and the (same kind of) objects inside.
 */
struct kmem_cache;

/*
 * A page (or a set of pages) marked as a slab was separated into fixed-
 * sized chunks.  When the chunk is free for use it stores the index
 * of the next available chunk.  The index of the first free chunk is
 * stored inside the first_free_index member of slab.
 *
 * One can think of what is organized inside the slab a series of allocated
 * chunks, plus a linked FIFO list of free indices.
 */

/* This structure is an inner structure of the page struct. */
struct kmem_slab {
	list_node_t		node;
	struct kmem_cache	*cache;
	/* The index to first free chunk, starting from 0 */
	int			index;
#define INDEX_INVALID	-1
	/* Number of free chunks */
	unsigned int		count;
	/* Number of chunks */
	unsigned int		capacity;
};
typedef struct kmem_slab kmem_slab_t;
#define node_to_slab(n)		member_to_struct(n, kmem_slab_t, node)
#define next_slab(s)		node_to_slab(list_next(&((s)->node)))
static inline bool single_slab(kmem_slab_t *s)
{
	return s == next_slab(s);
}
#define slab_full(s)		((s)->count == 0)
static inline bool slab_empty(kmem_slab_t *s)
{
	return s->count == s->capacity;
}

/*
 * A cache manages one particular type of object within one or more slabs...
 */
typedef struct kmem_cache {
	list_node_t	node;
	/* List (or queue) of available and full slabs.
	 *
	 * Empty slabs are immediately freed. */
	kmem_slab_t	*avail;
	kmem_slab_t	*full;
	/* Each object type have a corresponding cache */
	size_t		size;		/* cache object size */
} kmem_cache_t;

struct kmem_cache_group {
	list_node_t	head;
	/* TODO: add a lock */
};

extern struct kmem_cache_group kmcache_group;

/* These object list manipulating macros could be compacted into a
 * macro-building macro, but it's not ctags friendly. */
#define kmcache_list		(&(kmcache_group.head))
#define node_to_kmcache(n)	member_to_struct(n, kmem_cache_t, node)
#define first_kmcache()		node_to_kmcache(list_next(kmcache_list))
/* DO NOT REFERENCE */
#define end_kmcache()		node_to_kmcache(kmcache_list)
#define next_kmcache(cache)	node_to_kmcache(list_next(&((cache)->node)))
#define prev_kmcache(cache)	node_to_kmcache(list_next(&((cache)->node)))
#define kmcache_add_before(c, newc) \
	list_add_before(&((c)->node), &((newc)->node))
#define kmcache_add_after(c, newc) \
	list_add_after(&((c)->node), &((newc)->node))
#define kmcache_delete(c)	list_del_init(&((c)->node))

void *slab_alloc(size_t bytes);
void slab_free(void *ptr);
void slab_bootstrap(void);

void test_slab(void);

#endif