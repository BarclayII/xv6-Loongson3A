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

task_group_t task_group;

task_t *task_new(void)
{
	task_t *task;
	void *sp;

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
 * Setup kernel stack and trapframe
 */
int task_setup_kstack(task_t *task)
{
	task->kstack = kmalloc(KSTACK_SIZE);
	if (task->kstack == NULL)
		return -ENOMEM;
	ptr_t sp = task->kstack + KSTACK_SIZE;
	task->tf = (trapframe_t *)(sp - sizeof(*(task->tf)));
	return 0;
}

task_t *initproc_init(int argc, char *argv[])
{
	/* init runs in user mode */
	task_t *init = task_new();
	task_setup_mm(init);
	task_setup_kstack(init);
	task_init_trapframe(init);
	task_bootstrap_context(init);
	set_task_user(init);
	set_task_enable_intr(init);

	/* TODO: get entry of init process */
	extern void *_binary_ramdisk_init_init_start;
	extern size_t _binary_ramdisk_init_init_size;
	addr_t entry;
	int ret;

	ret = task_load_elf_kmem(init, _binary_ramdisk_init_init_start, &entry);
	if (ret != 0)
		panic("init spawning failed with code %d\r\n", ret);

	set_task_ustack(init);
	ptr_t sp = set_task_argv(init, argc, argv);
	set_task_ustacktop(init, sp);
	set_task_main_args(init, argc, (char **)sp);
	set_task_entry(init, entry);

	init->pid = 1;
	strlcpy(init->name, "init", PROC_NAME_LEN_MAX);

	init->flags = PF_RUNNING;

	add_process(init, init);
}

void idle_init(void)
{
	task_t *idle;

	idle = task_new();
	if (idle == NULL)
		panic("failed to spawn IDLE\r\n");
	idle->pid = 0;
	idle->kstack = &init_stack;
	strlcpy(idle->name, "IDLE", PROC_NAME_LEN_MAX);

	idle->flags = PF_RUNNING;

	add_process(idle, idle);
}

