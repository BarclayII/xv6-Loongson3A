/*
 * Copyright (C) xxxx Anonymous <http://github.com/chyyuu/ucore_lab>
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/ptrace.h>
#include <asm/trap.h>
#include <asm/thread_info.h>
#include <sched.h>
#include <mm/kmalloc.h>
#include <mm/vmm.h>
#include <string.h>
#include <stddef.h>
#include <ds/list.h>

task_set_t task_set;

task_t *idleproc, *initproc;

task_t *task_new(void)
{
	task_t *task;

	task = kmalloc(sizeof(*task));
	if (task != NULL) {
		memset(task, 0, sizeof(*task));

		task->flags = PF_STARTING;

		task->parent = task;
		task->thgroup_leader = task;

		list_init(&(task->proc_sib));
		list_init(&(task->thread_node));
		list_init(&(task->proc_node));
		list_init(&(task->hash_node));
	}

	return task;
}

int task_setup_mm(task_t *task)
{
	int retcode;
	task->mm = mm_new();
	retcode = arch_mm_new_pgtable(&(task->mm->arch_mm));
	return retcode;
}

/*
 * Setup kernel stack
 */
ptr_t task_setup_kstack(task_t *task)
{
	task->kstack = kmalloc(KSTACK_SIZE);
	if (task->kstack == NULL)
		return NULL;
	ptr_t sp = task->kstack + KSTACK_SIZE;
	/* Reserve a portion for storing process context */
	sp -= sizeof(*(task->context));
	task->context = (context_t *)sp;
	return sp;
}

int set_task_ustack(task_t *task)
{
	int ret;
	struct page *p = pgalloc();
	ret = map_pages(task->mm, (addr_t)task->progtop, p, USTACK_PERM);
	if (ret != 0)
		goto rollback_page;
	/* Set the starting sp inside trapframe */
	task->ustacktop = task->progtop + USTACK_SIZE;
	set_task_startsp(task, (addr_t)(task->ustacktop));
	return 0;

rollback_page:
	pgfree(p);
	return ret;
}

void initproc_init(int argc, char *const argv[])
{
	printk("Spawning init...\r\n");
	printk("\tArgument count: %d\r\n", argc);
	printk("\tArguments: ");
	int i;
	for (i = 0; i < argc; ++i)
		printk("\"%s\" ", argv[i]);
	printk("\r\n");

	initproc = task_new();
	if (initproc == NULL)
		panic("failed to allocate process for init\r\n");

	/* init runs in user mode */
	assert(task_setup_mm(initproc) == 0);
	ptr_t ksp = task_setup_kstack(initproc);
	assert(ksp != NULL);
	ksp = task_init_trapframe(initproc, ksp);
	task_bootstrap_context(initproc, ksp);
	set_task_user(initproc);
	set_task_enable_intr(initproc);

	/* Directly referencing this symbol actually returns the content
	 * inside, damn it. */
	extern unsigned long _binary_ramdisk_init_init_start;

	ptr_t init_start = &_binary_ramdisk_init_init_start;
	addr_t entry;
	int ret;

	printk("Loading init from ramdisk at %016x...\r\n", init_start);
	ret = task_load_elf_kmem(initproc, init_start, &entry);
	printk("Init entry virtual address %016x\r\n", entry);
	if (ret != 0)
		panic("init spawning failed with code %d\r\n", ret);

	assert(set_task_ustack(initproc) == 0);
	set_task_main_args(initproc, argc, (char **)initproc->ustacktop);
	ptr_t utop = (ptr_t)set_task_argv(initproc, argc, argv);
	assert(utop != 0);
	initproc->ustacktop = utop;
	set_task_entry(initproc, entry);

	initproc->pid = PID_INIT;
	strlcpy(initproc->name, "init", PROC_NAME_LEN_MAX);

	initproc->state = TASK_RUNNABLE;

	add_process(initproc, initproc);
}

void idle_init(void)
{
	printk("Spawning IDLE....\r\n");
	idleproc = task_new();
	if (idleproc == NULL)
		panic("failed to spawn IDLE\r\n");
	idleproc->mm = &kern_mm;
	idleproc->pid = PID_IDLE;
	idleproc->kstack = &init_stack;
	idleproc->context = &current_idle_context;
	strlcpy(idleproc->name, "IDLE", PROC_NAME_LEN_MAX);

	idleproc->state = TASK_RUNNABLE;

	add_process(idleproc, idleproc);
}

void dump_task(task_t *task)
{
	printk("PID\t%d\r\n", task->pid);
	printk("State\t%04x\r\n", task->state);
	printk("Flags\t%08x\r\n", task->flags);
	if (task->pid != PID_IDLE) {
		printk("Context %016x\r\n", task->context);
		printk("Trapframe %016x\r\n", task->tf);
	}
	printk("KSTACK\t%016x\r\n", task->kstack);
	if (task->pid != PID_IDLE) {
		printk("MM\t%016x\r\n", task->mm);
		printk("PGD\t%016x\r\n", task->mm->arch_mm.pgd);
		printk("PROGTOP   %016x\r\n", task->progtop);
		printk("USTACKTOP %016x\r\n", task->ustacktop);
	}
}

char *const initargv[] = {
	"init",
	"sample",
	"arguments"
};

void task_init(void)
{
	tasklist_init();

	idle_init();
	initproc_init(3, initargv);

	dump_task(idleproc);
	dump_task(initproc);

	current_task = idleproc;
}

void task_init_sched(void)
{
	sched_enqueue(&current_rq, initproc);
}
