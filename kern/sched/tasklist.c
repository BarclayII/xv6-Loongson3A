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

/* 2^31 + 2^29 - 2^25 + 2^22 - 2^19 - 2^16 + 1 */
#define GOLDEN_RATIO_PRIME_32       0x9e370001UL

/* *
 * hash32 - generate a hash value in the range [0, 2^@bits - 1]
 * @val:    the input value
 * @bits:   the number of bits in a return value
 *
 * High bits are more random, so we use them.
 * */
static inline unsigned int hash32(unsigned int pid, unsigned int bits)
{
	unsigned int hash = pid * GOLDEN_RATIO_PRIME_32;
	return hash >> (32 - bits);
}

static inline int pid_hash(int pid)
{
	return (int)hash32((unsigned int)pid, HASH_LIST_ORDER);
}

/* For adding a separate process */
void add_process(task_t *proc, task_t *parent)
{
	assert(task_is_process(proc));
	assert(task_is_process(parent));
	int pid_hash = pid_hash(proc->pid);
	list_add_before(&process_list, &(proc->proc_node));
	list_add_before(&(process_hash_list[pid_hash]), &(proc->proc_node));

	/* Maintain process tree */
	proc->parent = parent;
	if (parent != proc) {
		task_t *fchild = parent->first_child;
		if (fchild == NULL)
			parent->first_child = proc;
		else
			list_add_before(&(fchild->proc_sib), &(proc->proc_sib));
	}
	++nr_process;
}

/* For adding a separate thread into thread list of one process */
void add_thread(task_t *thread, task_t *owner)
{
	assert(thread->pid == owner->pid);
	assert(task_is_process(owner));
	assert(!task_is_process(thread));
	thread->thgroup_leader = owner_main;
	list_add_before(&(owner_main->thread_node), &(thread->thread_node));
	++nr_thread;
}

/* NOTE: only removes process itself from process tree, does not remove and
 *       destroy its threads.  destroy_process() destroys the process and
 *       all owned threads, releasing resources being held. */
void remove_process(task_t *proc)
{
	assert(task_is_process(proc));
}
