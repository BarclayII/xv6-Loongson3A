/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <sched.h>
#include <string.h>

struct cpu_run_queue cpu_rq[NR_CPUS];

int sched_init(void)
{
	int i;

	memset(cpu_rq, 0, sizeof(cpu_rq));

	for (i = 0; i < NR_CPUS; ++i) {
		cpu_rq[i].cpu = i;
		list_init(&(cpu_rq[i].head));
	}
	return 0;
}

int sched_enqueue(struct cpu_run_queue *rq, task_t *task)
{
	assert(task->se.rq == NULL);
	list_add_before(&(rq->head), &(task->se.node));
	task->se.rq = rq;

	return 0;
}

int sched_dequeue(struct cpu_run_queue *rq, task_t *task)
{
	assert(task->se.rq != NULL);
	list_del_init(&(task->se.node));
	task->se.rq = NULL;

	return 0;
}

task_t *sched_pick(struct cpu_run_queue *rq)
{
	if (list_empty(&(rq->head)))
		return NULL;
	return node_to_task(list_next(&(rq->head)));
}

int sched_tick(task_t *task)
{
	++(task->se.slice);
	if (task->se.slice == task->se.priority)
		return 1;
	else
		return 0;
}

