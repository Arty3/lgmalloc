/* ******************************************** */
/*                                              */
/*   lgmalloc_config.h                          */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#ifndef __LGMALLOC_CONFIG_H
#define __LGMALLOC_CONFIG_H

#include <limits.h>

#define LGMALLOC_MAX_ALLOC_SIZE	SIZE_MAX / 2
#define LGMALLOC_MMAP_THRESHOLD	262144			/* 256kb */

#endif /* __LGMALLOC_CONFIG_H */
