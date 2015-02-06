/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _CTYPE_H
#define _CTYPE_H

#define isdigit(c)	(((c) >= '0') && ((c) <= '9'))
#define isalpha(c)	((((c) >= 'A') && ((c) <= 'Z')) \
			|| (((c) >= 'a') && ((c) <= 'z')))
#define isupper(c)	(((c) >= 'A') && ((c) <= 'Z'))
#define isspace(c)	(((c) == ' ') || \
			((c) == '\n') || \
			((c) == '\r') || \
			((c) == '\v') || \
			((c) == '\f') || \
			((c) == '\t'))

#endif
