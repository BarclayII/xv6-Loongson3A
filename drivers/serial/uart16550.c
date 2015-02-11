/*
 * Copyright (C) 2001 MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 *
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/memrw.h>
#include <asm/io.h>
#include <drivers/uart16550.h>

void
Uart16550Init(unsigned int baud, unsigned char data, unsigned char parity,
    unsigned char stop)
{
	/* disable interrupts */
	UART16550_WRITE(OFS_INTR_ENABLE, 0);

	/* set up buad rate */
	{ 
		uint32 divisor;

		/* set DIAB bit */
		UART16550_WRITE(OFS_LINE_CONTROL, 0x80);

		/* set divisor */
		divisor = MAX_BAUD / baud;
		UART16550_WRITE(OFS_DIVISOR_LSB, divisor & 0xff);
		UART16550_WRITE(OFS_DIVISOR_MSB, (divisor & 0xff00)>>8);

		/* clear DIAB bit */
		UART16550_WRITE(OFS_LINE_CONTROL, 0x0);
	}

	/* set data format */
	UART16550_WRITE(OFS_DATA_FORMAT, data | parity | stop);
}

unsigned char Uart16550GetPoll()
{
	while((UART16550_READ(OFS_LINE_STATUS) & 0x1) == 0);
	return UART16550_READ(OFS_RCV_BUFFER);
}


void Uart16550Put(unsigned char byte)
{
	while ((UART16550_READ(OFS_LINE_STATUS) &0x20) == 0);
	UART16550_WRITE(OFS_SEND_BUFFER, byte);
}

