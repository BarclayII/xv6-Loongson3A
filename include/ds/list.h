/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _DS_LIST_H
#define _DS_LIST_H

/* Dual-linked cycled list */
typedef struct _list_node {
	struct _list_node *prev, *next;
} list_node_t;

inline void list_init(list_node_t *l);
inline void list_add_before(list_node_t *l, list_node_t *node);
inline void list_add_after(list_node_t *l, list_node_t *node);
#define list_add	list_add_after
inline void list_del(list_node_t *node);
inline void list_del_init(list_node_t *node);
inline list_node_t *list_prev(list_node_t *node);
inline list_node_t *list_next(list_node_t *node);
/* For lists with a head node */
#define list_empty(l)	(((l) == (l)->next) && ((l) == (l)->prev))
/* For lists without head node */
#define list_single(l)	list_empty(l)

#endif
