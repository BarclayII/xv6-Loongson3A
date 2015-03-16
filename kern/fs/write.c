
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <printk.h>
#include <stddef.h>
#include <errno.h>

#define MAX_WRITE_LEN	BUFSIZ

int do_write(int fd, const void *buf, size_t len, ssize_t *result)
{
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
