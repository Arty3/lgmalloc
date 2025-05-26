/* ******************************************** */
/*                                              */
/*   heap.c                                     */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#include "internal_headers/lgmalloc_global_include.h"

#include <sys/mman.h>
#include <errno.h>

MALLOC_CALL(1) COLD_CALL
void *memory_map(size_t size)
{
	void *memory_map = mmap(
		NULL, size,
		PROT_READ   | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS,
		-1, 0
	);

	if (memory_map == MAP_FAILED)
	{
		errno = ENOMEM;
		return NULL;
	}

	return memory_map;
}

/*
 * Allocate the first sizeof(heap_t) bytes onto the
 * memory mapping for the thread's heap structure.
 * Because each thread has its own heap, we can use
 * the thread id to uniquely identify each heap. 
 * 
 * This function should be only be used by thread_ctx
 * to initialize the current thread context heap.
 */
static COLD_CALL FLATTEN inline
heap_t *heap_init(void *__restrict__ allocation, size_t size)
{
	heap_t *heap = (heap_t*)allocation;
	memset_constexpr(heap, 0, sizeof(heap_t));

	heap->tid = lgmalloc_get_tid();

	return heap;
}

