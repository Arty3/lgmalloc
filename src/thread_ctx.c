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

static ALWAYS_INLINE
heap_t *__unsafe_get_current_thread_heap(void)
{
	return __current_thread_heap_g;
}

ALWAYS_INLINE
heap_t *__get_current_thread_heap(void)
{
	/* Avoid possible NULL in critical point. No weird unsafe
	 * global thread fallbacks. It should be initialized.
	 * The init function is responsible for proper init, 
	 * there is also a possible recursion issue
	 * here that needs special attention */
	if (UNLIKELY(!__unsafe_get_current_thread_heap()))
		thread_ctx_init();

	return __unsafe_get_current_thread_heap();
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

/*
 * There are two main approaches for initializing.
 * You can configure these at compile-time.
 *
 * The default:
 *     We make an assumption that the system has the resources
 *     to allow us a larger memory footprint, allocating a massive
 *     memory map where we can systematically allocate our malloc
 *     structures on. This helps with syscall overhead, improves
 *     locality, i.e. improving caching behavior, simplified
 *     structure management and memory tracking and significantly
 *     reducing memory fragmentation. This further benefits
 *     from dynamic sizeclass determinations and possibly
 *     malloc predictability at compile-time. Remember that
 *     anything over `LGMALLOC_MMAP_THRESHOLD` (commonly 256kb)
 *     will enter its own respective memory mapping logic.
 * 
 *     Though, this comes at a tradeoff: The initial footprint
 *     will be significant, by allocating everything upfront.
 *     This isn't too much of an issue in the context of libc_init
 *     and long-life threads / daemons. Growth flexibility is also
 *     significantly reduced, where structures might need to grow
 *     independently. Finally, memory release granularity is flawed.
 *     We can't release chunks of a memory map when they're no longer
 *     needed. So essentially we're making the assumption that
 *     we aren't in a constrained memory environment and the initial
 *     overhead isn't a deal breaker.
 *
 * Multiple mmap calls:
 *     This approach is much more viable for short-lived or constrained
 *     systems. Where instead of having one big memory mapping for
 *     everything we need, we create multiple memory maps for each
 *     structure to grow independently, so that we can also release
 *     these when they are no longer needed. This will significantly
 *     reduce the memory footprint. Though, the initial performance
 *     overhead will be more due to more system calls, memory tracking
 *     and atomic operations. The memory footprint will also become
 *     much more dynamic and no longer a constant throughout runtime.
 * 
 * Both approaches are configurable at compile-time during the build
 * process. I recommend to use the multiple mmap calls approach for
 * highly constrained systems, where system resources (particularly ram)
 * are much more of a priority than speed and efficiency.
 * 
 * You should also consider that the implementation does a lot of
 * heuristic determinations in order to reduce the mmap block's size.
 * This is done by predetermening the size classes and usage of the
 * allocator, which gives us a heuristic approach to optimize the
 * allocator to the program's requirements and life-span.
 */
static void thread_ctx_init(void)
{
	if (UNLIKELY(__unsafe_get_current_thread_heap()))
		return;
}

EXTERN_STRONG_ALIAS(__get_tid, lgmalloc_get_tid);
EXTERN_STRONG_ALIAS(__get_current_thread_heap, get_current_thread_heap);
