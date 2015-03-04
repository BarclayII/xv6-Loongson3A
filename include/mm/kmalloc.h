/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _MM_KMALLOC_H
#define _MM_KMALLOC_H

#include <mm/mmap.h>
#include <sys/types.h>

#define KMALLOC_LARGE	(PGSIZE >> 1)
#define ALIGN		8

void *kmalloc(size_t bytes);
void kfree(void *ptr);

#endif
