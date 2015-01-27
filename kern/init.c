/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <drivers/uart16550.h>

void dbg_puts(const char *s)
{
	const char *p;
	for (p = s; *p != '\0'; ++p)
		Uart16550Put((unsigned char)*p);
	Uart16550Put('\r');
	Uart16550Put('\n');
}

int main(void)
{
	dbg_puts("Hello Loongson 3A!");
	for (;;)
		/* do nothing */;
}
