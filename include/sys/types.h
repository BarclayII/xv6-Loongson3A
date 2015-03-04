/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#ifndef __ASSEMBLER__
typedef unsigned char uint8, uchar, __u8;
typedef signed char __s8;
typedef unsigned short uint16, ushort, __u16;
typedef signed short __s16;
typedef unsigned int uint32, uint, __u32;
typedef signed int __s32;
typedef unsigned long ulong;
typedef unsigned long long uint64, __u64, u64;
typedef signed long long int64, __s64;

/* unsigned long integers defined as ptr_t are only for manipulating addresses */
typedef unsigned long addr_t;

typedef void *genericptr_t, *ptr_t;

typedef int pid_t;	/* PID, -1 = invalid, 0 = idle, 1 = init, -2 = kernel */
typedef unsigned short asid_t;

typedef unsigned int bool;
#define false	0
#define true	1

typedef unsigned long size_t;
typedef signed long ssize_t;

typedef bool intr_flag_t;

/*
 * This macro is a wrapper for casting integer constants to unsigned longs,
 * freeing the code from compiler warnings and assembler errors (GCC
 * warns about shift count if a "UL" suffix is not appended while GAS
 * refuses to recognize the "UL" suffix).
 */
#define ULCAST(i)	(i##UL)
#else	/* __ASSEMBLER__ */
#define ULCAST(i)	i
#endif	/* !__ASSEMBLER__ */

#endif
