/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/ptrace.h>
#include <asm/cp0regdef.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <asm/thread_info.h>
#include <asm/syscalldefs.h>
#include <sched.h>
#include <sys/types.h>
#include <string.h>

ptr_t task_init_trapframe(task_t *task, ptr_t sp)
{
	sp -= sizeof(*(task->tf));
	task->tf = (trapframe_t *)sp;
	trapframe_t *tf = task->tf;
	memset(tf, 0, sizeof(*tf));
	tf->cp0_status = read_c0_status();
	tf->cp0_cause = read_c0_cause();
	tf->cp0_entryhi = ASID_INVALID;
	tf->gpr[_GP] = (unsigned long)&init_thread_union;
	/* not setting up EPC yet; this is done after locating entry and
	 * performed inside set_task_entry() */
	return sp;
}

/*
 * task_bootstrap_context() redirects control to forkret(), a function
 * which further redirects control to program entry by restoring tweaked
 * trapframe.
 */
void task_bootstrap_context(task_t *task, ptr_t sp)
{
	context_t *ctx = task->context;
	memset(ctx, 0, sizeof(*ctx));
	ctx->cp0_status = read_c0_status();
	ctx->cp0_cause = read_c0_cause();
	ctx->gpr[_A0] = (unsigned long)task->tf;
	ctx->gpr[_RA] = (unsigned long)forkret;
	ctx->gpr[_SP] = (unsigned long)sp;
	ctx->gpr[_GP] = (unsigned long)&init_thread_union;
}

void set_task_user(task_t *task)
{
	trapframe_t *tf = task->tf;
	/* CP0 register have to be disabled for user processes */
	tf->cp0_status &= ~ST_CU0;
	tf->cp0_status &= ~ST_EXCM;
	tf->cp0_status |= KSU_USER | ST_EXL | ST_PX | ST_UX;
}

void set_task_enable_intr(task_t *task)
{
	trapframe_t *tf = task->tf;
	tf->cp0_status |= ST_IE;
}

void set_task_startsp(task_t *task, addr_t startsp)
{
	task->tf->gpr[_SP] = startsp;
}

/*
 * Setup arguments passed into process, if applicable.
 * Pushes argument strings as well as their address on top of user stack.
 * Returns the heap bottom.
 *
 * @argv[] and its elements should reside in kernel space.
 */
addr_t set_task_argv(task_t *task, int argc, char *const argv[])
{
	int i;
	size_t argv_space = argc * 8;

	/* Space occupied by one string @s is actually
	 * POW2_ROUNDUP(strlen(s) + 1, 3)
	 * since the address should be aligned to 8 bytes, and the null-
	 * terminator '\0' itself takes one byte. */

	/* Push strings and their addresses one by one */
	ptr_t argvtop = task->ustacktop;
	ptr_t strtop = argvtop + argv_space;
	size_t strspace;
	for (i = 0; i < argc; ++i) {
		strspace = POW2_ROUNDUP(strlen(argv[i]) + 1, 3);
		if (copy_to_uvm(task->mm, strtop, argv[i], strspace) != 0)
			return 0;
		if (copy_to_uvm(task->mm, argvtop, &strtop, sizeof(ptr_t)) != 0)
			return 0;
		argvtop += 8;
		strtop += strspace;
	}

	return (addr_t)strtop;
}

void set_task_main_args(task_t *task, int argc, char *const argv[])
{
	task->tf->gpr[_A0] = (unsigned long)argc;
	task->tf->gpr[_A1] = (unsigned long)argv;
	printk("ARGC: %d\r\n", argc);
	printk("ARGV: %016x\r\n", argv);
}

void set_task_entry(task_t *task, addr_t entry)
{
	trapframe_t *tf = task->tf;
	tf->cp0_epc = entry;
}
