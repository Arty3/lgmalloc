/* ******************************************** */
/*                                              */
/*   lgfree.c                                   */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#include "internal/lgmalloc_global_include.h"
#include "api/lgmalloc_config.h"

static ALWAYS_INLINE HOT_CALL
void __lgfree_impl(void *ptr)
{
#if !defined(MANUAL_HANDLE_LGMALLOC_INIT)
	lgmalloc_init();
#endif

	if (UNLIKELY(!ptr))
		return;
}

void __lgfree_wrapper(void *ptr)
{
	__lgfree_impl(ptr);
}

EXTERN_STRONG_ALIAS(__lgfree_wrapper, lgfree);

/* -DFORCE_LGMALLOC_REPLACE_STDLIB */
#if defined(FORCE_LGMALLOC_REPLACE_STDLIB)
EXTERN_STRONG_ALIAS(lgfree, free);
#endif

/* -DWEAK_LGMALLOC_REPLACE_STDLIB */
#if defined(WEAK_LGMALLOC_REPLACE_STDLIB)
EXTERN_WEAK_ALIAS(lgfree, free);
#endif

