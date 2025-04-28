/* ******************************************** */
/*                                              */
/*   init.c                                     */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#include "internal_headers/lgmalloc_global_include.h"

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
