/*
 * Processor capabilities determination functions.
 *
 * Copyright (C) xxxx  the Anonymous
 * Copyright (C) 1994 - 2006 Ralf Baechle
 * Copyright (C) 2003, 2004  Maciej W. Rozycki
 * Copyright (C) 2001, 2004  MIPS Inc.
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <asm/cpu.h>
#include <asm/cp0regdef.h>
#include <panic.h>

/*
 * CPU information array
 */
struct cpuinfo_mips cpu_data[NR_CPUS];

/*
 * Kernel SP per CPU
 */
unsigned long kernelsp[NR_CPUS];

void cpu_probe(void)
{
	struct cpuinfo_mips *c = &current_cpu_data;

	c->processor_id = read_c0_prid();
	if (c->processor_id != PRID_IMP_LOONGSON3A) {
		panic("assertation failed: prid != LOONGSON_3A5\r\n");
	}

	c->cputype = CPU_LOONGSON3;
	c->core = read_c0_ebase() & EBASE_CPUNUM_MASK;

	smp_processor_id() = c->core;

	c->tlbsize = 64;
	c->isa_level = MIPS_CPU_ISA_III;
	c->options = MIPS_CPU_TLB | MIPS_CPU_4KEX | MIPS_CPU_4K_CACHE |
		MIPS_CPU_COUNTER | MIPS_CPU_FPU | MIPS_CPU_LLSC | MIPS_CPU_32FPR;
}
