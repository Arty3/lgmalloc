/* ******************************************** */
/*                                              */
/*   tests.c                                    */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#include "internal/lgmalloc_global_include.h"

#include <stddef.h>
#include <stdint.h>

/* Not the same as `tests/`.
 *
 * Tests here will be to ensure that no
 * weird convoluted issues will arise later.
 * 
 * These will only be compiled within debug
 * builds, and will only be called at init.
 * 
 * So for debugging it will serve as a guard.
 */

static ALWAYS_INLINE COLD_CALL
void __test_align_macros(void)
{
	LGMALLOC_ASSERT(ALIGN_UP(15, 8) == 16, "ALIGN_UP basic test failed");
	LGMALLOC_ASSERT(ALIGN_UP(16, 8) == 16, "ALIGN_UP boundary test failed");
	LGMALLOC_ASSERT(ALIGN_UP(17, 8) == 24, "ALIGN_UP overflow test failed");
	
	LGMALLOC_ASSERT(ALIGN_DOWN(15, 8) == 8,  "ALIGN_DOWN basic test failed");
	LGMALLOC_ASSERT(ALIGN_DOWN(16, 8) == 16, "ALIGN_DOWN boundary test failed");
	LGMALLOC_ASSERT(ALIGN_DOWN(17, 8) == 16, "ALIGN_DOWN overflow test failed");
	
	char buffer[100];
	void *aligned_ptr = ALIGN_PTR_UP(buffer, 16);
	LGMALLOC_ASSERT(IS_ALIGNED(aligned_ptr, 16), "Pointer alignment failed");
	
	size_t size = 100;
	size_t aligned_size = ALIGN_UP_TYPED(size, 64);
	LGMALLOC_ASSERT(aligned_size == 128,      "Type preservation failed");
	
	LGMALLOC_ASSERT(ALIGN_UP(0, 8)   == 0,    "Zero alignment test failed");
	LGMALLOC_ASSERT(ALIGN_DOWN(0, 8) == 0,    "Zero down-alignment test failed");
}

NO_INLINE COLD_CALL FLATTEN
void __debug_tests(void)
{
#ifdef _DEBUG
	__test_align_macros();
#else
	DISCARD_BRANCH;
#endif
}

EXTERN_STRONG_ALIAS(__debug_tests, do_debug_tests);
