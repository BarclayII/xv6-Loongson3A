/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _PRINTK_H
#define _PRINTK_H

#include <stdarg.h>
#include <config.h>

extern int early_printk;

int printk(const char *fmt, ...);
int vprintk(const char *fmt, va_list ap);

#ifdef DEBUG
#define pdebug(fmt, args...)	do { printk(fmt, args); } while (0)
#define vpdebug(fmt, ap)	do { vprintk(fmt, ap); } while (0)
#else
#define pdebug(fmt, args...)	do { } while (0)
#define vpdebug(fmt, ap)	do { } while (0)
#endif

#endif
