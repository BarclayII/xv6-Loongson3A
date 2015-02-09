/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/trap.h>
#include <string.h>
#include <printk.h>
#include <panic.h>

unsigned long except_handlers[32];
extern unsigned long except_generic;

void trap_init(void)
{
	unsigned long ebase = read_c0_ebase();
	ebase = (0xffffffff00000000) | ((ebase | 0x3ff) ^ 0x3ff);
	memcpy((void *)(ebase + 0x180), &except_generic, 0x80);
}

static const char *regname[] = {
	"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	"t8", "t9", "k0", "k1", "gp", "sp", "s8", "ra"
};

void dump_trapframe(struct trapframe *tf)
{
	int i;
	for (i = _ZERO; i <= _RA; ++i) {
		printk("%s\t%016x\r\n", regname[i], tf->gpr[i]);
	}
	printk("STATUS\t= %08x\r\n", tf->cp0_status);
	printk("CAUSE\t= %08x\r\n", tf->cp0_cause);
	printk("EPC\t= %08x\r\n", tf->cp0_epc);
}

void handle_exception(struct trapframe *tf)
{
	/* do nothing... */
	printk("Caught exception!\r\n");
	dump_trapframe(tf);
	panic("SUSPENDING SYSTEM...\r\n");
}
