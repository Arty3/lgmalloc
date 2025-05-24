/* ******************************************** */
/*                                              */
/*   lgmalloc_size_classes.h                    */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#ifndef __LGMALLOC_SIZE_CLASSES_H
#define __LGMALLOC_SIZE_CLASSES_H

#include "lgmalloc_features.h"
#include "lgmalloc_types.h"

#include "api/lgmalloc_config.h"

#include <stddef.h>
#include <stdint.h>

/* Implementations are all in header file
 * due to inlining and thread locality */

/* Size class abstraction
 * for memory blocks */
typedef struct __size_class_t
{
	size_t block_sz;	/* Block size  */
	size_t block_cnt;	/* Block count */
}	size_class_t;

/* Fallback size class configuration */

#define LGMALLOC_CHUNK_HEADER_SIZE			128
#define LGMALLOC_SEGMENT_HEADER_SIZE		128

_Static_assert(
	LGMALLOC_CHUNK_HEADER_SIZE == LGMALLOC_SEGMENT_HEADER_SIZE,
	"lgmalloc chunk and segment header sizes do not match"
);

#define LGMALLOC_SMALL_GRANULARITY			16

#define LGMALLOC_SMALL_CHUNK_SIZE_SHIFT		16
#define LGMALLOC_SMALL_CHUNK_SIZE			(1 << LGMALLOC_SMALL_CHUNK_SIZE_SHIFT)
#define LGMALLOC_SMALL_CHUNK_MASK			(~((uintptr_t)LGMALLOC_SMALL_CHUNK_SIZE - 1))

#define LGMALLOC_MEDIUM_CHUNK_SIZE_SHIFT	22
#define LGMALLOC_MEDIUM_CHUNK_SIZE			(1 << LGMALLOC_MEDIUM_CHUNK_SIZE_SHIFT)
#define LGMALLOC_MEDIUM_CHUNK_MASK			(~((uintptr_t)LGMALLOC_MEDIUM_CHUNK_SIZE - 1))

#define LGMALLOC_LARGE_CHUNK_SIZE_SHIFT		26
#define LGMALLOC_LARGE_CHUNK_SIZE			(1 << LGMALLOC_LARGE_CHUNK_SIZE_SHIFT)
#define LGMALLOC_LARGE_CHUNK_MASK			(~((uintptr_t)LGMALLOC_LARGE_CHUNK_SIZE - 1))

#define LGMALLOC_SEGMENT_SIZE				(256 * 1024 * 1024)
#define LGMALLOC_SEGMENT_MASK				(~((uintptr_t)(LGMALLOC_SEGMENT_SIZE - 1)))

#define LGMALLOC_SMALL_CLASS(n)			\
{										\
	(n * LGMALLOC_SMALL_GRANULARITY),	\
	(LGMALLOC_SMALL_CHUNK_SIZE   -		\
	 LGMALLOC_CHUNK_HEADER_SIZE) /		\
	(n * LGMALLOC_SMALL_GRANULARITY)	\
}

#define LGMALLOC_MEDIUM_CLASS(n)		\
{										\
	(n * LGMALLOC_SMALL_GRANULARITY),	\
	(LGMALLOC_MEDIUM_CHUNK_SIZE  -		\
	 LGMALLOC_CHUNK_HEADER_SIZE) /		\
	(n * LGMALLOC_SMALL_GRANULARITY)	\
}

#define LGMALLOC_LARGE_CLASS(n)			\
{										\
	(n * LGMALLOC_SMALL_GRANULARITY),	\
	(LGMALLOC_LARGE_CHUNK_SIZE   -		\
	 LGMALLOC_CHUNK_HEADER_SIZE) /		\
	(n * LGMALLOC_SMALL_GRANULARITY)	\
}

/* Default size classes which is then optimized
 * Will always be of size `size_class_count_g`
 */
