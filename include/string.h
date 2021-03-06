/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _STRING_H
#define _STRING_H

#include <sys/types.h>

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
size_t strlcat(char *dst, const char *src, size_t dsize);
size_t strlcpy(char *dst, const char *src, size_t dsize);
size_t strlen(const char *str);
void *memset(void *dst, int c, size_t n);
void *memcpy(void *dst0, const void *src0, size_t length);

#endif
