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
#include <sched.h>
#include <mm/kmalloc.h>
#include <mm/vmm.h>
#include <fs/sysfile.h>

#define MAX_WRITE_LEN	BUFSIZ

int do_write(int fd, void *buf, size_t len, ssize_t *result)
{
	int errno = 0;
	mm_t *mm = current_task->mm;
	/* Check if the buffer is inside userspace */
	if (!USERSPACE((addr_t)buf)) {
		*result = -1;
		return -EFAULT;
	}

	char *kbuf = kmalloc(MAX_WRITE_LEN);
	switch (fd) {
	case STDOUT_FILENO:
	case STDERR_FILENO:
		/* Here I only implemented writing to serial console.
		 * Writing to file will be implemented after I deal with
		 * filesystems. */
		if (copy_from_uvm(mm, buf, kbuf, MAX_WRITE_LEN) != 0) {
			*result = -1;
			errno = -E2BIG;
			break;
		}
		kbuf[MAX_WRITE_LEN - 1] = '\0';
		*result = printk("%s", kbuf);
		errno = 0;
		break;
	default:
		*result = -1;
		errno = -EBADF;
		break;
	}

	kfree(kbuf);
	return errno;
}
