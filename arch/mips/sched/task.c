
void task_init_trapframe(task_t *task)
{
	trapframe_t *tf = task->tf;
	memset(tf, 0, sizeof(*tf));
	tf->cp0_status = read_c0_status();
	tf->cp0_cause = read_c0_cause();
	tf->cp0_badvaddr = read_c0_badvaddr();
	/* not setting up EPC yet */
}

/*
 * task_bootstrap_context() redirects control to forkret(), a function
 * which further redirects control to program entry by restoring tweaked
 * trapframe.
 */
void task_bootstrap_context(task_t *task)
{
	context_t *ctx = &(task->context);
	ctx->cp0_status = read_c0_status();
	ctx->cp0_cause = read_c0_cause();
	ctx->cp0_badvaddr = read_c0_badvaddr();
	ctx->cp0_epc = (unsigned long)forkret;
	ctx->gpr[_A0] = (unsigned long)task->tf;
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

