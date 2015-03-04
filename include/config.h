/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _CONFIG_H
#define _CONFIG_H

/* Hierarchical Page Table */
#define CONFIG_HPT

/* Inverted Page Table, NYI */
/* #define CONFIG_IPT */

/* Enable pdebug() and vpdebug() printing */
#define DEBUG

/* Enable 16K pages */
#define CONFIG_16KPAGES

#ifdef CONFIG_16KPAGES
#define CONFIG_3LEVEL_PT
#endif

#endif
