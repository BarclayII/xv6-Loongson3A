/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _SCHED_H
#define _SCHED_H

#include <config.h>

#ifdef CONFIG_TASK
#include <sched/task.h>
#else
#error "!CONFIG_TASK not implemented"
#endif

#include <sched/sched.h>

#endif
