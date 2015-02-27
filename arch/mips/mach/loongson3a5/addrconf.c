/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <addrconf.h>
#include <asm/io.h>
#include <printk.h>

static const char *pri_prefix[] = {
	"CORE0",
	"CORE1",
	"CORE2",
	"CORE3",
	"EAST",
	"SOUTH",
	"WEST",
	"NORTH"
};

static const char *infix[] = {
	"WIN0",
	"WIN1",
	"WIN2",
	"WIN3",
	"WIN4",
	"WIN5",
	"WIN6",
	"WIN7"
};

static const char *suffix[] = {
	"BASE",
	"MASK",
	"MMAP",
	"????"		/* reserved, should not be accessed */
};

static const char *sec_prefix[] = {
	"CPU",
	"PCI"
};

#define PRI_CONFREG(device, win, param) \
	(PRI_ADDRCONF_BASE | ((device) << 8) | ((win) << 3) | ((param << 6)))
#define SEC_CONFREG(device, win, param) \
	(SEC_ADDRCONF_BASE | ((device) << 8) | ((win) << 3) | ((param << 6)))

#define dump_pri_conf(device, win, param) \
	do { \
		printk("%08x %s_%s_%s:\t%016x\r\n", \
		    PRI_CONFREG(device, win, param), \
		    pri_prefix[device], \
		    infix[win], \
		    suffix[param], \
		    read_io_q(PRI_CONFREG(device, win, param))); \
	} while (0)

#define dump_sec_conf(device, win, param) \
	do { \
		printk("%08x %s_%s_%s:\t%016x\r\n", \
		    SEC_CONFREG(device, win, param), \
		    sec_prefix[device], \
		    infix[win], \
		    suffix[param], \
		    read_io_q(SEC_CONFREG(device, win, param))); \
	} while (0)

void dump_addrconf(void)
{
	int device, win, param;
	/* dump primary address config registers */
	for (device = 0; device < NUM_PRI_DEVICE; ++device)
		for (param = MIN_PARAM; param <= MAX_PARAM; ++param)
			for (win = 0; win < NUM_WIN; ++win)
				dump_pri_conf(device, win, param);

	/* dump secondary registers */
	for (device = 0; device < NUM_SEC_DEVICE; ++device)
		for (param = MIN_PARAM; param <= MAX_PARAM; ++param)
			for (win = 0; win < NUM_WIN; ++win)
				dump_sec_conf(device, win, param);
}
