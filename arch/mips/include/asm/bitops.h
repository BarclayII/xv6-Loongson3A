/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_BITOPS_H
#define _ASM_BITOPS_H

#define BITS_OF_BYTE		8
#define BITS_OF_INT		(BITS_OF_BYTE * sizeof(int))
#define LOG_BYTES_OF_INT	2
#define LOG_BITS_OF_BYTE	3
#define LOG_BITS_OF_INT		(LOG_BITS_OF_BYTE + LOG_BYTES_OF_INT)

#define BITS_TO_INTS(x)		(((x) + BITS_OF_INT - 1) / BITS_OF_INT)

static inline unsigned int
atomic_get_bit(unsigned int pos, volatile unsigned int *addr)
{
	volatile unsigned int *m = addr + (pos >> LOG_BITS_OF_INT);
	unsigned short bit = pos & (BITS_OF_INT - 1);
	return !!((*m) & (1 << bit));
}

/*
 * Atomically set the @pos-th bit from start of @addr.
 */
static inline void
atomic_set_bit(unsigned int pos, volatile unsigned int *addr)
{
	volatile unsigned int *m = addr + (pos >> LOG_BITS_OF_INT);
	unsigned short bit = pos & (BITS_OF_INT - 1);
	unsigned long tmp;

	asm volatile (
		"	.set	mips3;"
		"1:	ll	%0, %1;"
		"	or	%0, %2;"
		"	sc	%0, %1;"
		"	beqz	%0, 1b;"
		"	nop;"
		: "=&r"(tmp), "=m"(*m)
		: "ir"(1 << bit), "m"(*m)
		);
}

/*
 * Atomically clear the @pos-th bit from start of @addr
 */
static inline void
atomic_clear_bit(unsigned int pos, volatile unsigned int *addr)
{
	volatile unsigned int *m = addr + (pos >> LOG_BITS_OF_INT);
	unsigned short bit = pos & (BITS_OF_INT - 1);
	unsigned long tmp;

	asm volatile (
		"	.set	mips3;"
		"1:	ll	%0, %1;"
		"	and	%0, %2;"
		"	sc	%0, %1;"
		"	beqz	%0, 1b;"
		"	nop;"
		: "=&r"(tmp), "=m"(*m)
		: "ir"(~(1 << bit)), "m"(*m)
		);
}

/*
 * Atomically change the @pos-th bit from start of @addr
 */
static inline void
atomic_change_bit(unsigned int pos, volatile unsigned int *addr)
{
	volatile unsigned int *m = addr + (pos >> LOG_BITS_OF_INT);
	unsigned short bit = pos & (BITS_OF_INT - 1);
	unsigned long tmp;

	asm volatile (
		"	.set	mips3;"
		"1:	ll	%0, %1;"
		"	xor	%0, %2;"
		"	sc	%0, %1;"
		"	beqz	%0, 1b;"
		"	nop;"
		: "=&r"(tmp), "=m"(*m)
		: "ir"(1 << bit), "m"(*m)
		);
}

#endif
