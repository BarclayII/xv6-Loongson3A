/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _IO_H
#define _IO_H

#include <sys/types.h>

#define IO_BASE		0x9000000000000000

#define io_remap(phys_off)	(IO_BASE + (phys_off))

#define PCI_OFFSET	0x1fd00000
#define PCI_BASE	io_remap(PCI_OFFSET)
#define pci_remap(phys_off)	(PCI_BASE + (phys_off))

#define inb(port)	(*((volatile uchar *)pci_remap(port)))
#define outb(port, val)	(*((volatile uchar *)pci_remap(port)) = (val))

#define inw(port)	(*((volatile ushort *)pci_remap(port)))
#define outw(port, val)	(*((volatile ushort *)pci_remap(port)) = (val))

#define ind(port)	(*((volatile uint *)pci_remap(port)))
#define outd(port, val)	(*((volatile uint *)pci_remap(port)) = (val))

#define inq(port)	(*((volatile u64 *)pci_remap(port)))
#define outq(port, val)	(*((volatile u64 *)pci_remap(port)) = (val))

#endif
