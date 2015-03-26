/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <unistd.h>
#include <string.h>

/*
 * The correct behavior would be displaying a hello message,
 * as well as the arguments received, one per line.
 */
int main(int argc, char *argv[])
{
	int i;
	char *hellomsg = "Hello from init!\r\n";
	char *crlf = "\r\n";
	write(STDERR_FILENO, hellomsg, strlen(hellomsg));
	for (i = 0; i < argc; ++i) {
		write(STDERR_FILENO, argv[i], strlen(argv[i]));
		write(STDERR_FILENO, crlf, strlen(crlf));
	}
	for (;;)
		/* nothing, since exit() is not yet implemented */;
	return 0;
}
