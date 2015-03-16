
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

	/* Create user virtual memory area and associated physical area */
	unsigned long flags = elf_to_vmaflags(ph->p_flags);
	retcode = mm_create_uvm(task->mm, ph->p_vaddr, ph->p_memsz, flags);
	if (retcode != 0)
		return retcode;

	retcode = copy_to_uvm(task->mm, ph->p_vaddr, addr, ph->p_filesz);
	if (retcode != 0)
		return retcode;

	return 0;

rollback_uvm:
	assert(mm_destroy_uvm(task->mm, ph->p_vaddr) == 0);
	return retcode;
}

void task_unload_seg(task_t *task, elf64_phdr_t *ph)
{
	mm_destroy_uvm(task->mm, ph->p_vaddr);
}

int task_load_segs_kmem(task_t *task, void *addr, elf64hdr_t *hdr)
{
	struct elf64_phdr_t proghdr;
	size_t off = hdr->e_phoff;
	int ret;

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
		if (task->progtop < proghdr.p_vaddr + proghdr.p_memsz)
			task->progtop = proghdr.p_vaddr + prohhdr.p_memsz;

		off += hdr->e_phentsize;
	}

	task->progtop = PGROUNDUP(task->progtop);

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
	if (strncmp(hdr.e_ident, ELFMAG, SELFMAG) != 0 ||
	    hdr.e_ident[EI_VERSION] != EV_CURRENT ||
	    hdr.e_type != ET_EXEC)
		return -ENOEXEC;

	if (hdr.e_ident[EI_CLASS] != ELFCLASS64 ||
	    hdr.e_ident[EI_DATA] != ELFDATA2LSB ||
	    hdr.e_machine != ELF_CURRENT_MACH)
		return -EINVAL;

	if (hdr.e_phentsize != sizeof(elf64_phdr_t) ||
	    hdr.e_shentsize != sizeof(elf64_shdr_t))
		return -ENOEXEC;

	*entry = hdr.e_entry;		/* main() or __start */

	return task_load_segs_kmem(task, addr, &hdr);
}
