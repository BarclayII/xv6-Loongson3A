/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <drivers/prom_printk.h>
#include <drivers/uart16550.h>
#include <stdio.h>
#include <stdarg.h>

static char prom_printk_buf[BUFSIZ];

void prom_puts(const char *s)
{
	for ( ; *s != '\0'; ++s)
		Uart16550Put((unsigned char)*s);
}

int prom_printk(const char *fmt, ...)
{
	int result;
	va_list ap;
	va_start(ap, fmt);
	result = vsnprintf(prom_printk_buf, BUFSIZ, fmt, ap);
	prom_puts(prom_printk_buf);
	va_end(ap);
	return result;
}

int prom_vprintk(const char *fmt, va_list ap)
{
	int result;
	result = vsnprintf(prom_printk_buf, BUFSIZ, fmt, ap);
	prom_puts(prom_printk_buf);
	return result;
}
