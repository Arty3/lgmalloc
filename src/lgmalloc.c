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

static ALWAYS_INLINE
void *__lgmalloc_impl(size_t size)
{
	/* Only inits on first call */
	lgmalloc_init();

	if (UNLIKELY(size > LGMALLOC_MAX_ALLOC_SIZE))
	{
		errno = EINVAL;
		return NULL;
	}

	if (UNLIKELY(!size))
	{
		/* Quickly do alloc of size 1 */
	}
}

void *__lgmalloc_wrapper(size_t size)
{
	return __lgmalloc_impl(size);
}

EXTERN_STRONG_ALIAS(__lgmalloc_wrapper, lgmalloc);

/* -DFORCE_LGMALLOC_REPLACE_STDLIB */
#if defined(FORCE_LGMALLOC_REPLACE_STDLIB)
EXTERN_STRONG_ALIAS(lgmalloc, malloc);
#endif

/* -DWEAK_LGMALLOC_REPLACE_STDLIB */
#if defined(WEAK_LGMALLOC_REPLACE_STDLIB)
EXTERN_WEAK_ALIAS(lgmalloc, malloc);
#endif
