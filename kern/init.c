/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <stdio.h>
#include <string.h>
#include <printk.h>
#include <ht_regs.h>
#include <bitmap.h>
#include <setup.h>
#include <drivers/uart16550.h>
#include <asm/memrw.h>
#include <asm/io.h>
#include <asm/mipsregs.h>
#include <asm/bootinfo.h>
#include <asm/cpu.h>
#include <asm/thread_info.h>
#include <asm/ptrace.h>
#include <asm/trap.h>
#include <asm/irq.h>

int main(void)
{
	unsigned int prid = read_c0_prid();
	unsigned long cputype_flag;
	int i;
	signed int *argv, *envp;
	char *arg;

	early_printk = 1;
	printk("Hello Loongson 3A!\r\n");
	printk("PRID: %08x\r\n", prid);
	printk("STATUS: %08x\r\n", read_c0_status());
	printk("CAUSE: %08x\r\n", read_c0_cause());
	printk("EBASE: %08x\r\n", read_c0_ebase());

	cputype_flag = read_mem_uint(HT_RX_BUFFER);
	printk("FLAG: %016x\r\n", cputype_flag);

	/*
	 * Here comes death trap from PMON where pointer is 32-bit long.
	 * We have to convert unsigned 32-bit pointers in argv[] to 64-bit ones.
	 */
	argv = (signed int *)fw_arg1;
	printk("ARGV: %p\r\n", argv);
	for (i = 0; i < fw_arg0; ++i) {
		printk("ARGV[%d]: %s\r\n", i, (char *)argv[i]);
	}

	envp = (signed int *)fw_arg2;
	printk("ENVP: %p %08x\r\n", envp, *envp);
	char *envs = (char *)*envp;
	for ( ; *envs != '\0'; ) {
		printk("\t%s\r\n", envs);
		envs += strlen(envs) + 1;
	}

	setup_arch();
	printk("Core number: %d\r\n", smp_processor_id());

	printk("FW_ARG3: %016x\r\n", *(unsigned long *)fw_arg3);

	unsigned int config0 = read_c0_config();
	/*
	 * Initial CONFIG = 0x80034483, which means both TLB and caches are
	 * on, initialized by either hardware or BIOS.
	 * Truly a relief on OS programmers...
	 */
	printk("CONFIG: %08x\r\n", config0);

	trap_init();

	mach_init_irq();

	local_irq_enable();

	asm volatile (
		"dli	$26, 0xc000000000000000;"
		"dmtc0	$26, $10;"
		"dli	$27, 0x40001e;"
		"dmtc0	$27, $2;"
		"daddiu	$27, 0x40;"
		"dmtc0	$27, $3;"
		"tlbwr"
	);

	long *sample_va = (long *)0xc000000000000000;
	memset(sample_va, '0', 4095);
	memset((char *)sample_va + 4096, '1', 4095);
	printk("sample_va = %016x\r\n", *sample_va);
	long *sample_shared_va = (long *)0xc000000000001000;
	printk("sample_shm= %016x\r\n", *sample_shared_va);

	for (;;)
		/* do nothing */;

	/* NOTREACHED */
	return 0;
}

