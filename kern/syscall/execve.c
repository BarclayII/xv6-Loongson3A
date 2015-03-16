
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
}

void task_load_segs_kmem(task_t *task, void *addr, elf64hdr_t *hdr)
{
	struct elf64_phdr_t proghdr;
	size_t off = hdr->e_phoff;
	for (i = 0; i < hdr->e_phnum; ++i) {
		memcpy(&proghdr, addr + off, hdr->e_phentsize);
		switch(proghdr.p_type) {
		case PT_LOAD:
			task_load_seg_kmem(task, addr, &proghdr);
			break;
		default:
			break;
		}

		off += hdr->e_phentsize;
	}

rollback_unload:
}

/*
 * Load ELF program from kernel space memory into user space.
 * Handling statically linked 64-bit program only in this routine is enough.
 */
int task_load_elf_kmem(task_t *task, void *addr)
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

	task_load_segs_kmem(task, addr, &hdr);
}
