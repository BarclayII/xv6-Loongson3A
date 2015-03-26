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
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

/*
 * Primitive implementation where output redirection is not implemented.
 * Will become a wrapper of fprintf() (much) later.
 */
int printf(const char *fmt, ...)
{
	int result;
	va_list ap;
	va_start(ap, fmt);
	result = vprintf(fmt, ap);
	va_end(ap);
	return result;
}

int vprintf(const char *fmt, va_list ap)
{
	int result;
	char buf[BUFSIZ];
	result = vsnprintf(buf, BUFSIZ, fmt, ap);
	return write(STDERR_FILENO, buf, result);
}
