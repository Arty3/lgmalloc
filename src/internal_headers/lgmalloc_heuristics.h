/* ******************************************** */
/*                                              */
/*   lgmalloc_heuristics.h                      */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#ifndef __LGMALLOC_HEURISTICS_H
#define __LGMALLOC_HEURISTICS_H

#include "lgmalloc_features.h"

#include <stddef.h>

#define LGMALLOC_HEURISTICS_SECTION ".lgmalloc_heuristics_data"

/*
 * Metadata for malloc calls throughout the user's source files.
 * Allows us to track and predict allocation patterns heurically.
 */
typedef struct
{
	size_t		size;
	const char	*file;
	int			line;
	const char	*func;
	size_t		freq;
	int			is_const;
}	lgmalloc_heuristic_entry_t;

/*
 * The initialization process relies on the respective
 * size classes when creating the memory mappings.
 * This attempts to improve the size class definitions
 * via a heuristic statistical approach.
 * 
 * These macros serve to wrap the lgmalloc call, allowing
 * us to track allocation patterns, predict size classes
 * and optimize memory mappings accordingly.
 * 
 * Two main branches exist. The most important is the
 * const call, i.e. tracking allocations where `size`
 * is a constant known at compile time. The second is
 * the runtime call tracker, where we don't know what
 * `size` will be.
 * 
 * The runtime call doesn't prove as effective since
 * we of course don't know the allocation size. Though,
 * the frequency of the calls give an insight into
 * overhead model designs and strategies.
 * 
 * The compile time call allows us to already predict
 * some, if not all, the allocations that will happen
 * throughout the program's life-span. This proves
 * HIGHLY useful in simple control flow programs.
 * 
 * Due to each thread having its own heap and memory
 * map, heuristics will be applied in a thread local
 * context based approach. Since we want to predict
 * size classes when we initialize, and we initialize
 * when a new thread is created and lgmalloc is called,
 * we can just restrict everything to a per-thread
 * context, avoiding unnecessary memory waste.
 * 
 * This functionality comes at a cost. The API will
 * not be as clean, because the lgmalloc function
 * (or the malloc symbol respectively) will become
 * a macro, therefore the type of `size` (size_t)
 * won't be evident to the average user. This
 * logic will also need to be exposed in the API.
 * 
 * We cannot wrap this logic within the function
 * itself as the heuristic approach will crumble,
 * losing call patterns and compile-time constants.
 */

#define __TRACK_LGMALLOC_RUNTIME_CALL(sz, file, line, func)						\
	do																			\
	{																			\
		__attribute__((section(LGMALLOC_HEURISTICS_SECTION), used))				\
		static _Thread_local TLS_MODEL lgmalloc_heuristic_entry_t				\
		__lgmalloc_track_##file##_##line = {									\
			.size = (sz), .file = #file, .line = (line),						\
			.func = (func), .freq = 0, .is_const = 0							\
		};	__lgmalloc_track_##file##_##line.freq++								\
	}	while (0)

#define __TRACK_LGMALLOC_CONST_CALL(sz)												\	
	do																				\
	{																				\
		__attribute__((section(LGMALLOC_HEURISTICS_SECTION ".const." #sz), used))	\
		static _Thread_local TLS_MODEL lgmalloc_heuristic_entry_t					\
		__lgmalloc_track_const_##__LINE__ = {										\
			.size = (sz), .file = __FILE__, .line = __LINE__,						\
			.func = __func__, .freq = 0, .is_const = 1								\
		};	__lgmalloc_track_const_##__LINE__.freq++								\
	}	while (0)

#define __TRACK_LGMALLOC_CALL(sz)					\
	do												\
	{												\
		if (__builtin_constant_p(sz))				\
			__TRACK_LGMALLOC_CONST_CALL(sz);		\
		else										\
			__TRACK_LGMALLOC_RUNTIME_CALL(			\
				sz, __FILE__, __LINE__, __func__	\
			);										\
	}	while (0)

#endif /* __LGMALLOC_HEURISTICS_H */
