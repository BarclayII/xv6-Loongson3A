
#include <asm/cp0regdef.h>
#include <asm/ptrace.h>
#include <asm/addrspace.h>
#include <asm/thread_info.h>
#include <asm/mm/tlb.h>
#include <sched/task.h>

int handle_pgfault(struct trapframe *tf)
{
	/* ASID switching is done with context switching */
	unsigned long entryhi = tf->cp0_entryhi;
	unsigned long asid = entryhi & ENTHI_ASID_MASK;
	entryhi ^= asid;
	/* First, check if current ASID is valid */
	assert(current_task->asid == asid);
	if (asid == ASID_INVALID) {
		if (current_next_asid == ASID_INVALID) {
			/* No ASID available, flush TLB and reset ASID */
			tlb_flush_all();
			asid_flush();
		}
		/* assign a new ASID to current task */
		current_task->asid = asid = current_next_asid++;
		/* register the PGD as online */
		online_hpt[asid] = current_task->mm->arch_mm.pgd;
		current_online_tasks[asid] = current_task;
		/* write the new ASID back into ENTRYHI */
		tf->cp0_entryhi = entryhi | asid;
		return 0;
	}
	return -1;
}
