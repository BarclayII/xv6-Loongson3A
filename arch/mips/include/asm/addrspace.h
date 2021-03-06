/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 */

#ifndef _ASM_ADDRSPACE_H
#define _ASM_ADDRSPACE_H

#include <asm/bootinfo.h>

/*
 * Virtual Address Space definitions
 *
 * On MIPS64 architectures, virtual addresses are 64-bit:
 *
 * 6666555555555544444444443333333333222222222211111111110000000000
 * 3210987654321098765432109876543210987654321098765432109876543210
 * ----------------------------------------------------------------
 * 3333333333333333222222222222222211111111111111110000000000000000
 * fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210
 *
 * The 63rd and 62nd bit matches current KSU mode.
 */

/*
 * Meaning of Mapped section:
 * Virtual addresses in Mapped section are first extended by ASID
 * in CP0 EntryHi register.  The VPN part are then extracted and
 * compared with TLB.  If VPN matches an entry in TLB, the global
 * bit and ASID field are checked in order to determine whether
 * a TLB hit or a TLB miss occur.
 */

/*
 * User address space
 * All access to virtual memory address outside XUSEG causes an
 * address error.
 */

/* Mapped */
#define XUSEG_BEGIN	0x0000000000000000
#define XUSEG_END	0x0000ffffffffffff

#define XUSEG		XUSEG_BEGIN

/*
 * Supervisor address space
 * All access to virtual memory address outside XSUSEG and XSSEG
 * causes an address error.
 */

/*
 * User address space is available in supervisor mode.
 */

/* Mapped */
#define XSUSEG_BEGIN	XUSEG_BEGIN
#define XSUSEG_END	XUSEG_END
#define XSSEG_BEGIN	0x4000000000000000
#define XSSEG_END	0x4000ffffffffffff
#define CSSEG_BEGIN	0xffffffffc0000000	/* compat */
#define CSSEG_END	0xffffffffdfffffff	/* compat */

#define XSUSEG		XSUSEG_BEGIN
#define XSSEG		XSSEG_BEGIN
#define CSSEG		CSSEG_BEGIN

/*
 * Kernel address space, more complicated
 */

/*
 * User and supervisor address space are available in kernel mode.
 */

/* Mapped */
#define XKUSEG_BEGIN	XUSEG_BEGIN
#define XKUSEG_END	XUSEG_END
#define XKSSEG_BEGIN	XSSEG_BEGIN
#define XKSSEG_END	XSSEG_END

/*
 * Physical address space are unmapped.
 * Low 48 bits are directly taken as physical address.
 * 61st to 59th bit determine cache coherency.
 */

/* Unmapped */
#define XKPHY_BEGIN	0x8000000000000000
#define XKPHY_END	0xbfffffffffffffff

/* Mapped */
#define XKSEG_BEGIN	0xc000000000000000
#define XKSEG_END	0xc00000ff7fffffff

/*
 * Finally some MIPS32 compatible kernel sections
 */

/*
 * CKSEG0 is mapped to the lowest 256 MB physical memory directly
 * by simply stripping off the higher bits (or by subtracting
 * 0xffffffff80000000).  Memory there are typically cached.
 *
 * Addresses greater than 0xffffffff90000000 correspond to I/O.
 * 
 * It's assumed here that BIOS had already properly initialized
 * caches and TLB.
 */

/* Unmapped, cached */
#define CKSEG0_BEGIN	0xffffffff80000000	/* compat */
#define CKSEG0_END	0xffffffff9fffffff	/* compat */

/*
 * CKSEG1 is also mapped to the lowest 256MB physical memory.
 * The only difference is that accessing this address space
 * does not go through caches.
 *
 * In most cases, this address is used for I/O and BIOS ROM.
 *
 * Loongson 3A user manual suggests that lower 256MB-512MB
 * should be reserved for I/O.  In terms of virtual address,
 * address region 0xffffffffb0000000-0xffffffffbfffffff
 * (or 0x9000000010000000-0x900000001fffffff in extended addressing)
 * should be reserved.
 */

/* Unmapped, uncached */
#define CKSEG1_BEGIN	0xffffffffa0000000	/* compat */
#define CKSEG1_END	0xffffffffbfffffff	/* compat */

/* Mapped */
#define CKSSEG_BEGIN	0xffffffffc0000000	/* compat, supervisor */
#define CKSSEG_END	0xffffffffdfffffff	/* compat, supervisor */
#define CKSEG3_BEGIN	0xffffffffe0000000	/* compat */
#define CKSEG3_END	0xffffffffffffffff	/* compat */

#define XKUSEG		XKUSEG_BEGIN
#define XKSSEG		XKSSEG_BEGIN
#define XKPHY		XKPHY_BEGIN
#define XKSEG		XKSEG_BEGIN
#define CKSEG0		CKSEG0_BEGIN
#define CKSEG1		CKSEG1_BEGIN
#define CKSSEG		CKSSEG_BEGIN
#define CKSEG3		CKSEG3_BEGIN

#define IO_BASE_CACHED	(XKPHY + 0x1800000000000000)
#define IO_BASE		(XKPHY + 0x1000000000000000)
#define KERNBASE	IO_BASE_CACHED

/*
 * Old 32-bit section namings
 */

#define USEG		XUSEG
#define SSEG		CSSEG
#define KSEG0		CKSEG0
#define KSEG1		CKSEG1
#define KSSEG		CKSSEG
#define KSEG3		CKSEG3

#define ASID_KERNEL		0x00	/* Kernel ASID */
#define ASID_MIN		0x01
#define ASID_INVALID		0xff	/* Invalid ASID, needs relocation */
#define ASID_MAX		0xff

#ifndef __ASSEMBLER__

#include <sys/types.h>

static inline bool __userspace(addr_t vaddr)
{
	return vaddr <= XUSEG_END;
}

static inline bool __superspace(addr_t vaddr)
{
	return (vaddr >= XSSEG_BEGIN && vaddr <= XSSEG_END) ||
	    (vaddr >= CSSEG_BEGIN && vaddr <= CSSEG_END);
}

static inline bool __kernspace(addr_t vaddr)
{
	return (vaddr >= XKPHY_BEGIN && vaddr <= XKPHY_END) ||
	    (vaddr >= XKSEG_BEGIN && vaddr <= XKSEG_END) ||
	    (vaddr >= CKSEG0_BEGIN && vaddr <= CKSEG1_END) ||
	    (vaddr >= CKSEG3_BEGIN && vaddr <= CKSEG3_END);
}

#define USERSPACE(vaddr)	__userspace(vaddr)
#define SUPERSPACE(vaddr)	(USERSPACE(vaddr) || __superspace(vaddr))
#define KERNSPACE(vaddr)	(SUPERSPACE(vaddr) || __kernspace(vaddr))

#endif

#endif
