/* ******************************************** */
/*                                              */
/*   lgmalloc_global_include.h                  */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

/* The allocator's global includes */

#ifndef __LGMALLOC_GLOBAL_INCLUDE_H
#define __LGMALLOC_GLOBAL_INCLUDE_H

/* Verify compilation platform */
#define __LGMALLOC_PLATFORM_GUARD_H
#include "platform_guard.h"
#undef __LGMALLOC_PLATFORM_GUARD_H

/* Ensure POSIX.1-2008 standard */
#define _XOPEN_SOURCE 700

#include "lgmalloc_features.h"
#include "lgmalloc_types.h"

#include <stddef.h>

#endif /* __LGMALLOC_GLOBAL_INCLUDE_H */
