#include <stdarg.h>
#include <printk.h>
#include <drivers/prom_printk.h>

int early_printk;

int printk(const char *fmt, ...)
{
	va_list ap;
	int result;
	va_start(ap, fmt);
	if (early_printk) {
		result = prom_vprintk(fmt, ap);
	} else {
		/* Not implemented yet */
		result = 0;
	}
	va_end(ap);
	return result;
}
