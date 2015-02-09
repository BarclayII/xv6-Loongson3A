#ifndef _ASM_THREAD_INFO_H
#define _ASM_THREAD_INFO_H

#include <asm/asm_off.h>

struct thread_info {
	unsigned int cpu_number;
};

union thread_stack_info {
	struct thread_info current_thread_info;
	unsigned long stack[THREAD_SIZE / sizeof(unsigned long)];
};

/*
 * The thread stack should be aligned to thread size.
 */
extern union thread_stack_info init_thread_union;

#endif
