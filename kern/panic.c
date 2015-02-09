/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/irq.h>
#include <panic.h>
#include <stdarg.h>
#include <printk.h>

void panic(const char *fmt, ...)
{
	va_list ap;

	local_irq_disable();

	va_start(ap, fmt);
	vprintk(fmt, ap);
	va_end(ap);


	for (;;)
		/* do nothing */;
}
