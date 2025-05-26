/* ******************************************** */
/*                                              */
/*   lgrealloc.c                                */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#include "internal_headers/lgmalloc_global_include.h"
#include "api/lgmalloc_config.h"

static ALWAYS_INLINE HOT_CALL
void *__lgrealloc_impl(void *ptr, size_t size)
{
}

void *__lgrealloc_wrapper(void *ptr, size_t size)
{
	return __lgrealloc_impl(ptr, size);
}

EXTERN_STRONG_ALIAS(__lgrealloc_wrapper, lgrealloc);

/* -DFORCE_LGMALLOC_REPLACE_STDLIB */
#if defined(FORCE_LGMALLOC_REPLACE_STDLIB)
EXTERN_STRONG_ALIAS(lgrealloc, realloc);
#endif

/* -DWEAK_LGMALLOC_REPLACE_STDLIB */
#if defined(WEAK_LGMALLOC_REPLACE_STDLIB)
EXTERN_WEAK_ALIAS(lgrealloc, realloc);
#endif
