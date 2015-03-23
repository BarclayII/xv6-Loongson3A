
#ifndef _ASM_MM_HIER_PGTABLE_H
#define _ASM_MM_HIER_PGTABLE_H

#include <asm/mm/page.h>
#include <sys/types.h>

struct task_struct;

struct asid_task_set {
	/* TODO: add a lock here */
	pgd_t		online_hpt[ASID_MAX + 1];
	asid_t		next_asid;
	struct task_struct *online_tasks[ASID_MAX + 1];
};

extern struct asid_task_set asid_task_set;
#define current_online_tasks	(asid_task_set.online_tasks)
#define current_next_asid	(asid_task_set.next_asid)

extern pgd_t *online_hpt;		/* for easy access by tlbex.S */

#endif
