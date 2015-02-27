
#include <asm/mm/tlb.h>
#include <asm/mm/page.h>
#include <asm/mipsregs.h>
#include <asm/cp0regdef.h>

void tlb_flush_all(void)
{
	unsigned int i;
	for (i = 0; i < NUM_TLB_ENTRIES; ++i) {
		write_c0_index(i);
		write_c0_entryhi(ENTRYHI_DUMMY(i));
		write_c0_entrylo0(i);
		write_c0_entrylo1(i);
		tlbwi();
	}
}

void tlb_remove(ptr_t vaddr)
{
	/* get ASID */
	unsigned long asid = read_c0_entryhi() & ENTHI_ASID_MASK;
	/* write even VPN and ASID into ENTRYHI and query TLB */
	write_c0_entryhi(asid | ((vaddr >> (PGSHIFT + 1)) << (PGSHIFT + 1)));
	tlbp();

	/* check if the entry exists */
	unsigned int index = read_c0_index();
	unsigned long entrylo1 = read_c0_entrylo1();
	unsigned long entrylo0 = read_c0_entrylo0();

	if (index >= 0) {
		tlbr();
		/* remove requested virtual page first */
		if (vaddr & PGSIZE) {
			/* odd virtual page */
			write_c0_entrylo1(0);
			entrylo1 = 0;
		} else {
			/* even virtual page */
			write_c0_entrylo0(0);
			entrylo0 = 0;
		}
		/* If both entries are cleared, clean ENTRYHI as well.
		 * NOTE:
		 * ALWAYS clean ENTRYHI with ENTRYHI_DUMMY() macro.
		 * See arch/mips/include/asm/mm/tlbops.h for more comments. */
		if (!(entrylo1 || entrylo0))
			write_c0_entryhi(ENTRYHI_DUMMY(index));

		tlbwi();
	}
}
