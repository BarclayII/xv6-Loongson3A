/*
 * Copyright (C) xxxx Anonymous
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _SCHED_TASK_H
#define _SCHED_TASK_H

/*
 * Somewhat an extremely-simplified version of Linux's task_struct, or an
 * extended version of xv6's struct proc.
 */
#define PROC_NAME_LEN_MAX	256
typedef struct task_struct {
	int		tid;		/* Thread ID in process */
#define TID_MAIN		0
	int		pid;		/* Process ID (TGID in Linux) */
	unsigned short	state;		/* Running state */
#define TASK_INTERRUPTIBLE	0x0001
#define TASK_UNINTERRUPTIBLE	0x0002
#define TASK_DEAD		0x0040
#define TASK_WAKEKILL		0x0080
#define TASK_WAKING		0x0100
#define TASK_RUNNABLE		0x0200	/* Needs to be scheduled if not set */
	unsigned short	exit_state;	/* Exit state */
#define EXIT_ZOMBIE		0x0010
#define EXIT_DEAD		0x0020
	union {
		int	exit_code;	/* Exit code */
		int	signal_code;	/* Signal code */
	};
	int		asid;		/* ASID if needed */
	unsigned int	flags;		/* Process flags */
#define PF_RUNNING	0x0		/* Running */
#define PF_STARTING	0x00000002	/* being created */
#define PF_EXITING	0x00000004	/* getting shut down */
#define PF_SIGNALED	0x00000400	/* killed by a signal */
	context_t	context;	/* Current context */
	trapframe_t	*tf;		/* Current trapframe */
	struct task_struct *parent;	/* Parent task (usually a process) */
	list_node_t	proc_child;	/* Children process list */
	list_node_t	proc_sib;	/* Sibling process list */
	struct task_struct *thgroup_leader;	/* Thread group leader */
	list_node_t	thread_node;	/* Thread list in single process */
	mm_t		*mm;		/* Memory mapping structure */
	ptr_t		kstack;		/* Kernel stack (bottom) */
	unsigned long	heapsize;	/* Extendable heap size */
	char		name[PROC_NAME_LEN_MAX];	/* Name */
	list_node_t	proc_list;	/* Global process list */
	list_node_t	hash_list;	/* Hash list */
} task_t;

typedef struct task_group {
	list_node_t	proc_list;	/* Global process list */
	list_node_t	hash_list[HASH_LIST_SIZE]; /* Process hash list */
	size_t		proc_num;	/* Number of total processes */
	size_t		th_num;		/* Number of total threads */
} task_group_t;

extern task_group_t task_group;
#define process_list		(task_group.proc_list)
#define process_hash_list	(task_group.hash_list)
#define nr_process		(task_group.proc_num)
#define nr_thread		(task_group.th_num)

#define HASH_LIST_SIZE	32

/*
 * The operating system utilizes XKPHY and CKSEG0 for kernel memory mappings.
 * Both address spaces are not processed through TLB, avoiding establishing
 * separate page table entries for kernel mappings in each process.
 *
 * Process virtual address space:
 * +----------------------------+ 0x0000 0000 0000 0000
 * |Reserved (for catching NULL)|
 * +----------------------------+ sh_addr(.text)
 * |           .text            |
 * +----------------------------+ . + sizeof(.text)
 * +----------------------------+ sh_addr(.data)
 * |        .data + .bss        |
 * +----------------------------+ . + sizeof(.data) + sizeof(.bss)
 * +----------------------------+ PGROUNDUP(.)
 * |   Thread-specific Stack    |
 * +----------------------------+ . + RLIMIT_STACK (usually aligned)
 * +----------------------------+ PGROUNDUP(.) (maybe equal to .)
 * |       Extendable Heap      |
 * +----------------------------+
 * |            ...             |
 * +----------------------------+ XKPHY_START (inaccessible in user mode)
 * |  Kernel Highmem Management |
 * +----------------------------+ XKPHY_END
 * |            ...             |
 * +----------------------------+ CKSEG0_START
 * |  Kernel Lowmem Management  |
 * +----------------------------+ CKSEG0_END
 */

#endif
