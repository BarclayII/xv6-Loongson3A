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
LOADADDR = 0xffffffff80300000

CROSS_COMPILE = mips64el-linux-

######## End of configuration ###########

CC	= $(CROSS_COMPILE)gcc
CPP	= $(CROSS_COMPILE)cpp
LD	= $(CROSS_COMPILE)ld

INCFLAG	= -Iarch/mips/include
CFLAGS	= -O -G 0 -mno-abicalls -fno-pic -Wall -mabi=64 $(INCFLAG)

LDSCRIPT= barebone.lds
LDFLAGS	= -N -T$(LDSCRIPT) -Ttext $(LOADADDR)

OUTPUT	= stupid

all: elf

elf: start.o
	$(LD) $(LDFLAGS) -o $(OUTPUT) $^

clean:
	rm -rf *.o $(OUTPUT)

.S.o:
	$(CC) $(CFLAGS) -c $< -o $*.o


