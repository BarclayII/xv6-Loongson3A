
#ifndef _FS_SYSFILE_H
#define _FS_SYSFILE_H

#include <sys/types.h>

int do_write(int fd, const void *buf, size_t len, ssize_t *result);

#endif
