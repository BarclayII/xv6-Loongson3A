/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <sched.h>
#include <mm/mmap.h>
#include <mm/vmm.h>
#include <elf.h>
#include <string.h>
#include <errno.h>
#include <config.h>

/*
 * A safe wrapper for converting ELF segment flags into virtual memory area
 * flags (and further to physical page permissions).
 */
static inline unsigned long elf_to_vmaflags(unsigned int elfflags)
{
	unsigned long flags = 0;
	flags |= (elfflags & PF_X) ? VMA_EXEC : 0;
	flags |= (elfflags & PF_W) ? VMA_WRITE : 0;
	flags |= (elfflags & PF_R) ? VMA_READ : 0;
	return flags;
}

/*
 * A program segment corresponds to one virtual memory area.
 */

int task_load_seg_kmem(task_t *task, void *addr, elf64_phdr_t *ph)
{
	int retcode;
	ptr_t dest = (ptr_t)(ph->p_vaddr);
	pdebug("Loading segment into %016x, size %d\r\n", dest, ph->p_memsz);

	/* Create user virtual memory area and associated physical area */
	unsigned long flags = elf_to_vmaflags(ph->p_flags);
	retcode = mm_create_uvm(task->mm, dest, ph->p_memsz, flags);
	if (retcode != 0)
		return retcode;

	retcode = copy_to_uvm(task->mm, dest, addr, ph->p_filesz);
	if (retcode != 0)
		goto rollback_uvm;

	return 0;

rollback_uvm:
	assert(mm_destroy_uvm(task->mm, dest) == 0);
	return retcode;
}

void task_unload_seg(task_t *task, elf64_phdr_t *ph)
{
	mm_destroy_uvm(task->mm, (ptr_t)(ph->p_vaddr));
}

int task_load_segs_kmem(task_t *task, void *addr, elf64hdr_t *hdr)
{
	elf64_phdr_t proghdr;
	size_t off = hdr->e_phoff;
	int ret, i;

	for (i = 0; i < hdr->e_phnum; ++i) {
		memcpy(&proghdr, addr + off, hdr->e_phentsize);
		switch(proghdr.p_type) {
		case PT_LOAD:
			ret = task_load_seg_kmem(task, addr, &proghdr);
			if (ret != 0)
				goto rollback_unload;
			break;
		default:
			break;
		}

		/* Setup program segment top to indicate where the user
		 * stack should start */
		addr_t vaddr = (addr_t)(proghdr.p_vaddr);
		if ((addr_t)(task->progtop) < vaddr + proghdr.p_memsz)
			task->progtop = (ptr_t)(vaddr + proghdr.p_memsz);

		off += hdr->e_phentsize;
	}

	/* For preventing the consequences where the user stack is overrun */
	task->progtop = (ptr_t)(PGROUNDUP(task->progtop) + PGSIZE);

	return 0;

rollback_unload:
	off = hdr->e_phoff;
	for (i = 0; i < hdr->e_phnum; ++i) {
		memcpy(&proghdr, addr + off, hdr->e_phentsize);
		task_unload_seg(task, &proghdr);
		off += hdr->e_phentsize;
	}
	return ret;
}

/*
 * Should not change.
 * This is not really hardware-specific, it's sole purpose is for checking
 * ELF integrity.
 */
#define ELF_CURRENT_MACH	EM_MIPS

/*
 * Load ELF program from kernel space memory into user space.
 * Handling statically linked 64-bit program only in this routine is enough.
 *
 * Stores entry address found in ELF into @entry.
 */
int task_load_elf_kmem(task_t *task, void *addr, addr_t *entry)
{
	struct elf64hdr hdr;

	memcpy(&hdr, addr, sizeof(hdr));

	/* Check identification and integrity */
	printk("\tChecking identification...\r\n");
	if (strncmp((char *)(hdr.e_ident), ELFMAG, SELFMAG) != 0 ||
	    hdr.e_ident[EI_VERSION] != EV_CURRENT ||
	    hdr.e_type != ET_EXEC)
		return -ENOEXEC;

	printk("\tChecking class...\r\n");
	if (hdr.e_ident[EI_CLASS] != ELFCLASS64 ||
	    hdr.e_ident[EI_DATA] != ELFDATA2LSB ||
	    hdr.e_machine != ELF_CURRENT_MACH)
		return -EINVAL;

	printk("\tChecking integrity...\r\n");
	if (hdr.e_phentsize != sizeof(elf64_phdr_t) ||
	    hdr.e_shentsize != sizeof(elf64_shdr_t))
		return -ENOEXEC;

	printk("\tAssigning entry...\r\n");
	*entry = hdr.e_entry;		/* main() or __start */

	printk("\tLoading segments...\r\n");
	return task_load_segs_kmem(task, addr, &hdr);
}
