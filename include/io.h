#ifndef _IO_H
#define _IO_H

#define IO_BASE		0x9000000000000000

#define io_remap(phys_off)	(IO_BASE + (phys_off))

#endif
