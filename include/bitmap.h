/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _BITMAP_H
#define _BITMAP_H

#include <asm/bitops.h>
#include <string.h>

#define DECLARE_BITMAP(name, bits) \
	unsigned int name[BITS_TO_INTS(bits)]

#define CLEAR_BITMAP(name)	memset(name, 0, sizeof(name))

#endif
