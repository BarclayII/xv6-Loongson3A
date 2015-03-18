
void task_init_trapframe(task_t *task)
{
	trapframe_t *tf = task->tf;
	memset(tf, 0, sizeof(*tf));
	tf->cp0_status = read_c0_status();
	tf->cp0_cause = read_c0_cause();
	tf->cp0_badvaddr = read_c0_badvaddr();
	tf->cp0_entryhi = ASID_INVALID;
	/* not setting up EPC yet; this is done after locating entry and
	 * performed inside set_task_entry() */
}

/*
 * task_bootstrap_context() redirects control to forkret(), a function
 * which further redirects control to program entry by restoring tweaked
 * trapframe.
 */
void task_bootstrap_context(task_t *task, ptr_t sp)
{
	context_t *ctx = task->context;
	ctx->cp0_status = read_c0_status();
	ctx->cp0_cause = read_c0_cause();
	ctx->cp0_badvaddr = read_c0_badvaddr();
	ctx->cp0_epc = (unsigned long)forkret;
	ctx->gpr[_A0] = (unsigned long)task->tf;
	ctx->gpr[_SP] = (unsigned long)sp;
}

void set_task_user(task_t *task)
{
	trapframe_t *tf = task->tf;
	tf->cp0_status &= ~(ST_KSU | ST_ERL | ST_EXL);
	tf->cp0_status |= KSU_USER;
}

void set_task_enable_intr(task_t *task)
{
	trapframe_t *tf = task->tf;
	tf->cp0_status |= ST_IE;
}

int set_task_ustack(task_t *task)
{
	int ret, i;
	struct page *p = pgalloc();
	trapframe_t *tf = task->tf;
	ptr_t sp;
	ret = map_pages(task->mm, (void *)(task->progtop), p, USTACK_PERM);
	if (ret != 0)
		goto rollback_page;
	/* Set user stack top (or heap bottom) */
	task->ustacktop = task->progtop + USTACK_SIZE;
	return 0;

rollback_page:
	pgfree(p);
	return ret;
}

/*
 * Setup arguments passed into process, if applicable.
 * Pushes argument strings as well as their address onto user stack.
 * Returns a new stack top to be filled into trapframe of the task.
 *
 * @argv[] and its elements should reside in kernel space.
 */
addr_t set_task_argv(task_t *task, int argc, char *argv[])
{
	int i;
	size_t argv_space = 0;

	/* Calculate overall space taken by argument strings.
	 * Note that space occupied by one string @s is actually
	 * POW2_ROUNDUP(strlen(s) + 1, 3)
	 * since the address should be aligned to 8 bytes, and the null-
	 * terminator '\0' itself takes one byte. */
	for (i = argc - 1; i >= 0; --i)
		argv_space += POW2_ROUNDUP(strlen(argv[i]) + 1, 3);

	/* Push strings and their addresses one by one */
	ptr_t strtop = task->ustacktop;
	ptr_t kstrtop;
	ptr_t argvtop = strtop - argv_space;
	ptr_t kargvtop;
	size_t strspace;
	for (i = argc - 1; i >= 0; --i) {
		strspace = POW2_ROUNDUP(strlen(argv[i]) + 1, 3);
		strtop -= strspace;
		argvtop -= 8;
		if (copy_to_uvm(task->mm, strtop, argv[i], strspace) != 0)
			return 0;
		if (copy_to_uvm(task->mm, argvtop, &strtop, sizeof(ptr_t) != 0))
			return 0;
	}

	return argvtop;
}

void set_task_ustacktop(task_t *task, ptr_t sp)
{
	task->tf->gpr[_SP] = (addr_t)sp;
}

void set_task_main_args(task_t *task, int argc, char *argv[])
{
	task->tf->gpr[_A0] = (unsigned long)argc;
	task->tf->gpr[_A1] = (unsigned long)argv;
}

void set_task_entry(task_t *task, addr_t entry)
{
	trapframe_t *tf = task->tf;
	tf->cp0_epc = entry;
}
