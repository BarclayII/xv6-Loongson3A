/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_TRAP_H
#define _ASM_TRAP_H

#include <asm/ptrace.h>

void trap_init(void);
void handle_exception(struct trapframe *tf);

void dump_trapframe(struct trapframe *tf);

int handle_sys(struct trapframe *tf);

#endif
