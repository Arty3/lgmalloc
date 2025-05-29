/* ******************************************** */
/*                                              */
/*   lgmalloc_decommit.h                        */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#ifndef __LGMALLOC_DECOMMIT_H
#define __LGMALLOC_DECOMMIT_H

#include "lgmalloc_features.h"

#include <stddef.h>
#include <stdint.h>

#ifdef LGMALLOC_ENABLE_DECOMMIT

#if defined(__linux__)
	/* Linux: Use MADV_DONTNEED to free physical pages */
	#define __vm_decommit(ptr, size)					\
		do												\
		{												\
			if (LIKELY((ptr) && (size)))				\
				madvise((ptr), (size), MADV_DONTNEED);	\
		}	while (0)

#elif defined(__APPLE__) || defined(__MACH__)
	/* macOS/Darwin: Use MADV_FREE (faster) or MADV_DONTNEED (more aggressive) */
	#define __vm_decommit(ptr, size)													\
		do																				\
		{																				\
			if (LIKELY((ptr) && (size)))												\
			{																			\
				/* Try MADV_FREE first (lazy decommit), fallback to MADV_DONTNEED */	\
				if (madvise((ptr), (size), MADV_FREE))									\
					madvise((ptr), (size), MADV_DONTNEED);								\
			}																			\
		}	while (0)

#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
	/* BSD variants: Use MADV_FREE if available, otherwise MADV_DONTNEED */
	#ifdef MADV_FREE
		#define __vm_decommit(ptr, size)						\
			do													\
			{													\
				if (LIKELY((ptr) && (size)))					\
					if (madvise((ptr), (size), MADV_FREE))		\
						madvise((ptr), (size), MADV_DONTNEED);	\
			}	while (0)
	#else
		#define __vm_decommit(ptr, size)					\
			do												\
			{												\
				if (LIKELY((ptr) && (size)))				\
					madvise((ptr), (size), MADV_DONTNEED);	\
			}	while (0)
	#endif

#elif defined(__sun) || defined(__solaris__)
	/* Solaris: Use MADV_DONTNEED */
	#define __vm_decommit(ptr, size)					\
		do												\
		{												\
			if (LIKELY((ptr) && (size)))				\
				madvise((ptr), (size), MADV_DONTNEED);	\
		}	while (0)

#elif defined(_AIX)
	/* AIX: Use MADV_DONTNEED */
	#define __vm_decommit(ptr, size)					\
		do												\
		{												\
			if (LIKELY((ptr) && (size)))				\
				madvise((ptr), (size), MADV_DONTNEED);	\
		}	while (0)

#else
	/* Generic Unix fallback: Try madvise with MADV_DONTNEED */
	#define __vm_decommit(ptr, size)							\
		do														\
		{														\
			if (LIKELY((ptr) && (size)))						\
				(void)madvise((ptr), (size), MADV_DONTNEED);	\
		}	while (0)
#endif

#endif /* LGMALLOC_ENABLE_DECOMMIT */

static ALWAYS_INLINE COLD_CALL
void vm_decommit(void *ptr, size_t size)
{
#ifdef LGMALLOC_ENABLE_DECOMMIT

	GUARANTEE(ptr,  "ptr must not be null");
	GUARANTEE(size, "size must not be 0");

	__vm_decommit(ptr, size);

#else

	DISCARD_ARGS(ptr, size);
	DISCARD_BRANCH;

#endif /* LGMALLOC_ENABLE_DECOMMIT */
}

static ALWAYS_INLINE COLD_CALL
void vm_decommit_aligned(void *ptr, size_t size)
{
#ifdef LGMALLOC_ENABLE_DECOMMIT

	GUARANTEE(ptr,  "ptr must not be null");
	GUARANTEE(size, "size must not be 0");

	uintptr_t start = (uintptr_t)ptr;
	uintptr_t end   = start + size;

	uintptr_t page_start = ALIGN_UP(start, PAGE_SIZE);
	uintptr_t page_end   = ALIGN_DOWN(end, PAGE_SIZE);

	if (UNLIKELY(page_end <= page_start))
		return;

	__vm_decommit((void*)page_start, page_end - page_start);

#else

	DISCARD_ARGS(ptr, size);
	DISCARD_BRANCH;

#endif /* LGMALLOC_ENABLE_DECOMMIT */
}

#endif /* __LGMALLOC_DECOMMIT_H */