static _Thread_local TLS_MODEL
size_class_t __size_classes_g[] =
{
	LGMALLOC_SMALL_CLASS(1),		LGMALLOC_SMALL_CLASS(1),		LGMALLOC_SMALL_CLASS(2),
	LGMALLOC_SMALL_CLASS(3),		LGMALLOC_SMALL_CLASS(4),		LGMALLOC_SMALL_CLASS(5),
	LGMALLOC_SMALL_CLASS(6),		LGMALLOC_SMALL_CLASS(7),		LGMALLOC_SMALL_CLASS(8),
	LGMALLOC_SMALL_CLASS(9),		LGMALLOC_SMALL_CLASS(10),		LGMALLOC_SMALL_CLASS(11),
	LGMALLOC_SMALL_CLASS(12),		LGMALLOC_SMALL_CLASS(13),		LGMALLOC_SMALL_CLASS(14),
	LGMALLOC_SMALL_CLASS(15),		LGMALLOC_SMALL_CLASS(16),		LGMALLOC_SMALL_CLASS(17),
	LGMALLOC_SMALL_CLASS(18),		LGMALLOC_SMALL_CLASS(19),		LGMALLOC_SMALL_CLASS(20),
	LGMALLOC_SMALL_CLASS(21),		LGMALLOC_SMALL_CLASS(22),		LGMALLOC_SMALL_CLASS(23),
	LGMALLOC_SMALL_CLASS(24),		LGMALLOC_SMALL_CLASS(25),		LGMALLOC_SMALL_CLASS(26),
	LGMALLOC_SMALL_CLASS(27),		LGMALLOC_SMALL_CLASS(28),		LGMALLOC_SMALL_CLASS(29),
	LGMALLOC_SMALL_CLASS(30),		LGMALLOC_SMALL_CLASS(31),		LGMALLOC_SMALL_CLASS(32),
	LGMALLOC_SMALL_CLASS(33),		LGMALLOC_SMALL_CLASS(34),		LGMALLOC_SMALL_CLASS(35),
	LGMALLOC_SMALL_CLASS(36),		LGMALLOC_SMALL_CLASS(37),		LGMALLOC_SMALL_CLASS(38),
	LGMALLOC_SMALL_CLASS(39),		LGMALLOC_SMALL_CLASS(40),		LGMALLOC_SMALL_CLASS(41),
	LGMALLOC_SMALL_CLASS(42),		LGMALLOC_SMALL_CLASS(43),		LGMALLOC_SMALL_CLASS(44),
	LGMALLOC_SMALL_CLASS(45),		LGMALLOC_SMALL_CLASS(46),		LGMALLOC_SMALL_CLASS(47),
	LGMALLOC_SMALL_CLASS(48),		LGMALLOC_SMALL_CLASS(49),		LGMALLOC_SMALL_CLASS(50),
	LGMALLOC_SMALL_CLASS(51),		LGMALLOC_SMALL_CLASS(52),		LGMALLOC_SMALL_CLASS(53),
	LGMALLOC_SMALL_CLASS(54),		LGMALLOC_SMALL_CLASS(55),		LGMALLOC_SMALL_CLASS(56),
	LGMALLOC_SMALL_CLASS(57),		LGMALLOC_SMALL_CLASS(58),		LGMALLOC_SMALL_CLASS(59),
	LGMALLOC_SMALL_CLASS(60),		LGMALLOC_SMALL_CLASS(61),		LGMALLOC_SMALL_CLASS(62),
	LGMALLOC_SMALL_CLASS(63),		LGMALLOC_SMALL_CLASS(64),		LGMALLOC_SMALL_CLASS(80),
	LGMALLOC_SMALL_CLASS(96),		LGMALLOC_SMALL_CLASS(112),		LGMALLOC_SMALL_CLASS(128),
	LGMALLOC_SMALL_CLASS(160),		LGMALLOC_SMALL_CLASS(192),		LGMALLOC_SMALL_CLASS(224),
	LGMALLOC_SMALL_CLASS(256),		LGMALLOC_MEDIUM_CLASS(320),		LGMALLOC_MEDIUM_CLASS(384),
	LGMALLOC_MEDIUM_CLASS(448),		LGMALLOC_MEDIUM_CLASS(512),		LGMALLOC_MEDIUM_CLASS(640),
	LGMALLOC_MEDIUM_CLASS(768),		LGMALLOC_MEDIUM_CLASS(896),		LGMALLOC_MEDIUM_CLASS(1024),
	LGMALLOC_MEDIUM_CLASS(1280),	LGMALLOC_MEDIUM_CLASS(1536),	LGMALLOC_MEDIUM_CLASS(1792),
	LGMALLOC_MEDIUM_CLASS(2048),	LGMALLOC_MEDIUM_CLASS(2560),	LGMALLOC_MEDIUM_CLASS(3072),
	LGMALLOC_MEDIUM_CLASS(3584),	LGMALLOC_MEDIUM_CLASS(4096),	LGMALLOC_MEDIUM_CLASS(5120),
	LGMALLOC_MEDIUM_CLASS(6144),	LGMALLOC_MEDIUM_CLASS(7168),	LGMALLOC_MEDIUM_CLASS(8192),
	LGMALLOC_MEDIUM_CLASS(10240),	LGMALLOC_MEDIUM_CLASS(12288),	LGMALLOC_MEDIUM_CLASS(14336),
	LGMALLOC_MEDIUM_CLASS(16384),	LGMALLOC_LARGE_CLASS(20480),	LGMALLOC_LARGE_CLASS(24576),
	LGMALLOC_LARGE_CLASS(28672),	LGMALLOC_LARGE_CLASS(32768),	LGMALLOC_LARGE_CLASS(40960),
	LGMALLOC_LARGE_CLASS(49152),	LGMALLOC_LARGE_CLASS(57344),	LGMALLOC_LARGE_CLASS(65536),
	LGMALLOC_LARGE_CLASS(81920),	LGMALLOC_LARGE_CLASS(98304),	LGMALLOC_LARGE_CLASS(114688),
	LGMALLOC_LARGE_CLASS(131072),	LGMALLOC_LARGE_CLASS(163840),	LGMALLOC_LARGE_CLASS(196608),
	LGMALLOC_LARGE_CLASS(229376),	LGMALLOC_LARGE_CLASS(262144),	LGMALLOC_LARGE_CLASS(327680),
	LGMALLOC_LARGE_CLASS(393216),	LGMALLOC_LARGE_CLASS(458752),	LGMALLOC_LARGE_CLASS(524288)
};

