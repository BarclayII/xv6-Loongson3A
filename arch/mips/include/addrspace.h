/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1996, 99 Ralf Baechle
 * Copyright (C) 2000, 2002  Maciej W. Rozycki
 * Copyright (C) 1990, 1999 by Silicon Graphics, Inc.
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 */

#ifndef _ASM_ADDRSPACE_H
#define _ASM_ADDRSPACE_H

#include <sys/types.h>

/*
 * Returns the kernel segment base of a given address
 */
#define KSEGX(a)		((a) & 0xe0000000)

/*
 * Returns the physical address of a CKSEGx / XKPHYS address
 */
#define CPHYSADDR(a)		((a) & 0x1fffffff)
#define XPHYSADDR(a)            ((a) & 0x000000ffffffffffLL)

/*
 * Memory segments (64bit kernel mode addresses)
 * The compatibility segments use the full 64-bit sign extended value.  Note
 * the R8000 doesn't have them so don't reference these in generic MIPS code.
 */
#define XKUSEG			0x0000000000000000LL
#define XKSSEG			0x4000000000000000LL
#define XKPHYS			0x8000000000000000LL
#define XKSEG			0xc000000000000000LL
#define CKSEG0			0xffffffff80000000LL
#define CKSEG1			0xffffffffa0000000LL
#define CKSSEG			0xffffffffc0000000LL
#define CKSEG3			0xffffffffe0000000LL

#define CKSEG0ADDR(a)		(CPHYSADDR(a) | CKSEG0)
#define CKSEG1ADDR(a)		(CPHYSADDR(a) | CKSEG1)
#define CKSEG2ADDR(a)		(CPHYSADDR(a) | CKSEG2)
#define CKSEG3ADDR(a)		(CPHYSADDR(a) | CKSEG3)

/*
 * Cache modes for XKPHYS address conversion macros
 */
#define K_CALG_COH_EXCL1_NOL2	0
#define K_CALG_COH_SHRL1_NOL2	1
#define K_CALG_UNCACHED		2
#define K_CALG_NONCOHERENT	3
#define K_CALG_COH_EXCL		4
#define K_CALG_COH_SHAREABLE	5
#define K_CALG_NOTUSED		6
#define K_CALG_UNCACHED_ACCEL	7

/*
 * 64-bit address conversions
 */
#define PHYS_TO_XKSEG_UNCACHED(p)	PHYS_TO_XKPHYS(K_CALG_UNCACHED, (p))
#define PHYS_TO_XKSEG_CACHED(p)		PHYS_TO_XKPHYS(K_CALG_COH_SHAREABLE, (p))
#define XKPHYS_TO_PHYS(p)		((p) & TO_PHYS_MASK)
#define PHYS_TO_XKPHYS(cm, a)		(0x8000000000000000LL | \
					 ((cm) << 59) | (a))

/*
 * The ultimate limited of the 64-bit MIPS architecture:  2 bits for selecting
 * the region, 3 bits for the CCA mode.  This leaves 59 bits of which the
 * R8000 implements most with its 48-bit physical address space.
 */
#define TO_PHYS_MASK	0x07ffffffffffffffLL	/* 2^^59 - 1 */


/*
 * The R8000 doesn't have the 32-bit compat spaces so we don't define them
 * in order to catch bugs in the source code.
 */

#define COMPAT_K1BASE32		0xffffffffa0000000LL
#define PHYS_TO_COMPATK1(x)	((x) | COMPAT_K1BASE32) /* 32-bit compat k1 */


#define KDM_TO_PHYS(x)		(_ACAST64_ (x) & TO_PHYS_MASK)
#define PHYS_TO_K0(x)		(_ACAST64_ (x) | CAC_BASE)

#endif
