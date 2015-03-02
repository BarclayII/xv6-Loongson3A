/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _SYNC_CRITSEC_H
#define _SYNC_CRITSEC_H

#ifndef _SYNC_H
#error "include <sync.h> instead"
#endif

#include <asm/irq.h>

/*
 * @lock is currently reserved for lock objects.
 * Temporarily we only disable IRQ for preventing intra-processor
 * races.  To avoid inter-processor races a lock object residing
 * in memory is needed.
 */
#define ENTER_CRITICAL_SECTION(lock, intr_flag) do { \
	local_irq_save(intr_flag); \
} while (0)

#define EXIT_CRITICAL_SECTION(lock, intr_flag) do { \
	local_irq_restore(intr_flag); \
} while (0)

#endif
