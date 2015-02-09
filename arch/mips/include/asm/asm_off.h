/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_ASM_OFF_H
#define _ASM_ASM_OFF_H

/*
 * (Hardware) Thread info structure
 */

#define THREAD_SIZE	8192
#define THREAD_MASK	(THREAD_SIZE - 1)

#define TF_MEMBERS	38
#define TF_SIZE		(TF_MEMBERS * 8)

/* Offsets */
#define TF_ZERO	0x000
#define TF_AT	0x008
#define TF_V0	0x010
#define TF_V1	0x018
#define TF_A0	0x020
#define TF_A1	0x028
#define TF_A2	0x030
#define TF_A3	0x038
#define TF_T0	0x040
#define TF_T1	0x048
#define TF_T2	0x050
#define TF_T3	0x058
#define TF_T4	0x060
#define TF_T5	0x068
#define TF_T6	0x070
#define TF_T7	0x078
#define TF_S0	0x080
#define TF_S1	0x088
#define TF_S2	0x090
#define TF_S3	0x098
#define TF_S4	0x0a0
#define TF_S5	0x0a8
#define TF_S6	0x0b0
#define TF_S7	0x0b8
#define TF_T8	0x0c0
#define TF_T9	0x0c8
#define TF_K0	0x0d0
#define TF_K1	0x0d8
#define TF_GP	0x0e0
#define TF_SP	0x0e8
#define TF_S8	0x0f0
#define TF_RA	0x0f8

#define TF_LO		0x100
#define TF_HI		0x108
#define TF_STATUS	0x110
#define TF_CAUSE	0x118
#define TF_BADVADDR	0x120
#define TF_EPC		0x128

#endif
