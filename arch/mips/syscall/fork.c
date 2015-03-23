
#include <asm/syscalldefs.h>
#include <asm/ptrace.h>

void forkret(struct trapframe *tf)
{
	arch_forkret(tf);
}
