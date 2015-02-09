
#ifndef _ASM_IRQ_H
#define _ASM_IRQ_H

#define local_irq_enable()	asm volatile ("ei");

#define local_irq_disable()	asm volatile ("di");

#endif
