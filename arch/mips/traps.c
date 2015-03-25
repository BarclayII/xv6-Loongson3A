/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <asm/trap.h>
#include <asm/cpu.h>
#include <asm/decode.h>
#include <asm/mm/tlb.h>
#include <asm/mm/pgtable.h>
#include <asm/thread_info.h>
#include <drivers/uart16550.h>
#include <irqregs.h>
#include <stddef.h>
#include <string.h>
#include <printk.h>
#include <panic.h>
#include <assert.h>

unsigned long except_handlers[32];
extern unsigned long except_generic, except_tlb;

void empty_routine(void)
{
	printk("executing empty_routine()\r\n");
}

static void test_skip(void)
{
	int a = 2, b = 2, c = -2, d;
	printk("**********test_skip**********\r\n");
	printk("Direct breakpoint\r\n");
	asm volatile (
		"break	0"
	);
	printk("Jump\r\n");
	asm volatile (
		"	.set	push;"
		"	.set	noreorder;"
		"	j	1f;"
		"	break	0;"
		"1:	.set	pop"
	);
	printk("Jump Register\r\n");
	asm volatile (
		"	.set	push;"
		"	.set	noreorder;"
		"	dla	$16, 1f;"
		"	jr	$16;"
		"	break	0;"
		"1:	.set	pop;"
		: /* no output */ : /* no input */ : "$16"
	);
	/* should print "empty_routine" once */
	printk("Jump And Link\r\n");
	asm volatile (
		".set	push;"
		".set	noreorder;"
		"jal	empty_routine;"
		"break	0;"
		".set	pop;"
	);
	/* should print "empty_routine" once */
	printk("Jump And Link Register\r\n");
	asm volatile (
		".set	push;"
		".set	noreorder;"
		"dla	$16, empty_routine;"
		"jalr	$16;"
		"break	0;"
		".set	pop;"
		: /* no output */ : /* no input */ : "$16"
	);
	printk("Branch if Equal (Success)\r\n");
	asm volatile (
		"	.set	push;"
		"	.set	noreorder;"
		"	li	%0, 1;"
		"	beq	%1, %2, 1f;"
		"	break	0;"
		"	move	%0, $0;"
		"1:	.set	pop"
		: "=r"(d) : "r"(a), "r"(b)
	);
	assert(d);
	printk("Branch if Equal (Fail)\r\n");
	asm volatile (
		"	.set	push;"
		"	.set	noreorder;"
		"	move	%0, $0;"
		"	beq	%1, %2, 1f;"
		"	break	0;"
		"	li	%0, 1;"
		"1:	.set	pop"
		: "=r"(d) : "r"(a), "r"(c)
	);
	assert(d);
	/* should print "empty_routine" once */
	printk("Branch if >= 0 And Link (Success)\r\n");
	asm volatile (
		".set	push;"
		".set	noreorder;"
		"bgezal	%0, empty_routine;"
		"break	0;"
		".set	pop"
		: /* no output */ : "r"(a)
	);
	/* should print "empty_routine" once */
	printk("Branch if >= 0 And Link (Fail)\r\n");
	asm volatile (
		".set	push;"
		".set	noreorder;"
		"bgezal	%0, empty_routine;"
		"break	0;"
		".set	pop"
		: /* no output */ : "r"(c)
	);
	/* should print "empty_routine" twice */
	printk("Branch back if not equal (Success and fail)\r\n");
	asm volatile (
		"	.set	push;"
		"	.set	noreorder;"
		"	b	2f;"
		"	move	$16, $0;"
		"1:	li	$16, 1;"
		"2:	beqz	$16, 1b;"
		"	break	0;"
		"	.set	pop"
		: /* no output */ : /* no input */ : "$16"
	);
	printk("Whoa!\r\n");
}

void trap_init(void)
{
	unsigned long ebase = read_c0_ebase();
	ebase = (0xffffffff00000000) | ((ebase | 0x3ff) ^ 0x3ff);
	memcpy((void *)(ebase + 0x180), &except_generic, 0x80);

	/*
	 * Temporarily we use generic routines for dealing TLB exceptions
	 *
	 * Looks like TLB refilling happens in EBASE + 0x00
	 */
	memcpy((void *)(ebase), &except_tlb, 0x80);
	memcpy((void *)(ebase + 0x80), &except_tlb, 0x80);
	/*
	 * And also cache errors
	 */
	memcpy((void *)(ebase + 0x100), &except_generic, 0x80);

	/* force linkage */
	empty_routine();

	test_skip();
}

static const char *regname[] = {
	"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	"t8", "t9", "k0", "k1", "gp", "sp", "s8", "ra"
};

