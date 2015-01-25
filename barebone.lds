# Copyright (C) 2001 MontaVista Software Inc.
# Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
#
# Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
# 
# This program is free software; you can redistribute  it and/or modify it
# under  the terms of  the GNU General  Public License as published by the
# Free Software Foundation;  either version 2 of the  License, or (at your
# option) any later version.

OUTPUT_FORMAT("elf64-tradlittlemips")
OUTPUT_ARCH(mips)
ENTRY(_start)

SECTIONS
{
  .text : {
        *(.text)
	*(.rodata)
        }

  .reginfo : { *(.reginfo) }

  .data : {
	*(.data)
	}

  .bss  : {
	*(.dynbss)
	*(.bss)
  }
}
