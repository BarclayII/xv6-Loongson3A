/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/thread_info.h>
#include <printk.h>
#include <time.h>
#include <sched.h>

void generic_cpu_tick(void)
{
	printk("CPUID %d\tticks\t= %d\tPID\t=%d\r\n",
	    current_thread_info->cpu_number,
	    current_thread_info->ticks,
	    current_task->pid);
	++(current_thread_info->ticks);
}
