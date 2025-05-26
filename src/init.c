/* ******************************************** */
/*                                              */
/*   init.c                                     */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#include "internal_headers/lgmalloc_global_include.h"

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

void config_init(void)
{
}

/* For scope and responsibility reasons this is
 * the only function that handles the init flags.
 * Either the caller handles the flag or the callee. */
COLD_CALL
void lgmalloc_init(void)
{
	/* More often than not will be initialized */
	if (LIKELY(__lgmalloc_is_init()))
		return;

	/* Initialize CPU feature detection */
	__builtin_cpu_init();

	/* logic */

	lgmalloc_set_init(1);
}

/* Just incase.. */
void lgmalloc_reinit(void)
{
	/* Cleanup */
	lgmalloc_set_init(0);
	lgmalloc_init();
}
