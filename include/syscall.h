/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <asm/syscall.h>

typedef void (*syscall_t)(struct trapframe *);

/* All system calls place return value and error code inside trapframe */

/* int write(int fd, const void *buf, size_t len) */
void sys_write(struct trapframe *tf);

#endif
