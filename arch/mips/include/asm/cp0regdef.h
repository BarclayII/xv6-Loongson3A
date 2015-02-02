/*
 * Copyright (C) 1994, 1995, 1996, 1997, 2000, 2001 by Ralf Baechle
 * Copyright (C) 2000 Silicon Graphics, Inc.
 * Modified for further R[236]000 support by Paul M. Antoine, 1996.
 * Kevin D. Kissell, kevink@mips.com and Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 2000, 07 MIPS Technologies, Inc.
 * Copyright (C) 2003, 2004  Maciej W. Rozycki
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
# define CP0_PAGEGRAIN	$5, 1
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
# define CP0_CERRADDR	$27, $1
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

/*
 * Exception codes
 */
#define EC_int		0
#define EC_tlbm		1
#define EC_tlbl		2
#define EC_tlbs		3
#define EC_adel		4
#define EC_ades		5
#define EC_ibe		6
#define EC_dbe		7
#define EC_sys		8
#define EC_bp		9
#define EC_ri		10
#define EC_cpu		11
#define EC_ov		12
#define EC_tr		13
#define EC_fpe		15
#define EC_is		16
#define EC_dib		19
#define EC_ddbs		20
#define EC_ddbl		21
#define EC_watch	23
#define EC_dbp		26
#define EC_dint		27
#define EC_dss		28
#define EC_cacheerr	30

/*
 * PageMask register
 */

#define PM_4K		0x00000000
#define PM_8K		0x00002000
#define PM_16K		0x00006000
#define PM_32K		0x0000e000
#define PM_64K		0x0001e000
#define PM_128K		0x0003e000
#define PM_256K		0x0007e000
#define PM_512K		0x000fe000
#define PM_1M		0x001fe000
#define PM_2M		0x003fe000
#define PM_4M		0x007fe000
#define PM_8M		0x00ffe000
#define PM_16M		0x01ffe000
#define PM_32M		0x03ffe000
#define PM_64M		0x07ffe000
#define PM_256M		0x1fffe000
#define PM_1G		0x7fffe000

/*
 * PageGrain register used by Loongson 3A
 */

#define PG_ELPA		0x20000000	/* Enable Large Page Address */

/*
 * Macros to access the system control coprocessor
 */

#define __read_32bit_c0_register(source, sel)				\
({ int __res;								\
	if (sel == 0)							\
		__asm__ __volatile__(					\
			"mfc0\t%0, " #source "\n\t"			\
			: "=r" (__res));				\
	else								\
		__asm__ __volatile__(					\
			".set\tmips32\n\t"				\
			"mfc0\t%0, " #source ", " #sel "\n\t"		\
			".set\tmips0\n\t"				\
			: "=r" (__res));				\
	__res;								\
})

#define __read_64bit_c0_register(source, sel)				\
({ unsigned long long __res;						\
	if (sizeof(unsigned long) == 4)					\
		__res = __read_64bit_c0_split(source, sel);		\
	else if (sel == 0)						\
		__asm__ __volatile__(					\
			".set\tmips3\n\t"				\
			"dmfc0\t%0, " #source "\n\t"			\
			".set\tmips0"					\
			: "=r" (__res));				\
	else								\
		__asm__ __volatile__(					\
			".set\tmips64\n\t"				\
			"dmfc0\t%0, " #source ", " #sel "\n\t"		\
			".set\tmips0"					\
			: "=r" (__res));				\
	__res;								\
})

#define __write_32bit_c0_register(register, sel, value)			\
do {									\
	if (sel == 0)							\
		__asm__ __volatile__(					\
			"mtc0\t%z0, " #register "\n\t"			\
			: : "Jr" ((unsigned int)(value)));		\
	else								\
		__asm__ __volatile__(					\
			".set\tmips32\n\t"				\
			"mtc0\t%z0, " #register ", " #sel "\n\t"	\
			".set\tmips0"					\
			: : "Jr" ((unsigned int)(value)));		\
} while (0)

#define __write_64bit_c0_register(register, sel, value)			\
do {									\
	if (sizeof(unsigned long) == 4)					\
		__write_64bit_c0_split(register, sel, value);		\
	else if (sel == 0)						\
		__asm__ __volatile__(					\
			".set\tmips3\n\t"				\
			"dmtc0\t%z0, " #register "\n\t"			\
			".set\tmips0"					\
			: : "Jr" (value));				\
	else								\
		__asm__ __volatile__(					\
			".set\tmips64\n\t"				\
			"dmtc0\t%z0, " #register ", " #sel "\n\t"	\
			".set\tmips0"					\
			: : "Jr" (value));				\
} while (0)

/*
 * These versions are only needed for systems with more than 38 bits of
 * physical address space running the 32-bit kernel.  That's none atm :-)
 */
