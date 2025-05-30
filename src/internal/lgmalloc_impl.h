/* ******************************************** */
/*                                              */
/*   lgmalloc_impl.h                            */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#ifndef __LGMALLOC_IMPL_H
#define __LGMALLOC_IMPL_H

NO_INLINE COLD_CALL FLATTEN
void do_debug_tests(void);

NO_INLINE COLD_CALL FLATTEN
void init_prof_system(void);

void lgmalloc_init(void);
void lgmalloc_reinit(void);
int	 lgmalloc_is_init(void);

/* Wrappers for internal usage */

void	*__lgmalloc_wrapper(size_t size);
void	__lgfree_wrapper(void *ptr);
void	*__lgcalloc_wrapper(size_t nmemb, size_t size);
void	*__lgrealloc_wrapper(void *ptr, size_t size);

#endif /* __LGMALLOC_IMPL_H */
