# Copyright (C) 2001 MontaVista Software Inc.
# Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
#
# Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
# 
# This program is free software; you can redistribute  it and/or modify it
# under  the terms of  the GNU General  Public License as published by the
# Free Software Foundation;  either version 2 of the  License, or (at your
# option) any later version.

######## Configuration goes here ########
CROSS_COMPILE = mips64el-linux-

MACH	= loongson3a5

# Install destination, feel free to change this.
DEST	= /srv/tftp/vmlinux-smp-4core

######## End of configuration ###########

CC		= $(CROSS_COMPILE)gcc
CPP		= $(CROSS_COMPILE)cpp
LD		= $(CROSS_COMPILE)ld
OBJDUMP 	= $(CROSS_COMPILE)objdump

INCFLAG		=  -I./arch/mips/include -I./include \
		   -I./arch/mips/mach/$(MACH)/include
CFLAGS		=  -O2 -G 0 -mno-abicalls -fno-pic -Wall -mabi=64 -fno-builtin
CFLAGS		+= -nostdinc -nostdlib -g -mips64r2 $(INCFLAG)

LDSCRIPT	= kernel.ld
LDFLAGS		= -N -T$(LDSCRIPT)

LIBC_OBJS	= lib/libc/stdio/snprintf.o \
		  lib/libc/stdlib/strtoul.o \
		  lib/libc/stdlib/rand.o \
		  lib/libc/string/strlcpy.o \
		  lib/libc/string/strcmp.o \
		  lib/libc/string/strlen.o \
		  lib/libc/string/memset.o \
		  lib/libc/string/memcpy.o

OBJS		= arch/mips/entry.o \
		  arch/mips/setup.o \
		  arch/mips/cpu.o \
		  arch/mips/traps.o \
		  arch/mips/except.o \
		  arch/mips/cache.o \
		  arch/mips/mm/init.o \
		  arch/mips/mm/tlbops.o \
		  arch/mips/mm/pgtable.o \
		  arch/mips/mm/hier/pgtable.o \
		  arch/mips/mm/hier/pgdir.o \
		  arch/mips/mm/hier/pgfault.o \
		  arch/mips/mm/hier/tlbex.o \
		  arch/mips/mach/loongson3a5/irq.o \
		  arch/mips/mach/loongson3a5/addrconf.o \
		  arch/mips/mach/loongson3a5/mm.o \
		  arch/mips/sched/task.o \
		  arch/mips/sched/switch.o \
		  arch/mips/syscall/forkret.o \
		  arch/mips/syscall/syscall.o \
		  drivers/serial/uart16550.o \
		  drivers/serial/prom_printk.o \
		  fs/write.o \
		  kern/mm/init.o \
		  kern/mm/pgalloc.o \
		  kern/mm/test.o \
		  kern/mm/slab.o \
		  kern/mm/kmalloc.o \
		  kern/mm/vm.o \
		  kern/sched/task.o \
		  kern/sched/sched.o \
		  kern/sched/tasklist.o \
		  kern/syscall/fork.o \
		  kern/syscall/execve.o \
		  kern/printk.o \
		  kern/panic.o \
		  kern/init.o $(LIBC_OBJS) \

BINS		= ramdisk/init/init

OUTPUT		= kernel

all: elf

elf: $(OBJS) $(BINS)
	$(LD) $(LDFLAGS) -o $(OUTPUT) $(OBJS) -b binary $(BINS)
	$(OBJDUMP) -S $(OUTPUT) >kernel.s

install: all
	cp $(OUTPUT) $(DEST)

clean:
	rm -rf $(OBJS) $(OUTPUT)
	cd ramdisk/init && make clean

.S.o:
	$(CC) $(CFLAGS) -c $< -o $*.o

ramdisk/init/init:
	cd ramdisk/init && make


