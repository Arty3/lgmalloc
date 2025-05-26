/* ******************************************** */
/*                                              */
/*   lgmalloc.h                                 */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#ifndef __LGMALLOC_H
#define __LGMALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

__attribute__((malloc, alloc_size(1)))
void *lgmalloc(size_t size);
void lgfree(void *ptr);
__attribute__((malloc, alloc_size(1, 2)))
void *lgcalloc(size_t nmemb, size_t size);
void *lgrealloc(void *ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* __LGMALLOC_H */
