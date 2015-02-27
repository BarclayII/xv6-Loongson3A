/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASSERT_H
#define _ASSERT_H

#include <asm/assert.h>
#include <printk.h>

/*
 * Print an assertation failed message and throw a hardware exception.
 */
#define assert(x)	do { \
	if (!(x)) { \
		printk("Assertation failed: %s\r\n", #x); \
		__halt(); \
	} \
} while (0)

#endif
