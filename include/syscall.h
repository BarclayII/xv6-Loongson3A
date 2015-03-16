
#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <asm/syscall.h>

typedef void (*syscall_t)(struct trapframe *);

/* All system calls place return value and error code inside trapframe */

/* int write(int fd, const void *buf, size_t len) */
void sys_write(struct trapframe *tf);

#endif