static _Thread_local TLS_MODEL size_t __size_class_count_g
	= sizeof(__size_classes_g) / sizeof(size_class_t);

/* Clears out size classes above the mmap threshold.
 *
 * This is possible to do at compile time, however,
 * due to the nature of C's preprocessing this would
 * become highly unmaintainable and unscalable.
 * 
 * Therefore this is done at runtime in
 * one function, the performance overhead
 * is neglegiable and the benefits of this
 * far outweigh the compile-time benefits.
 * 
 * This function is also highly unsafe
 * and requires special attention.
 */
static _Thread_local inline
void __clean_size_classes(void)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpointer-arith"
	/* write, read, end ptrs */
	size_class_t *wrp		= __size_classes_g;
	size_class_t *rdp		= __size_classes_g;
	size_class_t *const edp	= __size_classes_g +
							  __size_class_count_g;

	for (; rdp < edp; ++rdp)
		if (rdp->block_sz >= LGMALLOC_MMAP_THRESHOLD)
			if (wrp++ != rdp)
				*(wrp - 1) = *rdp;

	__size_class_count_g = wrp - __size_classes_g;

#if __has_builtin(__builtin_memset_inline)
	/* Avoid the loop if inline memset exists */
	const size_t bytes = (edp - wrp);
	if (bytes)
		__builtin_memset_inline(
			wrp, 0, bytes *
			sizeof(size_class_t)
		);
