# Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
# 
# This program is free software; you can redistribute  it and/or modify it
# under  the terms of  the GNU General  Public License as published by the
# Free Software Foundation;  either version 2 of the  License, or (at your
# option) any later version.

######## Configuration goes here ########
CROSS_COMPILE	= mips64el-linux-

MACH		= loongson3a5

ROOT		= ../..
######## End of configuration ###########

OUTPUT_FORMAT	= elf64-tradlittlemips

CC		= $(CROSS_COMPILE)gcc
LD		= $(CROSS_COMPILE)ld
OBJDUMP		= $(CROSS_COMPILE)objdump
OBJCOPY		= $(CROSS_COMPILE)objcopy

LIBCDIR		= $(ROOT)/lib/libc

INCFLAG		= -I$(ROOT)/include -I$(ROOT)/arch/mips/include \
		  -I$(ROOT)/arch/mips/mach/$(MACH)/include
CFLAGS		= -O2 -G 0 -mno-abicalls -fno-pic -Wall -mabi=64 -fno-builtin \
		  -nostdinc -nostdlib -g -mips64r2 $(INCFLAG)
LDFLAGS		= --oformat=$(OUTPUT_FORMAT)

EXEC		= init

LIBC_OBJS	= $(LIBCDIR)/arch/mips/syscall.o \
		  $(LIBCDIR)/arch/mips/crt0.o \
		  $(LIBCDIR)/string/strlen.o

OBJS		= init.o

all: $(EXEC)

$(EXEC): $(OBJS) $(LIBC_OBJS)
	$(LD) $(LDFLAGS) -o $(EXEC) $^
	$(OBJDUMP) -S $(EXEC) >init.s

clean:
	rm -rf $(EXEC) $(OBJS)

.S.o:
	$(CC) $(CFLAGS) -c $< -o $*.o
