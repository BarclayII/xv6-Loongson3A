#ifndef _PRINTK_H
#define _PRINTK_H

#include <stdarg.h>

extern int early_printk;

int printk(const char *fmt, ...);
int vprintk(const char *fmt, va_list ap);

#endif
