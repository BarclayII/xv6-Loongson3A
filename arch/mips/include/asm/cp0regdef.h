/*
 * Copyright (C) 1994, 1995, 1996, 1997, 2000, 2001 by Ralf Baechle
 *
 * Copyright (C) 2001, Monta Vista Software
 * Author: jsun@mvista.com or jsun@junsun.net
 *
 * Copyright (C) 2015, Gan Quan <coin2028@hotmail.com>
 */
#ifndef _ASM_CP0REGDEF_H
#define _ASM_CP0REGDEF_H

#define CP0_INDEX	$0
#define CP0_RANDOM	$1
#define CP0_ENTRYLO0	$2
#define CP0_ENTRYLO1	$3
#define CP0_CONTEXT	$4
#define CP0_PAGEMASK	$5
#define CP0_WIRED	$6
#define CP0_HWRENA	$7
#define CP0_BADVADDR	$8	/* Bad Virtual Address Register */
#define CP0_COUNT	$9	/* Counter */
#define CP0_ENTRYHI	$10
#define CP0_COMPARE	$11	/* Comparer: raises interrupt when = $9 */
#define CP0_STATUS	$12	/* Status Register */
# define CP0_INTCTL	$12, 1	/* Interrupt control */
# define CP0_SRSCTL	$12, 2
# define CP0_SRSMAP	$12, 3
#define CP0_CAUSE	$13	/* Cause Register */
#define CP0_EPC		$14	/* Exception Program Counter */
#define CP0_PRID	$15
# define CP0_EBASE	$15, 1	/* Exception base, and CPU ID for multicore */
#define CP0_CONFIG	$16
# define CP0_CONFIG1	$16, 1
# define CP0_CONFIG2	$16, 2
# define CP0_CONFIG3	$16, 3
#define CP0_LLADDR	$17
#define CP0_WATCHLO	$18
#define CP0_WATCHHI	$19
#define CP0_XCONTEXT	$20
#define CP0_FRAMEMASK	$21
#define CP0_DIAGNOSTIC	$22
#define CP0_PERFCTL	$25
# define CP0_PERFCNT	$25, $1
#define CP0_ECC		$26
#define CP0_CACHEERR	$27
#define CP0_TAGLO	$28
# define CP0_DATALO	$28, $1
#define CP0_TAGHI	$29
# define CP0_DATAHI	$29, $1
#define CP0_ERROREPC	$30

/*
 * Status register (CP0_STATUS) mode bits
 */
#define ST_CU3	0x80000000	/* Coprocessor 3 (MIPS IV User Mode) */
#define ST_CU2	0x40000000	/* Coprocessor 2 */
#define ST_CU1	0x20000000	/* Coprocessor 1 (FPU) */
#define ST_CU0	0x10000000	/* Coprocessor 0 (this one) */
#define ST_RP	0x08000000	/* Reduce power */
#define ST_FR	0x04000000	/* Float register mode switch (?) */
#define ST_RE	0x02000000	/* Reverse-endian */
#define ST_MX	0x01000000	/* Enable DSP or MDMX */
#define ST_PX	0x00800000
/* The exception handler would be at 0xbfc00000 if BEV=1, 0x80000000 
 * otherwise */
#define ST_BEV	0x00400000	/* Bootstrap Exception Vector, usually 0 */
#define ST_TS	0x00200000	/* TLB SHUTDOWN */
#define ST_SR	0x00100000	/* Soft Reset */
#define ST_NMI	0x00080000	/* Non-maskable Interrupt */
/* Interrupt Masks */
#define ST_IM	0x0000ff00	/* All interrupt masks */
#define ST_IMx(i)	(1 << ((i) + 8))
/* eXtended addressing bits for 64-bit addresses */
#define ST_KX	0x00000080	/* Kernel mode eXtended addressing */
#define ST_SX	0x00000040	/* Supervisor mode eXtended addressing */
#define ST_UX	0x00000020	/* User mode eXtended addressing */
/* Kernel/Supervisor/User mode switch */
#define ST_KSU	0x00000018	/* KSU switch */
# define KSU_USER	0x00000010	/* User mode */
# define KSU_SUPERVISOR	0x00000008	/* Supervisor mode */
# define KSU_KERNEL	0x00000000	/* Kernel mode */
#define ST_ERL	0x00000004	/* Error Level */
#define ST_EXL	0x00000002	/* Exception Level */
#define ST_IE	0x00000001	/* Global Interrupt Enable */

/*
 * Cause register (CP0_CAUSE) bits, for handling exceptions
 */

/* Branch Delay would be set if an exception occur in the delay slot, while
 * EPC points to the branching instruction. */
#define CR_BD	0x80000000	/* Branch Delay */
#define CR_TI	0x40000000	/* Timer Interrupt */
#define CR_CE	0x30000000	/* Coprocessor Error */
#define CR_DC	0x08000000	/* Disable Counter */
#define CR_PCI	0x04000000	/* CP0 Performance Counter Overflow (?) */
#define CR_IV	0x00800000	/* Interrupt Vector */
#define CR_WP	0x00400000	/* Watchpoint */
#define CR_IP	0x0000ff00	/* Interrupt Pending */
#define CR_IPx(i)	(1 << ((i + 8)))
#define CR_EC	0x0000007c	/* Exception Code */
#define EXCCODE(x)	(((x) & CR_EC) >> 2)


#endif
