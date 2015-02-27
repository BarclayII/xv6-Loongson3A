#ifndef _PRINTK_H
#define _PRINTK_H

#include <stdarg.h>
#include <config.h>

extern int early_printk;

int printk(const char *fmt, ...);
int vprintk(const char *fmt, va_list ap);

#ifdef DEBUG
#define pdebug(fmt, ...)	printk(fmt, ...)
#define vpdebug(fmt, ap)	vprintk(fmt, ap)
#else
#define pdebug(fmt, ...)
#define vpdebug(fmt, ap)
#endif

#endif