#else
	for (; wrp < edp; ++wrp)
		memset_constexpr(
			wrp, 0, sizeof(size_class_t)
		);
#endif /* __has_builtin(__builtin_memset_inline) */
#pragma clang diagnostic pop
}

/* Initializes the size class array.
 *
 * Removes classes above the mmap threshold.
 * Applies heuristics to better determine
 * the exact classes.
 */
static _Thread_local ALWAYS_INLINE
size_class_t *__build_size_classes(void)
{
	static int initialized = 0;
	
	if (LIKELY(!initialized))
	{
		__clean_size_classes();
		
		/* TODO: Apply heuristics.
		 *
		 * Consider that array length might
		 * need to change to accomodate this.
		 * 
		 * Therefore, a mmap call is possible.
		 */
		
		initialized = 1;
	}

	return __size_classes_g;
}

/* Get the size class array.
 *
 * Will always return initialized
 * and optimized size classes.
 *
 * Optionally pass a count pointer
 * to store the array size (recommended),
 * otherwise pass `NULL`.
 */
static _Thread_local ALWAYS_INLINE
size_class_t *get_size_classes(size_t *count)
{
	__build_size_classes();

	if (LIKELY(count))
		*count = __size_class_count_g;

	return __size_classes_g;
}

/* Get the size class for the given size.
 *
 * If the function returns 0,
 * there was a previous size
 * class misconfiguration
 */
static _Thread_local inline
size_t get_size_class(size_t size)
{
	/* To clarify this convoluted and unsafe mess:
	 *
	 * The `malloc(0)` edge case should already
	 * be handled earlier in the call stack,
	 * but in-case it's not, we return early.
	 * 
	 * We then calculate the minimum block count, rounded up.
	 * Then, for sizes up to 64 times the minimum granularity,
	 * the size class should already be predefined. Though,
	 * for safety, we also need to check the appropriate
	 * bounds and that the requested size can be held.
	 * 
	 * For larger allocations, we use bit manipulation.
	 * We calculate the position of the most significant bit,
	 * since the minimum block count is now guaranteed to be
	 * larger than 64, this position is guaranteed to be
	 * larger or equal to 6, this is still checked though.
	 * 
	 * Class sizes are then in the format `[...]000xxx000[...]`
	 * where we already have the position of the most significant
	 * bit, so then we can calculate the subclass from the
	 * remaining two bits.
	 * 
	 * Finally, we verify the bounds, the size class can hold
	 * the requested size and that we're not over-allocating.
	 */

	if (UNLIKELY(!size))
		return 1;

	size_t size_classes_count = 0;
	size_class_t *size_classes = get_size_classes(&size_classes_count);

	if (UNLIKELY(!size_classes_count))
		return size_classes_count;

	const size_t min_blk_cnt = (size + LGMALLOC_SMALL_GRANULARITY - 1)
									 / LGMALLOC_SMALL_GRANULARITY;

	if (size <= (LGMALLOC_SMALL_GRANULARITY * 64))
	{
		if (UNLIKELY(min_blk_cnt >= size_classes_count))
			return 0;

		if (UNLIKELY(size_classes[min_blk_cnt].block_sz < size))
			return 0;

		return min_blk_cnt;
	}

	const size_t search_val = min_blk_cnt - 1;

#if LGMALLOC_64_BIT
	const size_t most_sig_bit = (size_t)(63 - __builtin_clzl(search_val));
#else
	const size_t most_sig_bit = (size_t)(31 - __builtin_clz((unsigned int)search_val));
#endif

	if (UNLIKELY(most_sig_bit < 6))
		return 0;

	const size_t cls = ((most_sig_bit << 2)  +
		((search_val >> (most_sig_bit -  2)) & 0x03)) + 41;

	if (UNLIKELY((cls >= size_classes_count)			||
				 (size_classes[cls].block_sz < size)	||
				 (cls && size_classes[cls - 1].block_sz >= size)))
		return 0;

	return cls;
}

#endif /* __LGMALLOC_SIZE_CLASSES_H */
