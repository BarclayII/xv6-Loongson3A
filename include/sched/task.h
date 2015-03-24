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

#ifndef _SCHED_H
#error "include <sched.h> instead"
#endif

#include <asm/ptrace.h>
#include <mm/vmm.h>
#include <sys/types.h>
#include <ds/list.h>

/*
 * Somewhat an extremely-simplified version of Linux's task_struct, or an
 * extended version of xv6's struct proc.
 */
#define PROC_NAME_LEN_MAX	256
typedef struct task_struct {
	int		tid;		/* Thread ID in process */
#define TID_MAIN		0
	int		pid;		/* Process ID (TGID in Linux) */
	/* Predefined process IDs */
#define PID_IDLE		0
#define PID_INIT		1
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
		unsigned short	exit;	/* Exit code */
		unsigned short	signal;	/* Signal code */
	};
	unsigned short	on_cpu;		/* CPU # where this task is running */
	unsigned int	asid;		/* ASID if needed */
	unsigned int	flags;		/* Process flags */
#define PF_RUNNING	0x0		/* Running */
#define PF_STARTING	0x00000002	/* being created */
#define PF_EXITING	0x00000004	/* getting shut down */
#define PF_SIGNALED	0x00000400	/* killed by a signal */
	context_t	*context;	/* Current context */
	trapframe_t	*tf;		/* Current trapframe */
	mm_t		*mm;		/* Memory mapping structure */
	ptr_t		kstack;		/* Kernel stack (bottom) */
	/* For indicating where the user stack should start */
	ptr_t		progtop;	/* Top of all segments (page-aligned) */
	/* Expandable heap is placed directly above user stack */
	ptr_t		ustacktop;	/* Top of user stack */
	unsigned long	heapsize;	/* Expandable heap size */
	char		name[PROC_NAME_LEN_MAX];	/* Name */
	/* Process tree related */
	struct task_struct *parent;	/* Parent task (usually a process) */
	struct task_struct *first_child;/* First child process */
	list_node_t	proc_sib;	/* Sibling process list */
	unsigned int	num_child;	/* Number of children */
	unsigned int	num_sibling;	/* Number of siblings */
	/* Thread list per process */
	struct task_struct *thgroup_leader;	/* Thread group leader */
	list_node_t	thread_node;	/* Thread list in single process */
	/* Available in main thread only */
	unsigned long	num_threads;	/* Number of threads */
	/* Global process list related */
	list_node_t	proc_node;	/* Global process list */
	list_node_t	hash_node;	/* Hash list */
} task_t;

#define HASH_LIST_SIZE	32
#define HASH_LIST_ORDER	5

typedef struct task_set {
	list_node_t	proc_list;	/* Global process list */
	list_node_t	hash_list[HASH_LIST_SIZE]; /* Process hash list */
	size_t		proc_num;	/* Number of total processes */
	size_t		th_num;		/* Number of total threads */
} task_set_t;

extern task_set_t task_set;

#define process_list		(task_set.proc_list)
#define process_hash_list	(task_set.hash_list)
#define nr_process		(task_set.proc_num)
#define nr_thread		(task_set.th_num)

#define sibnode_to_proc(n)	member_to_struct(n, task_t, proc_sib)
#define sibnode_to_task(n)	sibnode_to_proc(n)
#define thnode_to_thread(n)	member_to_struct(n, task_t, thread_node)
#define thnode_to_task(n)	thnode_to_thread(n)
#define procnode_to_proc(n)	member_to_struct(n, task_t, proc_node)
#define procnode_to_task(n)	procnode_to_proc(n)
#define hashnode_to_proc(n)	member_to_struct(n, task_t, hash_node)
#define hashnode_to_task(n)	hashnode_to_proc(n)

#define next_sib_task(t)	sibnode_to_task(list_next(&((t)->proc_sib)))

#define task_is_process(task)	((task)->tid == TID_MAIN)
#define task_is_mainthread(task) task_is_process(task)

static inline unsigned int task_num_sibling(task_t *t)
{
	return (t == NULL) ? 0 : t->num_sibling;
}

/* IDLE and init are cached globally */
extern task_t *idleproc, *initproc;

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
 * +----------------------------+ progtop = . + sizeof(.data) + sizeof(.bss)
 * +----------------------------+ PGROUNDUP(.)
 * |   Thread-specific Stack    | (Stack grows down towards lower address)
 * +----------------------------+ ustacktop = .
 * |       Extendable Heap      | (Heap grows up towards higher address)
 * +----------------------------+
 * |            ...             |
 * +----------------------------+ XKPHY_START (inaccessible in user mode)
 * |  Kernel Highmem Management |
 * +----------------------------+ XKPHY_END
 * |            ...             |
 * +----------------------------+ CKSEG0_START
 * |  Kernel Lowmem Management  |
 * +----------------------------+ CKSEG0_END
 *
 * Kernel Stack Layout:
 * +----------------------------+ kstack + KSTACK_SIZE
 * |      Process Context       |
 * +----------------------------+
 * |         Trapframe          |
 * +----------------------------+
 * |            ...             |
 * +----------------------------+ kstack
 */

#define KSTACK_SIZE	8192
#define USTACK_SIZE	8192

#define USTACK_PERM	(VMA_READ | VMA_WRITE)

/* Task list manipulation */
void tasklist_init(void);
void add_process(task_t *proc, task_t *parent);
void add_thread(task_t *thread, task_t *owner);
void remove_process(task_t *proc);
void remove_thread(task_t *thread);

/* Hardware specific */
ptr_t task_init_trapframe(task_t *task, ptr_t sp);
void task_bootstrap_context(task_t *task, ptr_t sp);
void set_task_user(task_t *task);
void set_task_enable_intr(task_t *task);
int set_task_ustack(task_t *task);
addr_t set_task_argv(task_t *task, int argc, char *argv[]);
void set_task_ustacktop(task_t *task, ptr_t sp);
void set_task_main_args(task_t *task, int argc, char *argv[]);
void set_task_entry(task_t *task, addr_t entry);
/* Hardware independent */
task_t *task_new(void);
int task_setup_mm(task_t *task);
ptr_t task_setup_kstack(task_t *task);

int task_load_elf_kmem(task_t *task, void *addr, addr_t *entry);

void initproc_init(int argc, char *argv[]);
void idle_init(void);

void task_init(void);

extern task_t *idleproc, *initproc;

#endif
