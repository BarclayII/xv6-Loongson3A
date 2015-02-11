/*
 * Copyright (C) 2007 Lemote Inc. & Insititute of Computing Technology
 * Author: Fuxin Zhang, zhangfx@lemote.com
 *
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <asm/mipsregs.h>
#include <asm/smp.h>
#include <asm/irq.h>
#include <irqregs.h>
#include <ht_regs.h>

unsigned int nr_cpu_handle_int;

volatile int *lpc_int_regs_ctrl;
volatile int *lpc_int_regs_enable;
volatile int *lpc_int_regs_status;
volatile int *lpc_int_regs_clear;

void rs780e_init_irq(void)
{
	/* Route the HT interrupt to Core0 INT1 */
	INT_router_regs_HT1_int0 = 0x21;
	INT_router_regs_HT1_int1 = 0x21;
	INT_router_regs_HT1_int2 = 0x21;
	INT_router_regs_HT1_int3 = 0x21;
	INT_router_regs_HT1_int4 = 0x21;
	INT_router_regs_HT1_int5 = 0x21;
	INT_router_regs_HT1_int6 = 0x21;
	INT_router_regs_HT1_int7 = 0x21;
	/* Enable the all HT interrupt */
	HT_irq_enable_reg0 = 0x0000ffff;
	HT_irq_enable_reg1 = 0x00000000;
	HT_irq_enable_reg2 = 0x00000000;
	HT_irq_enable_reg3 = 0x00000000;
	HT_irq_enable_reg4 = 0x00000000;
	HT_irq_enable_reg5 = 0x00000000;
	HT_irq_enable_reg6 = 0x00000000;
	HT_irq_enable_reg7 = 0x00000000;

	/* Enable the IO interrupt controller */ 
	IO_control_regs_Intenset = IO_control_regs_Inten | (0xffff << 16);

	/* Sets the first-level interrupt dispatcher. */
	/*mips_cpu_irq_init();	*/

	set_c0_status(ST_IMx(6));
}

void mach_init_irq(void)
{
	/*
	 * init all controller
	 * 0-15   ----> i8259 interrupt(south bridge i8259)
	 * 16-31  ----> loongson cpu interrupt
	 * 32-55  ----> bonito north bridge irq
	 * 56-63  ----> mips cpu interrupt
	 * 64-319 ----> ht devices interrupt
	 */
	nr_cpu_handle_int = NR_CPUS;
	/*
	 * Clear all of the interrupts while we change the able around a bit.
	 * int-handler is not on bootstrap
	 */
	clear_c0_status(ST_IM | ST_BEV);
	local_irq_disable();
	/* enable timer interrupt */
	set_c0_status(ST_IMx(7));

	rs780e_init_irq();
	/* enable south chip irq */
	set_c0_status(ST_IMx(3));

	/* cascade irq not used at this moment */
	/* setup_irq(56 + 3, &cascade_irqaction); */

	/* enable IPI interrupt */
	set_c0_status(ST_IMx(6));

	/* Route the LPC interrupt to Core0 INT0 */
	INT_router_regs_lpc_int = 0x11;
	/* Enable LPC interrupts for CPU UART */
	IO_control_regs_Intenset = (0x1<<10);

	/* added for KBC attached on ls3 LPC controler  */
	lpc_int_regs_ctrl	= (volatile int *)LS3_LPC_INT_regs_ctrl;
	lpc_int_regs_enable	= (volatile int *)LS3_LPC_INT_regs_enable;
	lpc_int_regs_status	= (volatile int *)LS3_LPC_INT_regs_status;
	lpc_int_regs_clear	= (volatile int *)LS3_LPC_INT_regs_clear;

	/* Enable the LPC interrupt */
	LPC_INT_regs_ctrl = 0x80000000;
	/* set the 18-bit interrupt enable bit for keyboard and mouse */
	LPC_INT_regs_enable = (0x1 << 0x1 | 0x1 << 12);
	/* clear all 18-bit interrupt bit */
	LPC_INT_regs_clear = 0x3ffff;

	/* enable serial and lpc port irq */
	set_c0_status(ST_IMx(2));
}
