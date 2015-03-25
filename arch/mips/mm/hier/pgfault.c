/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/cp0regdef.h>
#include <asm/ptrace.h>
#include <asm/addrspace.h>
#include <asm/trap.h>
#include <asm/thread_info.h>
#include <asm/mm/tlb.h>
#include <sched.h>

int handle_pgfault(struct trapframe *tf)
{
	/* ASID switching is done with context switching */
	printk("Handling page fault with trapframe %016x\r\n", tf);
	dump_trapframe(tf);
	unsigned long entryhi = tf->cp0_entryhi;
	unsigned long asid = entryhi & ENTHI_ASID_MASK;
	entryhi ^= asid;
	/* First, check if current ASID is valid */
	/* Tasks are not fully implemented yet */
	/* assert(current_task->asid == asid); */
	if (asid == ASID_INVALID) {
		printk("Caught invalid ASID\r\n");
		if (current_next_asid == ASID_INVALID) {
			/* No ASID available, flush TLB and reset ASID */
			printk("Available ASID Exhausted\r\n");
			tlb_flush_all();
			asid_flush();
			printk("ASID refresh complete\r\n");
		}
		/* assign a new ASID to current task */
		printk("Assigning new asid %d\r\n", current_next_asid);
		current_task->asid = asid = current_next_asid++;
		/* register the PGD as online */
		printk("Registering ASID... PGD = %016x\r\n",
		    current_task->mm->arch_mm.pgd);
		current_online_hpt[asid] = current_task->mm->arch_mm.pgd;
		current_online_tasks[asid] = current_task;
		/* write the new ASID back into ENTRYHI */
		tf->cp0_entryhi = entryhi | asid;
		return 0;
	}
	return -1;
}
