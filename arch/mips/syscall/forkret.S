
#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/stackframe.h>

/* void __noreturn arch_forkret(struct trapframe *tf) */
LEAF(arch_forkret)
	/* TODO: check if the following is really ok */
	move	sp, a0
	RESTORE_ALL_AND_RET
END(arch_forkret)
