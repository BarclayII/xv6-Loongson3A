/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_ASSERT_H
#define _ASM_ASSERT_H

#ifndef _ASSERT_H
#error "should include <assert.h>"
#endif

/*
 * Invoked by assert(), never call this :)
 */
#define __halt()	asm volatile ("teq $0, $0")

#endif
