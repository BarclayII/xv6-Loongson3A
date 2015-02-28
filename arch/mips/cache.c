/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1996 David S. Miller (dm@engr.sgi.com)
 * Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002 Ralf Baechle (ralf@gnu.org)
 * Copyright (C) 1999, 2000 Silicon Graphics, Inc.
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 */

#include <asm/mipsregs.h>
#include <asm/mm/page.h>
#include <asm/cache.h>
#include <asm/cpu.h>
#include <printk.h>
#include <stddef.h>

static unsigned long icache_size, dcache_size, scache_size;
static char *way_string[] = { NULL, "direct mapped", "2-way",
	"3-way", "4-way", "5-way", "6-way", "7-way", "8-way"
};

void probe_pcache(void)
{
	struct cpuinfo_mips *c = &current_cpu_data;
	unsigned int config1;
	unsigned int lsize;

	config1 = read_c0_config1();
	if ((lsize = ((config1 >> 19) & 7)))
		c->icache.linesz = 2 << lsize;
	else
		c->icache.linesz = lsize;
	c->icache.sets = 64 << ((config1 >> 22) & 7);
	c->icache.ways = 1 + ((config1 >> 16) & 7);

	icache_size = c->icache.sets *
		c->icache.ways *
		c->icache.linesz;
	c->icache.waybit =0;

	if ((lsize = ((config1 >> 10) & 7)))
		c->dcache.linesz = 2 << lsize;
	else
		c->dcache.linesz= lsize;
	c->dcache.sets = 64 << ((config1 >> 13) & 7);
	c->dcache.ways = 1 + ((config1 >> 7) & 7);

	dcache_size = c->dcache.sets *
		c->dcache.ways *
		c->dcache.linesz;
	c->dcache.waybit =0;

	/* compute a couple of other cache variables */
	c->icache.waysize = icache_size / c->icache.ways;
	c->dcache.waysize = dcache_size / c->dcache.ways;

	c->icache.sets = c->icache.linesz ?
		icache_size / (c->icache.linesz * c->icache.ways) : 0;
	c->dcache.sets = c->dcache.linesz ?
		dcache_size / (c->dcache.linesz * c->dcache.ways) : 0;

	if (c->dcache.waysize > PGSIZE)
		c->dcache.flags |= MIPS_CACHE_ALIASES;

	printk("Primary icache %dkB, %s, %s, linesize %d bytes.\r\n",
	       icache_size >> 10,
	       c->icache.flags & MIPS_CACHE_VTAG ?
	       "virt indexed, virt tagged" :
	       "virt indexed, phys tagged",
	       way_string[c->icache.ways], c->icache.linesz);

	printk("Primary dcache %dkB, %s, %s, %s, linesize %d bytes\r\n",
	       dcache_size >> 10, way_string[c->dcache.ways],
	       (c->dcache.flags & MIPS_CACHE_PINDEX) ?
	       "phys indexed, phys tagged" :
	       "virt indexed, phys tagged",
	       (c->dcache.flags & MIPS_CACHE_ALIASES) ?
			"cache aliases" : "no aliases",
	       c->dcache.linesz);
}

void setup_scache(void)
{
	struct cpuinfo_mips *c = &current_cpu_data;
	unsigned int config2;
	unsigned int lsize;

	config2=read_c0_config2();
	if ((lsize = ((config2 >> 4) & 15)))
		c->scache.linesz = 2 << lsize;
	else
		c->scache.linesz = lsize;

	c->scache.sets = 64 << ((config2 >> 8) & 15);
	c->scache.ways = 1 + ((config2  ) & 15);

	scache_size = c->scache.sets *
		c->scache.ways *
		c->scache.linesz;

	c->scache.waybit =0; 

	printk("Unified scache %dkB x %d, %s, linesize %d bytes.\r\n",
	    scache_size >> 10, NR_SCACHE, way_string[c->scache.ways],
	    c->scache.linesz, c->scache.waybit);

	if (scache_size) c->options |= MIPS_CPU_INCLUSIVE_CACHES;
}
