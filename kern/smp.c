/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <smp.h>
#include <sys/types.h>
#include <string.h>

bool cpu_online[NR_CPUS];

void smp_init(void)
{
	memset(cpu_online, 0, sizeof(cpu_online));
	cpu_online[smp_processor_id()] = 1;
}
