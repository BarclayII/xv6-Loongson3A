/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <mm/vmm.h>
#include <asm/cpu.h>
#include <asm/mm/pgtable.h>
#include <asm/mm/tlb.h>
#include <asm/mipsregs.h>
#include <printk.h>
#include <sched/task.h>

task_t *online_task[ASID_MAX + 1];

static void init_tlb(void)
{
	printk("TLB entries: %d (pairs)\r\n", current_cpu_data.tlbsize);
	tlb_flush_all();
}

void arch_mm_init(void)
{
#ifdef CONFIG_16KPAGES
	write_c0_pagemask(3 << 13);
#endif
	memset(online_task, 0, sizeof(online_task));
	pgtable_bootstrap();
	init_tlb();
}
