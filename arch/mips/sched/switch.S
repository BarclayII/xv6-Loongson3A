
#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/stackframe.h>
#include <asm/ptrace.h>

/* void switch_context(context_t *old, context_t *new, ptr_t newksp) */
LEAF(switch_context)
	.set	push
	.set	noat
	/* Old sp register is stored at t8 */
	sd	zero,	TF_ZERO(a0)
	sd	AT,	TF_AT(a0)
	sd	v0,	TF_V0(a0)
	sd	v1,	TF_V1(a0)
	sd	a0,	TF_A0(a0)
	sd	a1,	TF_A1(a0)
	sd	a2,	TF_A2(a0)
	sd	a3,	TF_A3(a0)
	sd	t0,	TF_T0(a0)
	sd	t1,	TF_T1(a0)
	sd	t2,	TF_T2(a0)
	sd	t3,	TF_T3(a0)
	sd	t4,	TF_T4(a0)
	sd	t5,	TF_T5(a0)
	sd	t6,	TF_T6(a0)
	sd	t7,	TF_T7(a0)
	sd	t8,	TF_T8(a0)
	sd	t9,	TF_T9(a0)
	sd	s0,	TF_S0(a0)
	sd	s1,	TF_S1(a0)
	sd	s2,	TF_S2(a0)
	sd	s3,	TF_S3(a0)
	sd	s4,	TF_S4(a0)
	sd	s5,	TF_S5(a0)
	sd	s6,	TF_S6(a0)
	sd	s7,	TF_S7(a0)
	sd	s8,	TF_S8(a0)
	sd	k0,	TF_K0(a0)
	sd	k1,	TF_K1(a0)
	sd	gp,	TF_GP(a0)
	sd	sp,	TF_SP(a0)
	sd	ra,	TF_RA(a0)
	mflo	v0
	mfhi	v1
	sd	v0,	TF_LO(a0)
	sd	v1,	TF_HI(a0)
	mfc0	v0,	CP0_STATUS
	sd	v0,	TF_STATUS(a0)
	mfc0	v0,	CP0_CAUSE
	sd	v0,	TF_CAUSE(a0)
	dmfc0	v0,	CP0_BADVADDR
	sd	v0,	TF_BADVADDR(a0)
	dmfc0	v0,	CP0_EPC
	sd	v0,	TF_EPC(a0)
	dmfc0	v0,	CP0_ENTRYHI
	sd	v0,	TF_ENTRYHI(a0)
	.set	at
	set_saved_sp a2, t0
	.set	noat
	ld	v0,	TF_ENTRYHI(a1)
	dmtc0	v0,	CP0_ENTRYHI
	ld	v0,	TF_EPC(a1)
	dmtc0	v0,	CP0_EPC
	ld	v0,	TF_BADVADDR(a1)
	dmtc0	v0,	CP0_BADVADDR
	ld	v0,	TF_CAUSE(a1)
	mtc0	v0,	CP0_CAUSE
	ld	v0,	TF_STATUS(a1)
	mtc0	v0,	CP0_STATUS
	ld	v0,	TF_LO(a1)
	ld	v1,	TF_HI(a1)
	mtlo	v0
	mthi	v1
	ld	ra,	TF_RA(a1)
	ld	s8,	TF_S8(a1)
	ld	sp,	TF_SP(a1)
	ld	gp,	TF_GP(a1)
	ld	k0,	TF_K0(a1)
	ld	k1,	TF_K1(a1)
	ld	s0,	TF_S0(a1)
	ld	s1,	TF_S1(a1)
	ld	s2,	TF_S2(a1)
	ld	s3,	TF_S3(a1)
	ld	s4,	TF_S4(a1)
	ld	s5,	TF_S5(a1)
	ld	s6,	TF_S6(a1)
	ld	s7,	TF_S7(a1)
	ld	t0,	TF_T0(a1)
	ld	t1,	TF_T1(a1)
	ld	t2,	TF_T2(a1)
	ld	t3,	TF_T3(a1)
	ld	t4,	TF_T4(a1)
	ld	t5,	TF_T5(a1)
	ld	t6,	TF_T6(a1)
	ld	t7,	TF_T7(a1)
	ld	t8,	TF_T8(a1)
	ld	t9,	TF_T9(a1)
	ld	AT,	TF_AT(a1)
	ld	v0,	TF_V0(a1)
	ld	v1,	TF_V1(a1)
	ld	a0,	TF_A0(a1)
	ld	a2,	TF_A2(a1)
	ld	a3,	TF_A3(a1)
	ld	a1,	TF_A1(a1)
	jr	ra
	nop
	.set	pop
END(switch_context)

