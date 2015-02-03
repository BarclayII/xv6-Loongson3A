/*
 * Copyright (C) 1995 Linus Torvalds
 * Copyright (C) 1995 Waldorf Electronics
 * Copyright (C) 1994, 95, 96, 97, 98, 99, 2000, 01, 02, 03  Ralf Baechle
 * Copyright (C) 1996 Stoned Elipot
 * Copyright (C) 1999 Silicon Graphics, Inc.
 * Copyright (C) 2000, 2001, 2002, 2007  Maciej W. Rozycki
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/bootinfo.h>

/*
 * fw_arg0 and fw_arg1 serves as argc and argv for kernel.
 *
 * fw_arg2 stores a pointer to boot parameters.
 * It is a pointer to a structure if the system is booted from UEFI, which
 * is the case for Loongson 3A.
 *
 * fw_arg3 is likely reserved(?)
 */
unsigned long fw_arg0, fw_arg1, fw_arg2, fw_arg3;

void setup_arch(void)
{
	/* force compilation */
	return;
}
