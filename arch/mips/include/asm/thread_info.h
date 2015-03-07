/* thread_info.h: MIPS low-level thread information
 *
 * Copyright (C) 2002  David Howells (dhowells@redhat.com)
 * - Incorporating suggestions made by Linus Torvalds and Dave Miller
 *
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 */

#ifndef _ASM_THREAD_INFO_H
#define _ASM_THREAD_INFO_H

#include <asm/stackframe.h>

/*
 * (Hardware) Thread info structure
 */

#define THREAD_SIZE	KSTACK_SIZE
#define THREAD_MASK	(THREAD_SIZE - 1)

#ifndef __ASSEMBLER__

struct thread_info {
	unsigned int cpu_number;
	unsigned int ticks;		/* for clock interrupt */
	unsigned int compare;		/* for clock interrupt */
};

union thread_stack_info {
	struct thread_info thread_info;
	unsigned long stack[THREAD_SIZE / sizeof(unsigned long)];
};

/*
 * The thread stack should be aligned to thread size.
 */
extern union thread_stack_info init_thread_union;

/*
 * The pointer pointing current thread info is stored at GP.
 */
register struct thread_info *current_thread_info __asm__("$28");

#define init_thread_info	(init_thread_union.thread_info)
#define init_stack		(init_thread_union.stack)

#endif	/* !__ASSEMBLER__ */

#endif
