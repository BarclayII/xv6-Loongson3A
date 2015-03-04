#ifndef _PRINTK_H
#define _PRINTK_H

#include <stdarg.h>
#include <config.h>

extern int early_printk;

int printk(const char *fmt, ...);
int vprintk(const char *fmt, va_list ap);

#ifdef DEBUG
#define pdebug(...)		do { printk(__VA_ARGS__); } while (0)
#define vpdebug(fmt, ap)	do { vprintk(fmt, ap); } while (0)
#else
#define pdebug(...)		do { } while (0)
#define vpdebug(fmt, ap)	do { } while (0)
#endif

#endif
