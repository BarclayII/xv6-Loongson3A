
#ifndef _ASM_SYSCALLDEFS_H
#define _ASM_SYSCALLDEFS_H

#include <asm/ptrace.h>

void arch_forkret(struct trapframe *tf);
void forkret(struct trapframe *tf);

#endif
