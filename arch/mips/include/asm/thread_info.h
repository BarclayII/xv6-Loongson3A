#ifndef _ASM_THREAD_INFO_H
#define _ASM_THREAD_INFO_H

/*
 * (Hardware) Thread info structure
 */

#define THREAD_SIZE	16384

struct thread_info {
	unsigned int cpu_number;
};

union thread_stack_info {
	struct thread_info current_thread_info;
	unsigned long stack[THREAD_SIZE / sizeof(unsigned long)];
};

#endif