static const char *ex_desc[] = {
	[EC_int]	= "Interrupt Pending",
	[EC_tlbm]	= "TLB Modified",
	[EC_tlbl]	= "TLB Loading Miss",
	[EC_tlbs]	= "TLB Storing Miss",
	[EC_adel]	= "Address Error on Loading",
	[EC_ades]	= "Address Error on Storing",
	[EC_ibe]	= "Bus Error while Fetching Instruction",
	[EC_dbe]	= "Bus Error while Loading/Storing Data",
	[EC_sys]	= "System Call",
	[EC_bp]		= "Breakpoint",
	[EC_ri]		= "Reserved/Illegal Instruction",
	[EC_cpu]	= "Coprocessor Unusable",
	[EC_ov]		= "Arithmetic Overflow",
	[EC_tr]		= "Trap Instruction",
	[EC_fpe]	= "Floating-Point Exception",
	[EC_is]		= "Stack Exception",
	[EC_dib]	= "Debug Instruction Exception",
	[EC_ddbs]	= "Debug Data Storing Exception",
	[EC_ddbl]	= "Debug Data Loading Exception",
	[EC_watch]	= "Referencing WatchHi/WatchLo",
	[EC_dbp]	= "Debug Breakpoint",
	[EC_dint]	= "Debug Interrupt",
	[EC_dss]	= "Debug Step",
	[EC_cacheerr]	= "Cache Error"
};

void dump_trapframe(struct trapframe *tf)
{
	int i;
	unsigned long tmp_enthi;
	for (i = _ZERO; i <= _RA; ++i) {
		printk("%s\t%016x\r\n", regname[i], tf->gpr[i]);
	}
	printk("STATUS\t= %08x\r\n", tf->cp0_status);
	printk("CAUSE\t= %08x\r\n", tf->cp0_cause);
	printk("EPC\t= %016x\r\n", tf->cp0_epc);
	printk("BADVADDR= %016x\r\n", tf->cp0_badvaddr);
	printk("ENTRYHI\t=%016x\r\n", tf->cp0_entryhi);
	switch (EXCCODE(tf->cp0_cause)) {
	case EC_tlbm:
	case EC_tlbl:
	case EC_tlbs:
		/* Read current ENTRYLO0 and ENTRYLO1 contents */
		/* Redundant? */
		tmp_enthi = read_c0_entryhi();
		write_c0_entryhi(tf->cp0_entryhi);
		tlbp();
		if (read_c0_index() >= 0) {
			tlbr();
			printk("ENTRYLO0=%016x\r\n", read_c0_entrylo0());
			printk("ENTRYLO1=%016x\r\n", read_c0_entrylo1());
		}
		write_c0_entryhi(tmp_enthi);
		break;
	case EC_ri:
		printk("Ins\t= %08x\r\n", *(unsigned int *)(tf->cp0_epc));
		break;
	default:
		break;
	}
}

static int handle_int(struct trapframe *tf)
{
	unsigned int cause = tf->cp0_cause;
	if ((cause & CR_IPx(7)) && (cause & CR_TI)) {
		unsigned int cmp = read_c0_compare();
		/* Repeatedly write into CP0_COMPARE until it is ahead, but
		 * not too far from CP0_COUNT */
		do {
			cmp += 0x10000000;
			write_c0_compare(cmp);
		} while (cmp - read_c0_count() > 0x10000000);
		printk("CPUID %d\tticks\t= %d\r\n",
		    current_thread_info->cpu_number,
		    current_thread_info->ticks);
		++(current_thread_info->ticks);
		return 0;
	} else if (cause & CR_IPx(2)) {
		/* LPC Interrupts */
		printk("Received LPC Interrupt\r\n");
		printk("INTISR\t = %08x\r\n", IO_control_regs_Intisr);
		printk("INTEN\t = %08x\r\n", IO_control_regs_Inten);
		printk("INTENSET = %08x\r\n", IO_control_regs_Intenset);
		printk("INTENCLR = %08x\r\n", IO_control_regs_Intenclr);
		printk("INTEDGE\t = %08x\r\n", IO_control_regs_Intedge);
		printk("CORE0\t = %08x\r\n", IO_control_regs_CORE0_INTISR);
		printk("CORE1\t = %08x\r\n", IO_control_regs_CORE1_INTISR);
		printk("CORE2\t = %08x\r\n", IO_control_regs_CORE2_INTISR);
		printk("CORE3\t = %08x\r\n", IO_control_regs_CORE3_INTISR);
		printk("LPCCTRL\t = %08x\r\n", LPC_INT_regs_ctrl);
		printk("LPCEN\t = %08x\r\n", LPC_INT_regs_enable);
		printk("LPCSTAT\t = %08x\r\n", LPC_INT_regs_status);
		printk("LPCCLR\t = %08x\r\n", LPC_INT_regs_clear);
		printk("INTRID\t = %02x\r\n", UART16550_READ(OFS_INTR_ID));
	}
	return -1;
}

/*
 * Determine the branch target according to given program counter.
 *
 * @pc locates the branching or jumping instruction.
 */
