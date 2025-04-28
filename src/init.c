/* ******************************************** */
/*                                              */
/*   init.c                                     */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#include "internal_headers/lgmalloc_global_include.h"

/* Global initializer flag, initializes on first call of api */

static int __is_lgmalloc_init_g = 0;

static ALWAYS_INLINE void lgmalloc_set_init(int value)
{
	__is_lgmalloc_init_g = !!value;
}

static ALWAYS_INLINE int lgmalloc_is_init(void)
{
	return __is_lgmalloc_init_g;
}

/* For scope and responsibility reasons this is
 * the only function that handles the init flags.
 * Either the caller handles the flag or the callee. */
void lgmalloc_init(void)
{
	/* More often than not will be initialized */
	if (LIKELY(lgmalloc_is_init()))
		return;

	/* logic */

	lgmalloc_set_init(1);
}

/* Just incase.. */
void lgmalloc_reinit(void)
{
	lgmalloc_set_init(0);
	lgmalloc_init();
}
