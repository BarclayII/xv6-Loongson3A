/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1994, 1995 by Ralf Baechle
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 */

#ifndef __ASM_MIPS_REGDEF_H
#define __ASM_MIPS_REGDEF_H

/*
 * Symbolic register names for 32 bit ABI
 * 
 * Although I'm programming on a 64-bit machine, I retain the 32 bit register
 * names.
 */
#define zero	$0	/* wired zero */
#define AT	$1	/* assembler temp  - uppercase because of ".set at" */
#define v0	$2	/* return value */
#define v1	$3
#define a0	$4	/* argument registers */
#define a1	$5
#define a2	$6
#define a3	$7
#define t0	$8
#define t1	$9
#define t2	$10
#define t3	$11
#define t4	$12	/* caller saved */
#define t5	$13
#define t6	$14
#define t7	$15
#define s0	$16	/* callee saved */
#define s1	$17
#define s2	$18
#define s3	$19
#define s4	$20
#define s5	$21
#define s6	$22
#define s7	$23
#define t8	$24
#define t9	$25
#define jp	$26
#define k0	$26
#define k1	$27
#define gp	$28
#define sp	$29
#define fp	$30
#define s8	$30
#define ra	$31

#define _ZERO	0
#define _AT	1
#define _V0	2
#define _V1	3
#define _A0	4
#define _A1	5
#define _A2	6
#define _A3	7
#define _T0	8
#define _T1	9
#define _T2	10
#define _T3	11
#define _T4	12
#define _T5	13
#define _T6	14
#define _T7	15
#define _S0	16
#define _S1	17
#define _S2	18
#define _S3	19
#define _S4	20
#define _S5	21
#define _S6	22
#define _S7	23
#define _T8	24
#define _T9	25
#define _JP	25
#define _K0	26
#define _K1	27
#define _GP	28
#define _SP	29
#define _FP	30
#define _S8	30
#define _RA	31

#endif /* __ASM_MIPS_REGDEF_H */
