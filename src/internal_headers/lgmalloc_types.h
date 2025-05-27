/* ******************************************** */
/*                                              */
/*   lgmalloc_types.h                           */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#ifndef __LGMALLOC_TYPES_H
#define __LGMALLOC_TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef unsigned long int __attribute__ ((__may_alias__)) word_t;

/* Forward decls */
typedef struct __block_t	block_t;
typedef struct __chunk_t	chunk_t;
typedef struct __segment_t	segment_t;
typedef struct __mmap_t		mmap_t;
typedef struct __heap_t		heap_t;

/*
 * A raw memory block. It's the lowest abstraction
 * from a raw memory slab. This holds the raw memory
 * with a size of its respective size class.
 * 
 * Raw memory blocks are handled by chunks
 * which hold a linked list of memory blocks
 * all of the same size class (e.g. 8 bytes).
 */
typedef struct __block_t
{
	struct __block_t	*next;
	uintptr_t			alloc;
	/* Remove size field later if not needed */
	size_t				size;
}	block_t;

/*
 * A memory chunk which has all memory blocks within
 * it partitioned to the same size. This allows for
 * a simple linked list traversal of memory blocks
 * for efficient allocation tracking.
 * 
 * The key difference here is that all memory
 * blocks within a chunk are of the same size
 * while a segment holds chunks which in turn
 * hold blocks of different size classes.
 * 
 * Therefore a segment will have chunks
 * containing blocks of e.g. 8, 16, 32 etc.
 * 
 * aka. page
 */
typedef struct __chunk_t
{
	struct __chunk_t	*next;
	struct __chunk_t	*prev;
	size_t				size_class;
	size_t				block_count;
	size_t				blocks_in_use;
	int					is_full;
	segment_t			*parent_segment;
}	chunk_t;

/* 
 * A large memory mapping which is then split into chunks
 * based on size classes. This approach avoids the overhead
 * of constantly asking the kernel for extra memory for
 * every malloc call. Instead, we do one large memory mapping
 * segment it out, split it into size classes so we can prepare
 * pre-aligned memory for the requesting user.
 * 
 * If the `size` argument >= LGMALLOC_MMAP_THRESHOLD
 * then we call a dedicated memory mapping to not waste
 * segments which are already prepped for smaller and
 * predictable allocations. This also optimizes
 * handling over larger memory areas for the user.
 * 
 * aka. arena, span, etc
 */
typedef struct __segment_t
{
	struct __segment_t	*next;
	heap_t				*parent_heap;
	chunk_t				*chunk_list;
	size_t				chunk_count;
	size_t				segment_size;
	uintptr_t			mmap_start;
}	segment_t;

/*
 * Linked list holding all dedicated memory mappings.
 * This helps simplify freeing process, since
 * calling free on this will directly go to `munmap`
 */
typedef struct __mmap_t
{
	struct __mmap_t	*next;
	void			*alloc;
	size_t			size;
}	mmap_t;

/* 
 * Thread-unique structure holding all heap data 
 * This avoids atomic & locking overhead and
 * maintains performance scalability across
 * threads. Since each thread will have their
 * own memory segments and memory management
 * the memory footprint won't scale like it
 * does in tcmalloc which has a centralized
 * list handling method for all threads.
 */
typedef struct __heap_t
{
	uintptr_t		tid;
	segment_t		*segment_list;
	size_t			segment_count;
	mmap_t			*mmap_list;
	size_t			mmap_count;
}	heap_t;

#define LGMALLOC_BLOCK_T_SIZE	sizeof(block_t)
#define LGMALLOC_CHUNK_T_SIZE	sizeof(chunk_t)
#define LGMALLOC_SEGMENT_T_SIZE	sizeof(segment_t)
#define LGMALLOC_MMAP_T_SIZE	sizeof(mmap_t)
#define LGMALLOC_HEAP_T_SIZE	sizeof(heap_t)

_Static_assert(
	LGMALLOC_BLOCK_T_SIZE % sizeof(max_align_t) == 0,
	"mmap_t size must preserve alignment"
);
_Static_assert(
	LGMALLOC_CHUNK_T_SIZE % sizeof(max_align_t) == 0,
	"mmap_t size must preserve alignment"
);
_Static_assert(
	LGMALLOC_SEGMENT_T_SIZE % sizeof(max_align_t) == 0,
	"mmap_t size must preserve alignment"
);
_Static_assert(
	LGMALLOC_MMAP_T_SIZE % sizeof(max_align_t) == 0,
	"mmap_t size must preserve alignment"
);
_Static_assert(
	LGMALLOC_HEAP_T_SIZE % sizeof(max_align_t) == 0,
	"mmap_t size must preserve alignment"
);

#define OFFSET_PTR(p, of)	(void*)((unsigned char*)(p) + (ptrdiff_t)(of))
#define PTR_DIFF(a, b)		(ptrdiff_t)((const unsigned char*)(a) - \
										(const unsigned char*)(b))

#endif /* __LGMALLOC_TYPES_H */
