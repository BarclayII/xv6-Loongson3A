/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <stdio.h>

/*
 * The correct behavior would be displaying a hello message,
 * as well as the arguments received, one per line.
 */
int main(int argc, char *argv[])
{
	int i;
	char *hellomsg = "Hello from init!";
	printf("%s\r\n", hellomsg);
	for (i = 0; i < argc; ++i)
		printf("%s\r\n", argv[i]);

	for (;;)
		/* nothing, since exit() is not yet implemented */;
	return 0;
}
