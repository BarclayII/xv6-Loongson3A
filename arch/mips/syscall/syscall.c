
#include <asm/ptrace.h>
#include <asm/mipsregs.h>
#include <syscall.h>
#include <sys/types.h>
#include <fs/sysfile.h>

syscall_t __syscalls[NR_SYSCALLS] = {
	[NRSYS_write] = sys_write
};

void sys_write(struct trapframe *tf)
{
	int fd = tf->gpr[_A0];
	void *buf = (void *)(tf->gpr[_A1]);
	size_t len = tf->gpr[_A2];
	ssize_t result;
	tf->gpr[_A3] = (unsigned long)do_write(fd, buf, len, &result);
	tf->gpr[_V0] = (unsigned long)result;
}

void handle_sys(struct trapframe *tf)
{
	syscall_t call = __syscalls[tf->gpr[_V0]];
	call(tf);
}
