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
CFLAGS		=  -O -G 0 -mno-abicalls -fno-pic -Wall -mabi=64 -fno-builtin
CFLAGS		+= -nostdinc -nostdlib -g -mips64r2 $(INCFLAG)

LDSCRIPT	= kernel.ld
LDFLAGS		= -N -T$(LDSCRIPT)

KLIBC_OBJS	= klibc/snprintf.o \
		  klibc/strtoul.o \
		  klibc/strcmp.o \
		  klibc/strlen.o \
		  klibc/memset.o \
		  klibc/memcpy.o

LIB_OBJS	= lib/list.o

OBJS		= arch/mips/entry.o \
		  arch/mips/setup.o \
		  arch/mips/cpu.o \
		  arch/mips/traps.o \
		  arch/mips/except.o \
		  arch/mips/mach/loongson3a5/irq.o \
		  drivers/serial/uart16550.o \
		  drivers/serial/prom_printk.o \
		  kern/mm/init.o \
		  kern/printk.o \
		  kern/panic.o \
		  kern/init.o $(KLIBC_OBJS) \
		  $(LIB_OBJS)

OUTPUT		= hello

all: elf

elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $(OUTPUT) $^
	$(OBJDUMP) -S $(OUTPUT) >kernel.s

install: all
	cp $(OUTPUT) $(DEST)

clean:
	rm -rf $(OBJS) $(OUTPUT)

.S.o:
	$(CC) $(CFLAGS) -c $< -o $*.o