#define __read_64bit_c0_split(source, sel)				\
({									\
	unsigned long long __val;					\
	unsigned long __flags;						\
									\
	local_irq_save(__flags);					\
	if (sel == 0)							\
		__asm__ __volatile__(					\
			".set\tmips64\n\t"				\
			"dmfc0\t%M0, " #source "\n\t"			\
			"dsll\t%L0, %M0, 32\n\t"			\
			"dsra\t%M0, %M0, 32\n\t"			\
			"dsra\t%L0, %L0, 32\n\t"			\
			".set\tmips0"					\
			: "=r" (__val));				\
	else								\
		__asm__ __volatile__(					\
			".set\tmips64\n\t"				\
			"dmfc0\t%M0, " #source ", " #sel "\n\t"		\
			"dsll\t%L0, %M0, 32\n\t"			\
			"dsra\t%M0, %M0, 32\n\t"			\
			"dsra\t%L0, %L0, 32\n\t"			\
			".set\tmips0"					\
			: "=r" (__val));				\
	local_irq_restore(__flags);					\
									\
	__val;								\
})

#define __write_64bit_c0_split(source, sel, val)			\
do {									\
	unsigned long __flags;						\
									\
	local_irq_save(__flags);					\
	if (sel == 0)							\
		__asm__ __volatile__(					\
			".set\tmips64\n\t"				\
			"dsll\t%L0, %L0, 32\n\t"			\
			"dsrl\t%L0, %L0, 32\n\t"			\
			"dsll\t%M0, %M0, 32\n\t"			\
			"or\t%L0, %L0, %M0\n\t"				\
			"dmtc0\t%L0, " #source "\n\t"			\
			".set\tmips0"					\
			: : "r" (val));					\
	else								\
		__asm__ __volatile__(					\
			".set\tmips64\n\t"				\
			"dsll\t%L0, %L0, 32\n\t"			\
			"dsrl\t%L0, %L0, 32\n\t"			\
			"dsll\t%M0, %M0, 32\n\t"			\
			"or\t%L0, %L0, %M0\n\t"				\
			"dmtc0\t%L0, " #source ", " #sel "\n\t"		\
			".set\tmips0"					\
			: : "r" (val));					\
	local_irq_restore(__flags);					\
} while (0)

#define __read_ulong_c0_register(reg, sel)				\
	((sizeof(unsigned long) == 4) ?					\
	(unsigned long) __read_32bit_c0_register(reg, sel) :		\
	(unsigned long) __read_64bit_c0_register(reg, sel))

#define __write_ulong_c0_register(reg, sel, val)			\
do {									\
	if (sizeof(unsigned long) == 4)					\
		__write_32bit_c0_register(reg, sel, val);		\
	else								\
		__write_64bit_c0_register(reg, sel, val);		\
} while (0)

#define read_c0_index()		__read_32bit_c0_register($0, 0)
#define write_c0_index(val)	__write_32bit_c0_register($0, 0, val)

#define read_c0_random()	__read_32bit_c0_register($1, 0)
/*#define write_c0_random(val)	__write_32bit_c0_register($1, 0, val)*/

#define read_c0_entrylo0()	__read_ulong_c0_register($2, 0)
#define write_c0_entrylo0(val)	__write_ulong_c0_register($2, 0, val)

#define read_c0_entrylo1()	__read_ulong_c0_register($3, 0)
#define write_c0_entrylo1(val)	__write_ulong_c0_register($3, 0, val)

#define read_c0_context()	__read_ulong_c0_register($4, 0)
#define write_c0_context(val)	__write_ulong_c0_register($4, 0, val)

#define read_c0_pagemask()	__read_32bit_c0_register($5, 0)
#define write_c0_pagemask(val)	__write_32bit_c0_register($5, 0, val)

#define read_c0_pagegrain()	__read_32bit_c0_register($5, 1)
#define write_c0_pagegrain(val)	__write_32bit_c0_register($5, 1, val)

#define read_c0_wired()		__read_32bit_c0_register($6, 0)
#define write_c0_wired(val)	__write_32bit_c0_register($6, 0, val)

#define read_c0_hwrena()	__read_32bit_c0_register($7, 0)
#define write_c0_hwrena(val)	__write_32bit_c0_register($7, 0, val)

#define read_c0_badvaddr()	__read_ulong_c0_register($8, 0)

#define read_c0_count()		__read_32bit_c0_register($9, 0)
#define write_c0_count(val)	__write_32bit_c0_register($9, 0, val)

#define read_c0_entryhi()	__read_ulong_c0_register($10, 0)
#define write_c0_entryhi(val)	__write_ulong_c0_register($10, 0, val)

#define read_c0_compare()	__read_32bit_c0_register($11, 0)
#define write_c0_compare(val)	__write_32bit_c0_register($11, 0, val)

