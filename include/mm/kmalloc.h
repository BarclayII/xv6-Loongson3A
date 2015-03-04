
#ifndef _MM_KMALLOC_H
#define _MM_KMALLOC_H

#include <mm/mmap.h>
#include <sys/types.h>

#define KMALLOC_LARGE	(PGSIZE >> 1)
#define ALIGN		8

void *kmalloc(size_t bytes);
void kfree(void *ptr);

#endif
