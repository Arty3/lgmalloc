/* ******************************************** */
/*                                              */
/*   lgmalloc.c                                 */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#include "internal_headers/lgmalloc_global_include.h"
#include "api/lgmalloc_config.h"

#include <errno.h>

static ALWAYS_INLINE COLD_CALL
void *do_alloc_size_1(void)
{
	/* Quickly do alloc of size 1 for param size 0 */
}

static ALWAYS_INLINE MALLOC_CALL(1) HOT_CALL
void *__lgmalloc_impl(size_t size)
{
#if !defined(MANUAL_HANDLE_LGMALLOC_INIT)
	lgmalloc_init();
#endif

	if (UNLIKELY(size > LGMALLOC_MAX_ALLOC_SIZE))
	{
		errno = EINVAL;
		return NULL;
	}

	if (UNLIKELY(!size))
		return do_alloc_size_1();
}

MALLOC_CALL(1)
void *__lgmalloc_wrapper(size_t size)
{
	return __lgmalloc_impl(size);
}

/* Don't alias to lgmalloc, we wrap it in a macro
 * for sizeclass and mmap heuristic determinations */
EXTERN_STRONG_ALIAS(__lgmalloc_wrapper, __lgmalloc);

/* -DFORCE_LGMALLOC_REPLACE_STDLIB */
#if defined(FORCE_LGMALLOC_REPLACE_STDLIB)
EXTERN_STRONG_ALIAS(lgmalloc, malloc);
#endif

/* -DWEAK_LGMALLOC_REPLACE_STDLIB */
#if defined(WEAK_LGMALLOC_REPLACE_STDLIB)
EXTERN_WEAK_ALIAS(lgmalloc, malloc);
#endif
