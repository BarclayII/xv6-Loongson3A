/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _KERNEL_PRINTK_H
#define _KERNEL_PRINTK_H

#include <stdarg.h>

int prom_printk(const char *fmt, ...);
int prom_vprintk(const char *fmt, va_list ap);
void prom_puts(const char *s);

#endif
