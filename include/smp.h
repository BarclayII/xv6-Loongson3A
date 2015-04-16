/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _SMP_H
#define _SMP_H

#include <asm/thread_info.h>
#include <sys/types.h>

#define smp_processor_id()	(current_thread_info->cpu_number)
#define smp_current_task	(current_thread_info->task)

#define NR_CPUS			4

extern bool cpu_online[];
#define cpu_is_online(i)	(cpu_online[i])

void smp_init(void);
void smp_boot(void);

#endif
