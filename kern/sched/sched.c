/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/smp.h>
#include <sys/types.h>
#include <sched.h>
#include <string.h>

/*
 * Consider an environment where only two processes are running.  The
 * process scheduling goes like this:
 * (ALERT: ASCII art below)
 *
 *                 proc1                  proc2
 *                   |                      |
 *                   |                      |
 *                   v                      |
 * Timer intr-->except_generic              |
 *                [enter]                   |
 *                   |                      |
 *                   |                      |
 *                   v                      |
 *         tf<---handle_int                 |
 *                [enter]                   |
 *                   |                      |
 *                   |                      |
 *                   v                      |
 *                 sched                    |
 *                [enter]                   |
 *                   |                      |
 *                   |                      |
 *                   v                      |
 *            switch_context                |
 *                [enter]                   |
 *                   \                      |
 *                    \                     |
 *                     \                    |
 *                      ------------>switch_context
 *                                       [exit]
 */

void switch_task(task_t *newtask)
{
	task_t *oldtask = current_task;
	context_t *old = oldtask->context;
	context_t *new = newtask->context;
	ptr_t newksp = kstacktop(newtask);
	current_task = newtask;
	switch_context(old, new, newksp);
}

void sched(void)
{
	/* TODO: add lock */
}
