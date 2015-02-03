/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * Loongson 3A HyperTransport control registers and macros
 */

#ifndef _ASM_MACH_HT_REGS_H
#define _ASM_MACH_HT_REGS_H

#define HT_REGS_BASE		0x90000efdfb000000

#define HT_REG_BRCTRL		(HT_REGS_BASE + 0x3c)
#define HT_CAPABILITY(i)	(HT_REGS_BASE + 0x40 + ((i) << 2))
#define HT_CUSTOM		(HT_REGS_BASE + 0x50)
#define HT_RECV_DIAG		(HT_REGS_BASE + 0x54)
#define HT_INT_STRIPE		(HT_REGS_BASE + 0x58)
#define HT_RX_BUFFER		(HT_REGS_BASE + 0x5c)
#define HT_RX_IMAGE_EN_TRANS(i)	(HT_REGS_BASE + 0x60 + ((i) << 3))
#define HT_RX_IMAGE_BASE(i)	(HT_REGS_BASE + 0x64 + ((i) << 3))
#define HT_INTR_CASE(i)		(HT_REGS_BASE + 0x80 + ((i) << 2))
#define HT_INTR_MASK(i)		(HT_REGS_BASE + 0xa0 + ((i) << 2))
#define HT_INTR_CAPABILITY	(HT_REGS_BASE + 0xc0)
#define HT_DATAPORT		(HT_REGS_BASE + 0xc4)
#define HT_INTRINFO_LO		(HT_REGS_BASE + 0xc8)
#define HT_INTRINFO_HI		(HT_REGS_BASE + 0xcc)
#define HT_POST_EN_TRANS(i)	(HT_REGS_BASE + 0xd0 + ((i) << 3))
#define HT_POST_BASE(i)		(HT_REGS_BASE + 0xd4 + ((i) << 3))
#define HT_PREFETCH_EN_TRANS(i)	(HT_REGS_BASE + 0xe0 + ((i) << 3))
#define HT_PREFETCH_BASE(i)	(HT_REGS_BASE + 0xe4 + ((i) << 3))
#define HT_UNCACHE_EN_TRANS(i)	(HT_REGS_BASE + 0xf0 + ((i) << 3))
#define HT_UNCACHE_BASE(i)	(HT_REGS_BASE + 0xf4 + ((i) << 3))

#endif
