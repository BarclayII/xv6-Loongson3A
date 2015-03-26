
#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/syscall.h>

NESTED(__start, 0, sp)
	.set	push
	.set	reorder
	jal	main
	nop
	/* Call exit() after return */
	move	a0, v0
	li	v0, NRSYS_exit
	syscall
	/* NOTREACHED */
	.set	pop
END(__start)
