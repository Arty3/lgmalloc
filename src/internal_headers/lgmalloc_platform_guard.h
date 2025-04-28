/* ******************************************** */
/*                                              */
/*   lgmalloc_platform_guard.h                  */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#ifdef __LGMALLOC_PLATFORM_GUARD_H

#if !defined(__clang__) || !defined(__GNUC__) || defined(_MSC_VER)
#error "lgmalloc requires the clang or gcc compiler"
#endif

#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(__CYGWIN__)
#error "detected Windows platform, lgmalloc requires a unix platform"
#endif

#endif /* __LGMALLOC_PLATFORM_GUARD_H */
