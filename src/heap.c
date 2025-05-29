/* ******************************************** */
/*                                              */
/*   heap.c                                     */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#include "internal_headers/lgmalloc_global_include.h"
#include "internal_headers/lgmalloc_thread_ctx.h"
#include "internal_headers/lgmalloc_size_classes.h"

#include <sys/mman.h>
#include <errno.h>

#define LGMALLOC_TINY_THRESHOLD (LGMALLOC_SMALL_GRANULARITY * 64)

static ALWAYS_INLINE COLD_CALL PURE
size_t align_size_to_page(size_t size)
{
	GUARANTEE(size, "size must not be zero");
	return ALIGN_UP(size, PAGE_SIZE);
}

/* All mmap calls should go through here
 * Memory will always be committed. */
static MALLOC_CALL(1) COLD_CALL
void *memory_map(size_t size)
{
	GUARANTEE(size, "size must not be zero");

	void *memory_map = mmap(
		NULL, size,
		PROT_READ   | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS,
		-1, 0
	);

	if (memory_map == MAP_FAILED)
	{
		if (errno != EAGAIN)
			errno  = ENOMEM;

		return NULL;
	}

	return memory_map;
}

static MALLOC_CALL(1) COLD_CALL
mmap_t *get_dedicated_mmap(size_t size)
{
	GUARANTEE(size, "size must not be zero");

	size = align_size_to_page(size);

	void *alloc = INLINE_CALL(
		memory_map(size + LGMALLOC_MMAP_T_SIZE)
	);

	if (UNLIKELY(!alloc))
		return NULL;

	mmap_t *map = (mmap_t*)alloc;

	map->alloc	= alloc;
	map->size	= size;
	map->next	= NULL;

	return map;
}

static COLD_CALL
void store_dedicated_mmap(
	heap_t *__restrict__ heap,
	mmap_t *__restrict__ map)
{
	/* Avoid prefetching since the function is a cold call,
	 * the datastructures are decently small and will likely
	 * fit within 1 or 2 cache lines, there will likely also
	 * be very few dedicated memory maps, so the traversal
	 * will be very short and the memory will already be
	 * hot since the heap and map are constantly being
	 * used in the call graph. So prefetching here would
	 * almost double the instruction cycles. */

	GUARANTEE(heap, "heap must not be null");
	GUARANTEE(map,  "map must not be null");

	if (UNLIKELY(!heap->mmap_list))
	{
		heap->mmap_list  = map;
		heap->mmap_count = 1;
		return;
	}

	mmap_t *cur = heap->mmap_list;
	for(; cur->next; cur = cur->next);

	cur->next = map;
	++heap->mmap_count;
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
heap_t *heap_init(void *__restrict__ alloc, size_t size)
{
	GUARANTEE(alloc, "alloc must not be null");

	if (UNLIKELY(size < sizeof(heap_t)))
		return NULL;

	heap_t *heap = (heap_t*)alloc;
	memset_constexpr(heap, 0, sizeof(heap_t));

	heap->tid = lgmalloc_get_tid();

	return heap;
}

static MALLOC_CALL(2) ALWAYS_INLINE
void *do_tiny_alloc(heap_t *__restrict__ heap, size_t size)
{
	GUARANTEE(heap, "heap must not be null");
	GUARANTEE(size, "size must not be 0");

	ASSUME(size <= LGMALLOC_TINY_THRESHOLD);

	const size_t class = (((size_t)size + (LGMALLOC_SMALL_GRANULARITY - 1))
										/  LGMALLOC_SMALL_GRANULARITY);

	block_t *block = ...;

	if (UNLIKELY(!block))
		return NULL;

	return block;
}

static NO_INLINE MALLOC_CALL(2)
void *heap_alloc_mmap(heap_t *__restrict__ heap, size_t size)
{
	GUARANTEE(heap, "heap must not be null");
	GUARANTEE(size, "size must not be 0");

	mmap_t *map = get_dedicated_mmap(size);

	if (UNLIKELY(!map))
		return NULL;

	store_dedicated_mmap(heap, map);

	return OFFSET_PTR(map->alloc, LGMALLOC_MMAP_T_SIZE);
}

MALLOC_CALL(2) ALWAYS_INLINE
void *regular_heap_alloc(heap_t *__restrict__ heap, size_t size)
{
	GUARANTEE(heap, "heap must not be null");
	GUARANTEE(size, "size must not be 0");

	ASSUME(size > LGMALLOC_TINY_THRESHOLD);

	size_t class = get_size_class(size);

	if (UNLIKELY(!class))
		return NULL;

	if (LIKELY(class < get_size_class_count()))
	{
		block_t *block = ...;

		if (UNLIKELY(!block))
		{
			/* Fallback to allocating back on chunk */
			/* Needs to be handled efficiently */
		}

		return block;
	}

	return heap_alloc_mmap(heap, size);
}

MALLOC_CALL(2) HOT_CALL NO_INLINE
void *heap_alloc(heap_t *__restrict__ heap, size_t size)
{
	GUARANTEE(heap, "heap must not be null");
	GUARANTEE(size, "size must not be 0");

	if (size <= LGMALLOC_TINY_THRESHOLD)
	{
		block_t *block = do_tiny_alloc(heap, size);

		if (LIKELY(block))
			return block;
	}

	void *alloc = regular_heap_alloc(heap, size);

	if (UNLIKELY(!alloc))
		errno = errno != EAGAIN
			  ? ENOMEM : EAGAIN;

	return alloc;
}
