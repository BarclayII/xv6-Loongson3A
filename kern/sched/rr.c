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
	}
	return 0;
}

