/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <stdio.h>
#include <drivers/uart16550.h>

void dbg_puts(const char *s)
{
	const char *p;
	for (p = s; *p != '\0'; ++p)
		Uart16550Put((unsigned char)*p);
}

int main(void)
{
	char buf[256];
	snprintf(buf, 256, "%s debug: %c %08x %u %d\r\n", "snprintf", 'a', 256, 128, -4);
	dbg_puts(buf);
	dbg_puts("Hello Loongson 3A!\r\n");
	for (;;)
		/* do nothing */;
}
