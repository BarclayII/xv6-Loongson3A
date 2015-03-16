
#ifndef _SCHED_H
#define _SCHED_H

#include <config.h>

#ifdef CONFIG_TASK
#include <sched/task.h>
#else
#error "!CONFIG_TASK not implemented"
#endif

#endif
