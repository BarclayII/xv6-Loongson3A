
#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/stackframe.h>
#include <asm/ptrace.h>

/* void switch_context(context_t *old, context_t *new, ptr_t newkstack) */
LEAF(switch_context)
	/* Old sp register is stored at t8 */
	move	t8, sp
	move	sp, a0
	SAVE_ALL
	set_saved_sp a2, t9
	move	sp, a1
	RESTORE_ALL
	move	sp, t8
	jr	ra
	nop
END(switch_context)

