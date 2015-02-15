/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <ds/list.h>

inline void list_init(list_node_t *l)
{
	l->prev = l->next = l;
}

inline void list_add_before(list_node_t *l, list_node_t *node)
{
	l->prev->next = node;
	node->prev = l->prev;
	l->prev = node;
	node->next = l;
}

inline void list_add_after(list_node_t *l, list_node_t *node)
{
	l->next->prev = node;
	node->next = l->next;
	l->next = node;
	node->prev = l;
}

inline void list_del(list_node_t *node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;
}

inline void list_del_init(list_node_t *node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;
	node->prev = node->next = node;
}

inline list_node_t *list_prev(list_node_t *node)
{
	return node->prev;
}

inline list_node_t *list_next(list_node_t *node)
{
	return node->next;
}
