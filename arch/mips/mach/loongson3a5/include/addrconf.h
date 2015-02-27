
#ifndef _ASM_MACH_ADDRCONF_H
#define _ASM_MACH_ADDRCONF_H

#define PARAM_BASE	0
#define PARAM_MASK	1
#define PARAM_MMAP	2
#define MIN_PARAM	PARAM_BASE
#define MAX_PARAM	PARAM_MMAP

#define NUM_PRI_DEVICE	8
#define NUM_SEC_DEVICE	2
#define NUM_WIN		8

/* Should be read from read_io() */
#define PRI_ADDRCONF_BASE	0x3ff02000
#define SEC_ADDRCONF_BASE	0x3ff00000

#ifndef __ASSEMBLER__
void dump_addrconf(void);		/* Address window config dumper */
#endif

#endif
