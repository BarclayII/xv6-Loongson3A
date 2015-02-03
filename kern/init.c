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
#include <printk.h>
#include <drivers/uart16550.h>
#include <asm/cp0regdef.h>
#include <ht_regs.h>
#include <memrw.h>
#include <bitmap.h>
#include <asm/bootinfo.h>

int main(void)
{
	unsigned int prid = read_c0_prid();
	unsigned long cputype_flag;
	int i;
	signed int *argv;
	char *arg;

	early_printk = 1;
	printk("Hello Loongson 3A!\r\n");
	printk("Testing: %c %08x %016x %08x %u\r\n",
	    'a',
	    256,
	    -1,
	    -1,
	    -1);
	printk("PRID: %08x\r\n", prid);

	cputype_flag = read_mem_uint(HT_RX_BUFFER);
	printk("FLAG: %016x\r\n", cputype_flag);

	/*
	 * Here comes death trap from PMON where pointer is 32-bit long.
	 * We have to convert unsigned 32-bit pointers in argv[] to 64-bit ones.
	 */
	argv = (signed int *)fw_arg1;
	printk("ARGV: %p\r\n", argv);
	for (i = 0; i < fw_arg0; ++i) {
		arg = (char *)&(argv[i]);
		printk("ARGV[%d]: %s\r\n", i, argv[i]);
	}

	for (;;)
		/* echo characters */
		Uart16550Put(Uart16550GetPoll());

	/* NOTREACHED */
	return 0;
}

