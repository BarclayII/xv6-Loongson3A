/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/addrspace.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <printk.h>
#include <stddef.h>
#include <errno.h>
#include <fs/sysfile.h>

#define MAX_WRITE_LEN	BUFSIZ

int do_write(int fd, const void *buf, size_t len, ssize_t *result)
{
	/* Check if the buffer is inside userspace */
	if (!USERSPACE((addr_t)buf)) {
		*result = -1;
		return -EFAULT;
	}

	char s[MAX_WRITE_LEN];
	switch (fd) {
	case STDOUT_FILENO:
	case STDERR_FILENO:
		/* Here I only implemented writing to serial console.
		 * Writing to file will be implemented after I deal with
		 * filesystems. */
		strlcpy(s, buf, MAX_WRITE_LEN);
		*result = printk("%s", s);
		return 0;
	default:
		*result = -1;
		return -EBADF;
	}
	/* NOTREACHED */
	return 0;
}
