/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _SCHED_SCHED_H
#define _SCHED_SCHED_H

#include <asm/smp.h>
#include <sched/task.h>
#include <ds/list.h>

struct cpu_run_queue {
	list_node_t	head[2];
	unsigned int	cpu;
	unsigned short	run;		/* The index of running queue head */
	unsigned short	exhaust;	/* The index of exhausted queue head */
	unsigned long	payload;	/* Total payload per run queue */
};

extern struct cpu_run_queue cpu_rq[];

int sched_init(void);
int sched_enqueue(struct cpu_run_queue *rq, task_t *task);
int sched_dequeue(struct cpu_run_queue *rq, task_t *task);
task_t *sched_pick(struct cpu_run_queue *rq);
int sched_tick(task_t *task);

void switch_task(task_t *newtask);
void switch_context(context_t *old, context_t *new, ptr_t newksp);

void sched(void);

#endif