static unsigned long branch_target(struct trapframe *tf, unsigned long pc)
{
	unsigned int insn = read_insn(pc);
	unsigned int opcode = OPCODE(insn);
	unsigned int func, rs, rt, rd;
	unsigned long offset, branch_pc, cont_pc;

	cont_pc = pc + 8;

	/* Handle jumps */
	switch (opcode) {
	case OP_SPECIAL:
		rs = R_RS(insn);
		rd = R_RD(insn);
		func = R_FUNC(insn);
		switch (func) {
		case FN_JALR:
			/* JALR changes destination register (RA in most cases)
			 * to return address */
			tf->gpr[rd] = cont_pc;
			/* fallthru */
		case FN_JR:
			/* JR and JALR sets target to where the source register
			 * is pointing */
			return tf->gpr[rs];
		default:
			goto fail;
		}
		break;
	case OP_JAL:
		tf->gpr[_RA] = cont_pc;
		/* fallthru */
	case OP_J:
		return (pc & ~J_ACTUAL_INDEX_MASK) | J_INDEX(insn);
	}

	/* Handle branches */
	rs = I_RS(insn);
	offset = I_IOFFSET(insn);
	/* Offset are added to the address of delay slot, not the branch itself,
	 * as described in MIPS64 manual */
	branch_pc = pc + offset + 4;

	switch (opcode) {
	case OP_REGIMM:
		func = I_FUNC(insn);
		switch (func) {
		case FN_BLTZAL:
		case FN_BLTZALL:
			if (tf->gpr[rs] < 0)
				/* question: will RA be overwritten even if a
				 * branch doesn't happen? */
				tf->gpr[_RA] = cont_pc;
			/* fallthru */
		case FN_BLTZ:
		case FN_BLTZL:
			if (tf->gpr[rs] < 0)
				return branch_pc;
			else
				return cont_pc;
			break;
		case FN_BGEZAL:
		case FN_BGEZALL:
			if (tf->gpr[rs] >= 0)
				tf->gpr[_RA] = cont_pc;
			/* fallthru */
		case FN_BGEZ:
		case FN_BGEZL:
			if (tf->gpr[rs] >= 0)
				return branch_pc;
			else
				return cont_pc;
			break;
		default:
			goto fail;
		}
		break;
	case OP_BLEZ:
	case OP_BLEZL:
		if (tf->gpr[rs] <= 0)
			return branch_pc;
		else
			return cont_pc;
		break;
	case OP_BGTZ:
	case OP_BGTZL:
		if (tf->gpr[rs] > 0)
			return branch_pc;
		else
			return cont_pc;
		break;
	}

	/* Handle beq and bne */
	rt = I_RT(insn);
	switch (opcode) {
	case OP_BEQ:
	case OP_BEQL:
		if (tf->gpr[rs] == tf->gpr[rt])
			return branch_pc;
		else
			return cont_pc;
		break;
	case OP_BNE:
	case OP_BNEL:
		if (tf->gpr[rs] != tf->gpr[rt])
			return branch_pc;
		else
			return cont_pc;
		break;
	}

fail:
	printk("instruction address = %016x\r\n", pc);
	printk("instruction content = %08x\r\n", insn);
	panic("branch_target(): not a branch instruction\r\n");
	/* NOTREACHED */
	return 0;
}

/* 
 * Skips the current exception victim instruction and move on if necessary,
 * i.e. after system call or some of breakpoints.
 *
 * This routine should be called after everything is done.
 */
static void skip_victim(struct trapframe *tf)
{
	/* Check if victim instruction is inside a branch delay slot */
	if (tf->cp0_cause & CR_BD)
		tf->cp0_epc = branch_target(tf, tf->cp0_epc);
	else
		tf->cp0_epc += 4;
}

static int handle_bp(struct trapframe *tf)
{
	unsigned long victim = (tf->cp0_cause & CR_BD) ?
	    tf->cp0_epc + 4: tf->cp0_epc;
	unsigned int code = R_CODE(read_insn(victim));
	if (code == 0) {
		printk("Caught breakpoint with code 0, resuming execution\r\n");
		skip_victim(tf);
		return 0;
	} else {
		printk("Caught breakpoint with code %d\r\n", code);
	}
	return -1;
}

void handle_exception(struct trapframe *tf)
{
	int exccode = EXCCODE(tf->cp0_cause);
	/* Someday I'll merge this switch block into function array */
	switch (exccode) {
	case EC_int:
		if (handle_int(tf) == 0)
			return;
		break;
	case EC_bp:
		if (handle_bp(tf) == 0)
			return;
		break;
	case EC_sys:
		/* System call */
		handle_sys(tf);
		return;
	case EC_tlbm:
		/* Writing on read-only page */
		break;
	case EC_tlbl:
	case EC_tlbs:
		/* Loading/storing misses (page faults) */
		if (handle_pgfault(tf) == 0)
			return;
		break;
	}

	printk("Caught exception %s (%d)\r\n",
	    (ex_desc[exccode] == NULL) ? "(unknown)" : ex_desc[exccode],
	    exccode);
	dump_trapframe(tf);
	panic("SUSPENDING SYSTEM...\r\n");
}

