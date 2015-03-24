/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_MM_HIER_PGTABLE_H
#define _ASM_MM_HIER_PGTABLE_H

#include <asm/mm/page.h>
#include <sys/types.h>

struct task_struct;

struct asid_task_set {
	/* TODO: add a lock here */
	/* online_hpt should be the first member, or tlbex.S should be changed
	 * accordingly. */
	pgd_t		online_hpt[ASID_MAX + 1];
	asid_t		next_asid;
	struct task_struct *online_tasks[ASID_MAX + 1];
};

extern struct asid_task_set asid_task_set;
#define current_online_tasks	(asid_task_set.online_tasks)
#define current_online_hpt	(asid_task_set.online_hpt)
#define current_next_asid	(asid_task_set.next_asid)

#endif
