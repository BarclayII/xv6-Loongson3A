/*
 * Copyright (c) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * this program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the gnu general  public license as published by the
 * free software foundation;  either version 2 of the  license, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_BOOTINFO_H
#define _ASM_BOOTINFO_H

#ifndef __ASSEMBLER__
/* Parameters in register a0-a3 stored by PMON */
extern unsigned long fw_arg0, fw_arg1, fw_arg2, fw_arg3;
extern unsigned long memsize, highmemsize, cpu_clock_freq, bus_clock_freq;
extern unsigned long memlimit;	/* total memory size rounded up to power of 2 */
#endif

#endif
