/* ******************************************** */
/*                                              */
/*   lgmalloc_features.h                        */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#ifndef __LGMALLOC_FEATURES_H
#define __LGMALLOC_FEATURES_H

/* Inlining features */
#define NO_INLINE		__attribute__((noinline))
#define ALWAYS_INLINE	__attribute__((always_inline))

/* Thread Local Storage (TLS) heap model */
#define TLS_MODEL		__attribute__((tls_model("initial-exec")))

/* Optimizing compilation target based on probability of logic */
#define LIKELY(x)		__builtin_expect(!!(x), 1)
#define UNLIKELY(x)		__builtin_expect(!!(x), 0)

/* Compile time constant implementations of `memcpy` and `memset` */
#ifdef __has_builtin
#if __has_builtin(__builtin_memcpy_inline)
#define memcpy_constexpr(x, y, s) __builtin_memcpy_inline(x, y, s)
#else
#define memcpy_constexpr(x, y, s)						\
	do													\
	{													\
		_Static_assert(__builtin_choose_expr(			\
			__builtin_constant_p(s), 1, 0),				\
			"memcpy `n` must be a constant integer");	\
		memcpy(x, y, s);								\
	}	while (0)
#endif
#if __has_builtin(__builtin_memset_inline)
#define memset_constexpr(x, y, s) __builtin_memset_inline(x, y, s)
#else
#define memset_constexpr(x, y, s)						\
	do													\
	{													\
		_Static_assert(__builtin_choose_expr(			\
			__builtin_constant_p(s), 1, 0),				\
			"memset `n` must be a constant integer");	\
		memset(x, y, s);								\
	}	while (0)
#endif
#else
/* Default fallback */
#define memcpy_constexpr(x, y, s) memcpy(x, y, s)
#define memset_constexpr(x, y, s) memset(x, y, s)
#endif

/* Alias lgmalloc to malloc & lgfree to free */

#define EXTERN_WEAK_ALIAS(old, new) extern __typeof(old) \
	new __attribute__((__weak__, __alias__(#old)))

#define EXTERN_STRONG_ALIAS(old, new) extern __typeof(old) \
	new __attribute__((__alias__(#old)))

#define LGMALLOC_STRINGIFY(x) #x

/* Check if architecture is 64bit or 32bit */
#if ARCH_64BIT
#define LGMALLOC_64_BIT
#else
#define LGMALLOC_32_BIT
#endif

/* PAGE_SIZE doesn't have a standardized location across platforms */
#if !defined(PAGE_SIZE)
#if defined(__linux__) || defined(linux) || defined(__linux)
#include <sys/user.h>
#elif defined(__APPLE__) || defined(__MACH__)
#include <mach/vm_param.h>			/* For older macOS */
#include <mach/machine/vm_param.h>	/* For newer macOS */
#ifndef PAGE_SIZE
#ifdef vm_page_size
#define PAGE_SIZE vm_page_size
#endif
#endif
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#include <sys/param.h>
#elif defined(__OpenBSD__) || defined(__NetBSD__)
#include <machine/param.h>
#else
/* POSIX sysconf runtime fallback */
#include <unistd.h>
#ifdef _SC_PAGESIZE
long __pagesz_g = sysconf(_SC_PAGESIZE);
#define PAGE_SIZE __pagesz_g
#endif
#endif
/* Sometimes defined as PAGESIZE */
#if defined(PAGESIZE)
#define PAGE_SIZE PAGESIZE
#endif
#endif

/* 4096 is the standardized page size */
#if !defined(PAGE_SIZE)
#warning "PAGE_SIZE couldn't be resolved. Using 4096 assumption."
#define PAGE_SIZE 4096
#endif

#endif /* __LGMALLOC_FEATURES_H */
