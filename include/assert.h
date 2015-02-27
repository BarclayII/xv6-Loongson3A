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

#include <panic.h>

#define assert(x)	do { \
	if (!(x)) \
		panic("Assertation failed: %s\r\n", #x); \
} while (0)

#endif
