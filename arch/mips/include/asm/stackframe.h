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

#ifndef _ASM_STACKFRAME_H
#define _ASM_STACKFRAME_H

#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/ptrace.h>

#define KSTACK_SIZE	8192

#ifdef __ASSEMBLER__

	/*
	 * Get saved kernel sp.
	 * Should be invoked when switching to kernel mode.
	 */
	.macro	get_saved_sp
	mfc0	k0, CP0_EBASE
	andi	k0, k0, 0x3ff
	dsll	k0, k0, 3
	ld	k1, kernelsp(k0)
	.endm

	/*
	 * Save current kernel sp.
	 * Should be invoked when switching from kernel mode.
	 */
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
	 * by checking the KSU bits.
	 */
	mfc0	k0, CP0_STATUS
	andi	k0, ST_KSU
	beqz	k0, 8f
	move	k1, sp
	/* If it is called from user mode, kernel sp should be retrieved. */
	get_saved_sp
	/* Save original (user or kernel) sp into k0 */
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
	.set	reorder
	mfc0	v1, CP0_STATUS
	sd	v1, TF_STATUS(sp)
	mfc0	v1, CP0_CAUSE
	sd	v1, TF_CAUSE(sp)
	mfc0	v1, CP0_EPC
	sd	v1, TF_EPC(sp)
	dmfc0	v1, CP0_ENTRYHI
	sd	v1, TF_ENTRYHI(sp)
	dmfc0	v1, CP0_BADVADDR
	sd	v1, TF_BADVADDR(sp)
	sd	t0, TF_T0(sp)
	sd	t1, TF_T1(sp)
	sd	t9, TF_T9(sp)		/* t9 = jp */
	sd	gp, TF_GP(sp)
	sd	ra, TF_RA(sp)
#if 0
	ori	gp, sp, THREAD_MASK
	xori	gp, THREAD_MASK		# gp points to start of thread_info union
#endif
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

	.macro	RESTORE_TEMP
	ld	t8, TF_HI(sp)
	ld	t2, TF_T2(sp)
	ld	t3, TF_T3(sp)
	ld	t4, TF_T4(sp)
	mthi	t8
	ld	t8, TF_LO(sp)
	ld	t5, TF_T5(sp)
	ld	t6, TF_T6(sp)
	ld	t7, TF_T7(sp)
	mtlo	t8
	ld	t8, TF_T8(sp)
	.endm

	.macro	RESTORE_STATIC
	ld	s0, TF_S0(sp)
	ld	s1, TF_S1(sp)
	ld	s2, TF_S2(sp)
	ld	s3, TF_S3(sp)
	ld	s4, TF_S4(sp)
	ld	s5, TF_S5(sp)
	ld	s6, TF_S6(sp)
	ld	s7, TF_S7(sp)
	ld	s8, TF_S8(sp)
	.endm

	.macro	RESTORE_AT
	.set	push
	.set	noat
	ld	AT, TF_AT(sp)
	.set	pop
	.endm

	.macro	RESTORE_SOME
	.set	push
	.set	reorder
	.set	noat
	/*
	 * Restoring Status Register is somehow complicated.
	 * We have to ensure the routine runs in kernel mode first, retain
	 * current IMx and (K/S/U)X bit, combining them with saved Status
	 * Register content thereafter.
	 */
	mfc0	a0, CP0_STATUS
	li	v1, 0xff00
	ori	a0, ST_EXCM
	xori	a0, ST_EXCM		# Clear KSU, ERL, EXL and IE
	mtc0	a0, CP0_STATUS
	and	a0, v1			# a0 now contains IMx and KX, SX, UX
	ld	v0, TF_STATUS(sp)
	nor	v1, zero, v1
	and	v0, v1			# v0 contains bits other than those in a0
	or	v0, a0
	mtc0	v0, CP0_STATUS

	ld	v1, TF_EPC(sp)
	mtc0	v1, CP0_EPC
	ld	ra, TF_RA(sp)
	ld	gp, TF_GP(sp)
	ld	v1, TF_ENTRYHI(sp)
	dmtc0	v1, CP0_ENTRYHI
	ld	t9, TF_T9(sp)
	ld	t0, TF_T0(sp)
	ld	t1, TF_T1(sp)
	ld	a3, TF_A3(sp)
	ld	a2, TF_A2(sp)
	ld	a1, TF_A1(sp)
	ld	a0, TF_A0(sp)
	ld	v1, TF_V1(sp)
	ld	v0, TF_V0(sp)
	.set	pop
	.endm

	/*
	 * sp should be restored last since it points to the trapframe.
	 */
	.macro	RESTORE_SP
	ld	sp, TF_SP(sp)
	.endm

	.macro	RESTORE_SP_AND_RET
	ld	sp, TF_SP(sp)
	eret
	.endm

	.macro	RESTORE_ALL
	RESTORE_TEMP
	RESTORE_STATIC
	RESTORE_AT
	RESTORE_SOME
	RESTORE_SP
	.endm

	.macro	RESTORE_ALL_AND_RET
	RESTORE_TEMP
	RESTORE_STATIC
	RESTORE_AT
	RESTORE_SOME
	RESTORE_SP_AND_RET
	.endm

#endif	/* __ASSEMBLER__ */

#endif
