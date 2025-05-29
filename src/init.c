/* ******************************************** */
/*                                              */
/*   init.c                                     */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#include "internal_headers/lgmalloc_global_include.h"

/* Catch critical and convoluted issues early */
static ALWAYS_INLINE COLD_CALL
void __verify_align_macros(void)
{
#ifdef _DEBUG
	LGMALLOC_ASSERT(ALIGN_UP(15, 8) == 16, "ALIGN_UP basic test failed");
	LGMALLOC_ASSERT(ALIGN_UP(16, 8) == 16, "ALIGN_UP boundary test failed");
	LGMALLOC_ASSERT(ALIGN_UP(17, 8) == 24, "ALIGN_UP overflow test failed");
	
	LGMALLOC_ASSERT(ALIGN_DOWN(15, 8) == 8,  "ALIGN_DOWN basic test failed");
	LGMALLOC_ASSERT(ALIGN_DOWN(16, 8) == 16, "ALIGN_DOWN boundary test failed");
	LGMALLOC_ASSERT(ALIGN_DOWN(17, 8) == 16, "ALIGN_DOWN overflow test failed");
	
	char buffer[100];
	void *aligned_ptr = ALIGN_PTR_UP(buffer, 16);
	LGMALLOC_ASSERT(IS_ALIGNED(aligned_ptr, 16), "Pointer alignment failed");
	
	size_t size = 100;
	size_t aligned_size = ALIGN_UP_TYPED(size, 64);
	LGMALLOC_ASSERT(aligned_size == 128,      "Type preservation failed");
	
	LGMALLOC_ASSERT(ALIGN_UP(0, 8)   == 0,    "Zero alignment test failed");
	LGMALLOC_ASSERT(ALIGN_DOWN(0, 8) == 0,    "Zero down-alignment test failed");
	
	LGMALLOC_ASSERT(IS_POWER_OF_2(8),         "8 should be power of 2");
	LGMALLOC_ASSERT(IS_POWER_OF_2(16),        "16 should be power of 2");
	LGMALLOC_ASSERT(IS_POWER_OF_2(PAGE_SIZE), "PAGE_SIZE should be power of 2");
	LGMALLOC_ASSERT(!IS_POWER_OF_2(15),       "15 should not be power of 2");
#else
	DISCARD_BRANCH;
#endif
}

static int __is_lgmalloc_init_g = 0;

static ALWAYS_INLINE HOT_CALL
void lgmalloc_set_init(int value)
{
	__is_lgmalloc_init_g = !!value;
}

static ALWAYS_INLINE HOT_CALL
int __lgmalloc_is_init(void)
{
	return __is_lgmalloc_init_g;
}

/* Exposing this for critical points that require extra safety */
HOT_CALL FLATTEN
int lgmalloc_is_init(void)
{
	return __lgmalloc_is_init();
}

/*
 * Each thread uniquely holds a large allocation
 * That is partitioned to hold all the necessary
 * structures and memory. The layout is as depicted
 * below, where each memory block will hold its
 * respective memory area adjacent to it.
 * 
 * The goal for this layout is to minimize the look-ahead
 * for each structure member. Tightly packing these in
 * memory allows for a constant lookup that is predictable
 * throughout the life-span of the program. This also
 * improves hardware caching mechanisms.
 * 
 * The key here is the dynamic size class generation,
 * which through the use of heuristics we can predict
 * at a further accuracy, minimizing the actual allocation
 * size needed for each thread to run. Otherwise, a
 * constant sized allocation of (for example) 1mb will
 * clog up memory the more threads you create.
 *
 *		+----------------------+  <- Start
 *		| Heap structure       |
 *		+----------------------+  <- + sizeof(heap_t)
 *		| Segment structure    |
 *		+----------------------+  <- + sizeof(segment_t)
 *		| Chunk structure 1    |
 *		+----------------------+  <- + sizeof(chunk_t)
 *		| Block structure 1    |
 *		+----------------------+  <- + sizeof(block_t)
 *		| Raw memory for use   |
 *		+----------------------+  <- + block_1->size
 *		| Block structure 2    |
 *		+----------------------+  <- + sizeof(block_t)
 *		| Raw memory for use   |
 *		+----------------------+  <- + block_2->size
 *		| Block structure ...  |
 *		+----------------------+  <- + sizeof(block_t)
 *		| Raw memory for use   |
 *		+----------------------+  <- + block_n->size
 *		| Chunk structure ...  |
 *		+----------------------+  <- + sizeof(chunk_t)
 */
size_t calculate_init_mmap_layout_size(void)
{
}

/* For scope and responsibility reasons this is
 * the only function that handles the init flags.
 * Either the caller handles the flag or the callee. */
COLD_CALL NO_INLINE
void lgmalloc_init(void)
{
	if (UNLIKELY(__lgmalloc_is_init()))
		return;

	__verify_align_macros();

	/* logic */

	lgmalloc_set_init(1);
}
