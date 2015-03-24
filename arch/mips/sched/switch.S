
#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/stackframe.h>
#include <asm/ptrace.h>

/* void switch_context(context_t *old, context_t *new, ptr_t newkstack) */
LEAF(switch_context)
	.set	push
	.set	noreorder
	/* Old sp register is stored at t8 */
	/* TODO: rewrite switch_context()... */
	.set	pop
END(switch_context)

