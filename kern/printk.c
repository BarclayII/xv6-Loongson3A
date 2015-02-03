/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <stdarg.h>
#include <printk.h>
#include <drivers/prom_printk.h>

int early_printk;

int printk(const char *fmt, ...)
{
	va_list ap;
	int result;
	va_start(ap, fmt);
	if (early_printk) {
		result = prom_vprintk(fmt, ap);
	} else {
		/* Not implemented yet */
		result = 0;
	}
	va_end(ap);
	return result;
}
