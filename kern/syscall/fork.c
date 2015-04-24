/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/syscalldefs.h>
#include <asm/ptrace.h>
#include <asm/trap.h>
#include <asm/cpu.h>
#include <printk.h>
#include <panic.h>

void forkret(struct trapframe *tf)
{
	pdebug("Entering forkret with trapframe %016x\r\n", tf);
	arch_forkret(tf);
	panic("Returned from arch_forkret???\r\n");
}

int do_fork(task_t *parent, task_t **child)
{
	int ret;
	task_t *child = task_new();
	if ((ret = task_early_init(child)) != 0)
		return ret;

	if ((ret = dup_uvm(parent->mm, child->mm)) != 0)
		return ret;
}
