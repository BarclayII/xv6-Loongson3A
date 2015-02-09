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
#include <stddef.h>
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

static const char *ex_desc[] = {
	[EC_int]	= "Interrupt Pending",
	[EC_tlbm]	= "TLB Modified",
	[EC_tlbl]	= "TLB Loading Miss",
	[EC_tlbs]	= "TLB Storing Miss",
	[EC_adel]	= "Address Error on Loading",
	[EC_ades]	= "Address Error on Storing",
	[EC_ibe]	= "Bus Error while Fetching Instruction",
	[EC_dbe]	= "Bus Error while Loading/Storing Data",
	[EC_sys]	= "System Call",
	[EC_bp]		= "Breakpoint",
	[EC_ri]		= "Reserved/Illegal Instruction",
	[EC_cpu]	= "Coprocessor Unusable",
	[EC_ov]		= "Arithmetic Overflow",
	[EC_tr]		= "Trap Instruction",
	[EC_fpe]	= "Floating-Point Exception",
	[EC_is]		= "Stack Exception",
	[EC_dib]	= "Debug Instruction Exception",
	[EC_ddbs]	= "Debug Data Storing Exception",
	[EC_ddbl]	= "Debug Data Loading Exception",
	[EC_watch]	= "Referencing WatchHi/WatchLo",
	[EC_dbp]	= "Debug Breakpoint",
	[EC_dint]	= "Debug Interrupt",
	[EC_dss]	= "Debug Step",
	[EC_cacheerr]	= "Cache Error"
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

static unsigned long long ticks = 0;
static unsigned int compare = 0;

static int handle_int(struct trapframe *tf)
{
	unsigned int cause = tf->cp0_cause;
	if ((cause & CR_IPx(7)) && (cause & CR_TI)) {
		compare += 0x10000000;
		write_c0_compare(compare);
		printk("ticks\t= %d\r\n", ticks);
		++ticks;
		return 0;
	}
	return -1;
}

void handle_exception(struct trapframe *tf)
{
	int exccode = EXCCODE(tf->cp0_cause);
	switch (exccode) {
	case EC_int:
		if (handle_int(tf) == 0)
			break;
		/* else fallthru */
	default:
		printk("Caught exception %s (%d)\r\n",
		    (ex_desc[exccode] == NULL) ? "(unknown)" : ex_desc[exccode],
		    exccode);
		dump_trapframe(tf);
		panic("SUSPENDING SYSTEM...\r\n");
		break;
	}
}