
#include <asm/asm.h>
#include <asm/syscall.h>
#include <asm/regdef.h>

#define DEFINE_SYSCALL(name) \
LEAF(name); \
	li	v0, NRSYS_##name; \
	syscall; \
	jr	ra; \
	nop; \
END(name);

DEFINE_SYSCALL(fork)
DEFINE_SYSCALL(execve)
DEFINE_SYSCALL(waitpid)
DEFINE_SYSCALL(kill)
DEFINE_SYSCALL(read)
DEFINE_SYSCALL(write)

