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
#include <drivers/uart16550.h>
#include <drivers/prom_printk.h>

int main(void)
{
	prom_printk("Hello Loongson 3A!\r\n");
	prom_printk("Testing: %c %08x %016x %08x %u\r\n",
	    'a',
	    256,
	    -1,
	    -1,
	    -1);
	for (;;)
		/* echo characters */
		Uart16550Put(Uart16550GetPoll());
}
