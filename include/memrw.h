/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _MEMRW_H
#define _MEMRW_H

#include <sys/types.h>

#define read_mem_char(addr)		(*((volatile char *)(addr)))
#define write_mem_char(addr, val)	(*((volatile char *)(addr)) = (val))

#define read_mem_byte(addr)		(*((volatile uchar *)(addr)))
#define write_mem_byte(addr, val)	(*((volatile uchar *)(addr)) = (val))

#define read_mem_int(addr)		(*((volatile int *)(addr)))
#define write_mem_int(addr, val)	(*((volatile int *)(addr)) = (val))

#define read_mem_uint(addr)		(*((volatile uint *)(addr)))
#define write_mem_uint(addr, val)	(*((volatile uint *)(addr)) = (val))

#define read_mem_long(addr)		(*((volatile long *)(addr)))
#define write_mem_long(addr, val)	(*((volatile long *)(addr)) = (val))

#define read_mem_ulong(addr)		(*((volatile ulong *)(addr)))
#define write_mem_ulong(addr, val)	(*((volatile ulong *)(addr)) = (val))

#endif
