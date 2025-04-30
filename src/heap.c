/* ******************************************** */
/*                                              */
/*   heap.c                                     */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#include "internal_headers/lgmalloc_global_include.h"

/*
 * Allocate the first sizeof(heap_t) bytes onto the
 * memory mapping for the thread's heap structure.
 * Because each thread has its own heap, we can use
 * the thread id to uniquely identify each heap. 
 * 
 * This function should be only be used by thread_ctx
 * to initialize the current thread context heap.
 */
static inline
heap_t *heap_init(void *mmap_region_ptr)
{
	heap_t *heap = (heap_t*)mmap_region_ptr;
	memset_constexpr(heap, 0, sizeof(heap_t));

	heap->tid = lgmalloc_get_tid();

	return heap;
}
