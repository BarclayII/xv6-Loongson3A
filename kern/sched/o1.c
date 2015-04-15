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
#include <ds/list.h>
#include <assert.h>

/*
 * A simplified O(1) scheduler
 *
 * The tasks in the running queue regularly executes and yields until their
 * time slices become exhausted, after which they're enqueued into an
 * "exhausted" queue.  After all tasks in the running queue have finished
 * their time slices, the exhausted queue becomes the new running queue,
 * with all tasks having their time slices replenished.
 *
 * The switching between two queues should take place transparently, and the
 * tasks calling the scheduler should not notice the switching happening.
 */

struct cpu_run_queue cpu_rq[NR_CPUS];

static inline int sched_exhaust(task_t *task)
{
	return task->se.priority >= task->se.slice;
}

void sched_init(void)
{
	int i;

	memset(cpu_rq, 0, sizeof(cpu_rq));

	for (i = 0; i < NR_CPUS; ++i) {
		cpu_rq[i].cpu = i;
		cpu_rq[i].run = 0;
		cpu_rq[i].exhaust = 1;
		list_init(&(cpu_rq[i].head[0]));
		list_init(&(cpu_rq[i].head[1]));
	}
}

int sched_enqueue(struct cpu_run_queue *rq, task_t *task)
{
	assert(task->se.rq == NULL);
	int i = sched_exhaust(task) ? rq->exhaust : rq->run;
	list_add_before(&(rq->head[i]), &(task->se.node));
	++(rq->payload);
	task->se.rq = rq;

	return 0;
}

int sched_dequeue(task_t *task)
{
	assert(task->se.rq != NULL);
	--(task->se.rq->payload);
	list_del_init(&(task->se.node));
	task->se.rq = NULL;

	return 0;
}

task_t *sched_pick(struct cpu_run_queue *rq)
{
	unsigned short tmp;

	/* Switch two queues if the running queue is empty */
	if (list_empty(&(rq->head[rq->run]))) {
		if (list_empty(&(rq->head[rq->exhaust]))) {
			return NULL;
		} else {
			tmp = rq->run;
			rq->run = rq->exhaust;
			rq->exhaust = tmp;
		}
	}
	return node_to_task(list_next(&(rq->head[rq->run])));
}

int sched_tick(task_t *task)
{
	if (!sched_exhaust(task))
		++(task->se.slice);
	task->state |= TASK_YIELDED;	/* Always yields */

	return 0;
}

