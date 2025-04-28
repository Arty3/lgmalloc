/* ******************************************** */
/*                                              */
/*   thread_ctx.c                               */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#include "internal_headers/lgmalloc_global_include.h"

#include <sys/syscall.h>

/* Should be managed internally here */
static _Thread_local TLS_MODEL
heap_t *__current_thread_heap_g = 0;

static ALWAYS_INLINE
void __set_current_thread_heap(heap_t *heap)
{
	__current_thread_heap_g = heap;
}

ALWAYS_INLINE
heap_t *__get_current_thread_heap(void)
{
	/* Avoid possible NULL in critical point. No weird unsafe
	 * global thread fallbacks. It should be initialized.
	 * The init function is responsible for proper init, 
	 * there is also a possible recursion issue
	 * here that needs special attention */
	if (UNLIKELY(!lgmalloc_is_init()))
		lgmalloc_init();

	return __current_thread_heap_g;
}

ALWAYS_INLINE
uintptr_t __get_tid(void)
{
#if !defined(__APPLE__) && (defined(__aarch64__) || defined(__x86_64__))
	/* Should work because of clang / gcc */
	return (uintptr_t)__builtin_thread_pointer();
#else
	uintptr_t tid;
#if defined(__i386__)
	__asm__("movl %%gs:0, %0" : "=r"(tid) : :);
#elif defined(__x86_64__)
#if defined(__MACH__)
	__asm__("movq %%gs:0, %0" : "=r"(tid) : :);
#else
	__asm__("movq %%fs:0, %0" : "=r"(tid) : :);
#endif
#elif defined(__arm__)
	__asm__ __volatile__("mrc p15, 0, %0, c13, c0, 3" : "=r"(tid));
#elif defined(__aarch64__)
#if defined(__MACH__)
	__asm__ __volatile__("mrs %0, tpidrro_el0" : "=r"(tid));
#else
	__asm__ __volatile__("mrs %0, tpidr_el0" : "=r"(tid));
#endif
#else
#if defined(SYS_gettid)
	tid = (uintptr_t)syscall(SYS_gettid);
	/* Fallback using unique thread heap address */
	if (UNLIKELY(tid == (uintptr_t)-1))
		tid = (uintptr_t)__get_current_thread_heap();
#else
	/* Direct fallback if SYS_gettid not available */
	tid = (uintptr_t)__get_current_thread_heap();
#endif
#endif
	return tid;
#endif
}

EXTERN_STRONG_ALIAS(__get_tid, lgmalloc_get_tid);
EXTERN_STRONG_ALIAS(__get_current_thread_heap, get_current_thread_heap);
