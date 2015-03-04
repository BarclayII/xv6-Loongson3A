/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _STDLIB_H
#define _STDLIB_H

#define	RAND_MAX	2147483647

unsigned long strtoul(const char *nptr, char **endptr, int base);
int rand_r(unsigned int *seed);
int rand(void);
void srand(unsigned int seed);

#endif
