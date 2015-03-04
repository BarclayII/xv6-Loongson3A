
#include <mm/kmalloc.h>
#include <mm/mmap.h>
#include <mm/slab.h>
#include <sys/types.h>
#include <printk.h>
#include <stddef.h>

static void *kmalloc_large(size_t bytes)
{
	pdebug("Allocating large chunk with %d bytes\r\n", bytes);
	struct page *p;
	size_t pages = NR_PAGES_NEEDED(bytes);
	p = alloc_cont_pages(pages);
	if (p == NULL)
		return NULL;
	p->type = PGTYPE_GENERIC;
	pdebug("Allocated %d contiguous pages at %016x\r\n",
	    pages, PAGE_TO_KVADDR(p));
	return (void *)PAGE_TO_KVADDR(p);
}

void *kmalloc(size_t bytes)
{
	bytes = ROUNDUP(bytes, ALIGN);
	pdebug("Requested %d bytes allocation\r\n", bytes);
	if (bytes > KMALLOC_LARGE)
		return kmalloc_large(bytes);
	return slab_alloc(bytes);
}

static void kfree_large(void *ptr)
{
	pdebug("Freeing large chunk at %016x\r\n", ptr);
	free_pages(KVADDR_TO_PAGE((addr_t)ptr));
}

void kfree(void *ptr)
{
	/* Check and filter the NULL pointers */
	if (ptr == NULL)
		return;

	pdebug("Freeing chunk at %016x\r\n", ptr);
	if (is_slab(KVADDR_TO_PAGE((addr_t)ptr)))
		slab_free(ptr);
	else
		kfree_large(ptr);
}

