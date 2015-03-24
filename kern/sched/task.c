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
#include <asm/thread_info.h>
#include <sched/task.h>
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

void initproc_init(int argc, char *argv[])
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
	ptr_t usp = (ptr_t)set_task_argv(initproc, argc, argv);
	set_task_ustacktop(initproc, usp);
	set_task_main_args(initproc, argc, (char **)usp);
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
	idleproc->pid = PID_IDLE;
	idleproc->kstack = &init_stack;
	strlcpy(idleproc->name, "IDLE", PROC_NAME_LEN_MAX);

	idleproc->state = TASK_RUNNABLE;

	add_process(idleproc, idleproc);
}

extern void dump_trapframe(trapframe_t *tf);

void dump_task(task_t *task)
{
	printk("PID\t%d\r\n", task->pid);
	printk("State\t%04x\r\n", task->state);
	printk("Flags\t%08x\r\n", task->flags);
	if (task->pid != PID_IDLE) {
		printk("Context\r\n");
		dump_trapframe((trapframe_t *)(task->context));
		printk("Trapframe\r\n");
		dump_trapframe((trapframe_t *)(task->tf));
	}
	printk("KSTACK\t%016x\r\n", task->kstack);
	if (task->pid != PID_IDLE) {
		printk("MM\t%016x\r\n", task->mm);
		printk("PGD\t%016x\r\n", task->mm->arch_mm.pgd);
		printk("PROGTOP   %016x\r\n", task->progtop);
		printk("USTACKTOP %016x\r\n", task->ustacktop);
	}
}

void task_init(void)
{
	tasklist_init();

	idle_init();
	initproc_init(0, NULL);

	dump_task(idleproc);
	dump_task(initproc);
}
