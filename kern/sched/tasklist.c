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

#include <sched/task.h>
#include <ds/list.h>
#include <assert.h>

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

void tasklist_init(void)
{
	int i;
	list_init(&(process_list));
	for (i = 0; i < HASH_LIST_SIZE; ++i)
		list_init(&(process_hash_list[i]));
}

/* For adding a separate process */
void add_process(task_t *proc, task_t *parent)
{
	assert(task_is_process(proc));
	assert(task_is_process(parent));

	int hash = pid_hash(proc->pid);
	int i;

	list_add_before(&process_list, &(proc->proc_node));
	list_add_before(&(process_hash_list[hash]), &(proc->hash_node));

	/* Maintain process tree */
	proc->parent = parent;
	if (parent != proc) {
		task_t *fchild = parent->first_child;
		if (fchild == NULL) {
			parent->first_child = proc;
			proc->num_sibling = 1;
		} else {
			task_t *t = fchild;
			for (i = 0; i < fchild->num_sibling; ++i) {
				++(t->num_sibling);
				t = next_sib_task(t);
			}
			list_add_before(&(fchild->proc_sib), &(proc->proc_sib));
			proc->num_sibling = fchild->num_sibling;
		}
	}
	++(parent->num_child);
	++nr_process;
}

/* For adding a separate thread into thread list of one process */
void add_thread(task_t *thread, task_t *owner)
{
	assert(thread->pid == owner->pid);
	assert(task_is_process(owner));
	assert(!task_is_process(thread));

	thread->thgroup_leader = owner;
	list_add_before(&(owner->thread_node), &(thread->thread_node));
	++(owner->num_threads);
	++nr_thread;
}

/* NOTE: only removes process itself from process tree, does not remove and
 *       destroy its threads.  destroy_process() destroys the process and
 *       all owned threads, releasing resources being held. */
void remove_process(task_t *proc)
{
	assert(task_is_process(proc));
	assert(proc->pid != PID_IDLE && proc->pid != PID_INIT);

	int i;
	task_t *t;

	list_del_init(&(proc->proc_node));
	list_del_init(&(proc->hash_node));

	/* Modify parent's first child entry */
	task_t *parent = proc->parent;
	if (parent->first_child == proc) {
		list_node_t *sib = &(proc->proc_sib);
		if (list_single(sib))
			parent->first_child = NULL;
		else
			parent->first_child = sibnode_to_proc(list_next(sib));
	}
	--(parent->num_child);

	/* Delete process from list of its siblings */
	t = proc;
	for (i = 0; i < proc->num_sibling; ++i) {
		--(t->num_sibling);
		t = next_sib_task(t);
	}
	list_del_init(&(proc->proc_sib));

	/* Children are taken over by init */
	task_t *child = proc->first_child, *ichild = initproc->first_child;
	unsigned int nr_sibs = task_num_sibling(child);
	unsigned int nr_isibs = task_num_sibling(ichild);
	if (child != NULL) {
		/* Modify the children's parent to init */
		for (i = 0; i < nr_sibs; ++i) {
			child->parent = initproc;
			child->num_sibling = nr_sibs + nr_isibs;
			child = next_sib_task(child);
		}
		if (ichild == NULL) {
			initproc->first_child = child;
		} else {
			for (i = 0; i < nr_isibs; ++i) {
				ichild->num_sibling = nr_sibs + nr_isibs;
				ichild = next_sib_task(ichild);
			}
			list_concat(&(child->proc_sib), &(ichild->proc_sib));
		}
	}

	--nr_process;
}

/* NOTE: does not actually free resources being held by this thread. */
void remove_thread(task_t *thread)
{
	assert(!task_is_mainthread(thread));

	task_t *owner = thread->thgroup_leader;
	--(owner->num_threads);
	list_del_init(&(thread->thread_node));
	--nr_thread;
}

