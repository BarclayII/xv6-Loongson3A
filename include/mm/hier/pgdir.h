/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _MM_HIER_PGDIR_H
#define _MM_HIER_PGDIR_H

#include <asm/mm/page.h>
#include <mm/mmap.h>
#include <sys/types.h>

typedef struct page pgdir_t;

pgdir_t *pgdir_new();
void pgdir_delete(pgdir_t *pgdir);
#define pgdir_load(addr)	\
	((pgdir_t *)KVADDR_TO_PAGE((unsigned long)addr))
#define pgdir_entries(pgdir)	((pgdir)->entries)
#define pgdir_empty(pgdir)	(pgdir_entries(pgdir) == 0)
ptr_t pgdir_add_entry(pgdir_t *pgdir, unsigned short index, struct page *p);
ptr_t pgdir_add_page(pgdir_t *pgdir, unsigned short index);
ptr_t pgdir_add_pgdir(pgdir_t *pgdir, unsigned short index);
ptr_t pgdir_remove_entry(pgdir_t *pgdir, unsigned short index);
void pgdir_remove_page(pgdir_t *pgdir, unsigned short index);
void pgdir_remove_pgdir(pgdir_t *pgdir, unsigned short index);

/* Shortcuts */
#define pde_add_entry(pde, index, p) \
	pgdir_add_entry(pgdir_load(pde), index, p)
#define pde_add_page(pde, index) \
	pgdir_add_page(pgdir_load(pde), index)
#define pde_add_pgdir(pde, index) \
	pgdir_add_pgdir(pgdir_load(pde), index)
#define pde_remove_entry(pde, index) \
	pgdir_remove_entry(pgdir_load(pde), index)
#define pde_remove_page(pde, index) \
	pgdir_remove_page(pgdir_load(pde), index)
#define pde_remove_pgdir(pde, index) \
	pgdir_remove_pgdir(pgdir_load(pde), index)
#define pde_empty(pde)		pgdir_empty(pgdir_load(pde))
#define pde_delete(pde)		pgdir_delete(pgdir_load(pde))
#define pde_entries(pde)	pgdir_entries(pgdir_load(pde))

#endif