#define read_c0_status()	__read_32bit_c0_register($12, 0)
/*
 * Loongson documentation states that it detects and resolves CP0 hazards,
 * along with load hazards, in hardware, removing necessity of manual NOP
 * corrections.
 */
#define write_c0_status(val)						\
	__write_32bit_c0_register($12, 0, val);				\

#define read_c0_intctl()	__read_32bit_c0_register($12, 1)
#define write_c0_intctl(val)	__write_32bit_c0_register($12, 1, val)

#define read_c0_srsctl()	__read_32bit_c0_register($12, 2)
#define write_c0_srsctl(val)	__write_32bit_c0_register($12, 2, val)

#define read_c0_cause()		__read_32bit_c0_register($13, 0)
#define write_c0_cause(val)	__write_32bit_c0_register($13, 0, val)

#define read_c0_epc()		__read_ulong_c0_register($14, 0)
#define write_c0_epc(val)	__write_ulong_c0_register($14, 0, val)

#define read_c0_prid()		__read_32bit_c0_register($15, 0)

#define read_c0_ebase()		__read_32bit_c0_register($15, 1)
#define write_c0_ebase(val)	__write_32bit_c0_register($15, 1, val)

#define read_c0_config()	__read_32bit_c0_register($16, 0)
#define read_c0_config1()	__read_32bit_c0_register($16, 1)
#define read_c0_config2()	__read_32bit_c0_register($16, 2)
#define read_c0_config3()	__read_32bit_c0_register($16, 3)

#define write_c0_config(val)	__write_32bit_c0_register($16, 0, val)
#define write_c0_config1(val)	__write_32bit_c0_register($16, 1, val)
#define write_c0_config2(val)	__write_32bit_c0_register($16, 2, val)
#define write_c0_config3(val)	__write_32bit_c0_register($16, 3, val)

#define read_c0_lladdr()	__read_ulong_c0_register($17, 0)

#define read_c0_watchlo()	__read_ulong_c0_register($18, 0)
#define write_c0_watchlo(val)	__write_ulong_c0_register($18, 0, val)
#define read_c0_watchhi()	__read_32bit_c0_register($19, 0)
#define write_c0_watchhi(val)	__write_32bit_c0_register($19, 0, val)

#define read_c0_xcontext()	__read_ulong_c0_register($20, 0)
#define write_c0_xcontext(val)	__write_ulong_c0_register($20, 0, val)

#define read_c0_framemask()	__read_32bit_c0_register($21, 0)
#define write_c0_framemask(val)	__write_32bit_c0_register($21, 0, val)

#define read_c0_diag()		__read_32bit_c0_register($22, 0)
#define write_c0_diag(val)	__write_32bit_c0_register($22, 0, val)

#define read_c0_perfctrl0()	__read_32bit_c0_register($25, 0)
#define write_c0_perfctrl0(val)	__write_32bit_c0_register($25, 0, val)
#define read_c0_perfcntr0()	__read_32bit_c0_register($25, 1)
#define write_c0_perfcntr0(val)	__write_32bit_c0_register($25, 1, val)
#define read_c0_perfctrl1()	__read_32bit_c0_register($25, 2)
#define write_c0_perfctrl1(val)	__write_32bit_c0_register($25, 2, val)
#define read_c0_perfcntr1()	__read_32bit_c0_register($25, 3)
#define write_c0_perfcntr1(val)	__write_32bit_c0_register($25, 3, val)

#define read_c0_ecc()		__read_32bit_c0_register($26, 0)
#define write_c0_ecc(val)	__write_32bit_c0_register($26, 0, val)

#define read_c0_cacheerr()	__read_32bit_c0_register($27, 0)

#define read_c0_cerraddr()	__read_ulong_c0_register($27, 1)
#define write_c0_cerraddr(val)	__write_ulong_c0_register($27, 1, val)

#define read_c0_taglo()		__read_32bit_c0_register($28, 0)
#define write_c0_taglo(val)	__write_32bit_c0_register($28, 0, val)

#define read_c0_taghi()		__read_32bit_c0_register($29, 0)
#define write_c0_taghi(val)	__write_32bit_c0_register($29, 0, val)

#define read_c0_datalo()	__read_32bit_c0_register($28, 1)
#define write_c0_datalo(val)	__write_32bit_c0_register($28, 1, val)

#define read_c0_datahi()	__read_32bit_c0_register($29, 1)
#define write_c0_datahi(val)	__write_32bit_c0_register($29, 1, val)

#define read_c0_errorepc()	__read_ulong_c0_register($30, 0)
#define write_c0_errorepc(val)	__write_ulong_c0_register($30, 0, val)

static inline void __ehb(void)
{
	asm volatile (
	    ".set	mips32r2;"
	    "ehb;"
	    ".set	mips0"
	    );
}

#endif
