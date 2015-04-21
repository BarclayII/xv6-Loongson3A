/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_ATOMIC_H
#define _ASM_ATOMIC_H

#include <sys/types.h>

static inline void atomic_incq(u64 *addr)
{
	u64 tmp;
	asm volatile (
		"	.set	push;"
		"	.set	mips3;"
		"	.set	reorder;"
		"1:	lld	%0, %1;"
		"	daddiu	%0, 1;"
		"	scd	%0, %1;"
		"	beqz	%0, 1b;"
		"	.set	pop;"
		: "=&r"(tmp), "=m"(*addr)
		: "m"(*addr)
	);
}

static inline void atomic_incd(u32 *addr)
{
	u32 tmp;
	asm volatile (
		"	.set	push;"
		"	.set	mips3;"
		"	.set	reorder;"
		"1:	ll	%0, %1;"
		"	addiu	%0, 1;"
		"	sc	%0, %1;"
		"	beqz	%0, 1b;"
		"	.set	pop;"
		: "=&r"(tmp), "=m"(*addr)
		: "m"(*addr)
	);
}

/*
 * atomic_decX() functions decreases a value atomically, if it's greater
 * than zero.
 */
static inline void atomic_decq(u64 *addr)
{
	u64 tmp;
	asm volatile (
		"	.set	push;"
		"	.set	mips3;"
		"	.set	reorder;"
		"1:	lld	%0, %1;"
		"	beqz	%0, 2f;"
		"	daddiu	%0, -1;"
		"	scd	%0, %1;"
		"	beqz	%0, 1b;"
		"2:	.set	pop;"
		: "=&r"(tmp), "=m"(*addr)
		: "m"(*addr)
	);
}

static inline void atomic_decd(u32 *addr)
{
	u32 tmp;
	asm volatile (
		"	.set	push;"
		"	.set	mips3;"
		"	.set	reorder;"
		"1:	ll	%0, %1;"
		"	beqz	%0, 2f;"
		"	addiu	%0, -1;"
		"	sc	%0, %1;"
		"	beqz	%0, 1b;"
		"2:	.set	pop;"
		: "=&r"(tmp), "=m"(*addr)
		: "m"(*addr)
	);
}

#endif
