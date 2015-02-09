/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1994, 95, 96, 99, 2001 Ralf Baechle
 * Copyright (C) 1994, 1995, 1996 Paul M. Antoine.
 * Copyright (C) 1999 Silicon Graphics, Inc.
 * Copyright (C) 2007  Maciej W. Rozycki
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 */

#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm_off.h>

	.macro	get_saved_sp
	mfc0	k0, CP0_EBASE
	andi	k0, k0, 0x3ff
	dsll	k0, k0, 3
	ld	k1, kernelsp(k0)
	.endm

	.macro	set_saved_sp stackp temp
	mfc0	\temp, CP0_EBASE
	andi	\temp, \temp, 0x3ff
	dsll	\temp, \temp, 3
	sd	\stackp, kernelsp(k0)
	.endm

	.macro	SAVE_SOME
	.set	push
	.set	noat
	.set	noreorder
	/*
	 * Since in interrupts these code always run in kernel mode (KSU = 0),
	 * we have to determine whether the routine is called *from* user mode,
	 * by checking whether the CU0 bit is set.
	 */
	mfc0	k0, CP0_STATUS
	sll	k0, 3
	bltz	k0, 8f
	move	k1, sp
	/* If it is called from user mode, kernel sp should be retrieved. */
	get_saved_sp
	/* Save original sp into k0 */
8:	move	k0, sp
	/* Allocate trapframe */
	dsubu	sp, k1, TF_SIZE
	sd	k0, TF_SP(sp)
	sd	zero, TF_ZERO(sp)
	sd	v0, TF_V0(sp)
	sd	v1, TF_V1(sp)
	sd	a0, TF_A0(sp)
	sd	a1, TF_A1(sp)
	sd	a2, TF_A2(sp)
	sd	a3, TF_A3(sp)
	mfc0	v1, CP0_STATUS
	sd	v1, TF_STATUS(sp)
	mfc0	v1, CP0_CAUSE
	sd	v1, TF_CAUSE(sp)
	mfc0	v1, CP0_EPC
	sd	v1, TF_EPC(sp)
	sd	t0, TF_T0(sp)
	sd	t1, TF_T1(sp)
	sd	t9, TF_T9(sp)		/* t9 = jp */
	sd	gp, TF_GP(sp)
	sd	ra, TF_RA(sp)
	.set	pop
	.endm
	
	.macro	SAVE_AT
	.set	push
	.set	noat
	sd	AT, TF_AT(sp)
	.set	pop
	.endm

	.macro	SAVE_TEMP
	/*
	 * Fetching HI and LO registers needs more cycles, so inserting a few
	 * irrelevant instructions improves CPU utilization.
	 */
	mfhi	v1
	sd	t2, TF_T2(sp)
	sd	t3, TF_T3(sp)
	sd	t4, TF_T4(sp)
	sd	v1, TF_HI(sp)
	mflo	v1
	sd	t5, TF_T5(sp)
	sd	t6, TF_T6(sp)
	sd	t7, TF_T7(sp)
	sd	t8, TF_T8(sp)
	sd	v1, TF_LO(sp)
	.endm

	.macro	SAVE_STATIC
	sd	s0, TF_S0(sp)
	sd	s1, TF_S1(sp)
	sd	s2, TF_S2(sp)
	sd	s3, TF_S3(sp)
	sd	s4, TF_S4(sp)
	sd	s5, TF_S5(sp)
	sd	s6, TF_S6(sp)
	sd	s7, TF_S7(sp)
	sd	s8, TF_S8(sp)
	.endm

	.macro	SAVE_ALL
	SAVE_SOME
	SAVE_AT
	SAVE_TEMP
	SAVE_STATIC
	.endm

