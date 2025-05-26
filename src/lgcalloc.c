/* ******************************************** */
/*                                              */
/*   lgcalloc.c                                 */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#include "internal_headers/lgmalloc_global_include.h"
#include "api/lgmalloc_config.h"

#include <immintrin.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

static ALWAYS_INLINE FLATTEN
void *__memzero_avx2(void *p, size_t n)
{
	__m256i zeros = _mm256_setzero_si_256();
	unsigned char *s = (unsigned char *p);

	for (; (uintptr_t)s % 32 && n; --n, ++s)
		*s = 0;

	__m256i *v = (__m256i *)s;

	for (; n >= 32; n -= 32, ++v)
		_mm256_storeu_si256(v, zeros);

	s = (unsigned char *)v;

	for (; n; --n, ++s)
		*s = 0;

	return p;
}

/* Check how many bytes are still needed to be cleared */
static FLATTEN inline
size_t __bytes_to_clear(void *__restrict__ p, size_t n)
{
	if (n < PAGE_SIZE)
		return n;

#ifdef __GNUC__
#if defined(LGMALLOC_64_BIT)
	typedef uint64_t __attribute__((__may_alias__)) T;
#elif defined(LGMALLOC_32_BIT)
	typedef uint32_t __attribute__((__may_alias__)) T;
#else
	typedef unsigned char T;
#endif
#else
	typedef unsigned char T;
#endif

	char *end = p + n;
	size_t i = (uintptr_t)end & (PAGE_SIZE - 1);

	while (1)
	{
		end = __cpu_supports_avx2()
			? __memzero_avx2(end - i, i)
			: memset(end - i, 0, i);

		if (end - p < PAGE_SIZE)
			return end - p;

		for (i = PAGE_SIZE; i; i -= 2 * sizeof(T), end -= 2 * sizeof(T))
			if (((T*)end)[-1] | ((T*)end)[-2])
				break;
	}
}

static ALWAYS_INLINE FLATTEN
int __is_already_zeroed_simd(void *p, size_t n)
{
	const unsigned char *s = (const unsigned char *)p;
	uintptr_t misalign = (uintptr_t)s & 31;

	if (misalign)
	{
		size_t prefix = 32 - misalign;

		if (prefix > n)
			prefix = n;

		for (; prefix; --prefix, ++s)
			if (*s) return 0;

		n -= (s - (const unsigned char *)p);
	}

	const __m256i *v = (const __m256i *)s;

	for (; n >= 32; n -= 32)
	{
		PREFETCH_STRUCTURE(
			v, CACHE_READ,
			TEMPORAL_LOCALITY_HIGH
		);
		__m256i chunk = _mm256_loadu_si256(v++);
		if (!_mm256_testz_si256(chunk, chunk))
			return 0;
	}

	s = (const unsigned char *)v;

	for (; n; --n, ++s)
		if (*s) return 0;

	return 1;
}

/* Check if buffer is zeroed out with SIMD-like approach for larger buffers */
static inline HOT_CALL FLATTEN
int __is_already_zeroed(void *p, size_t n)
{
	if (__cpu_supports_avx2())
		return __is_already_zeroed_simd(p, n);

	const unsigned char *s = (const unsigned char *)p;

	const unsigned char *start = s;
	uintptr_t misalign = (uintptr_t)s & (sizeof(word_t) - 1);

	if (misalign)
	{
		size_t prefix = sizeof(word_t) - misalign;
		n -= prefix;
		do
		{
			if (*s++)
				return 0;
		}	while (--prefix);
	}
	
	const word_t *w = (const word_t *)s;
	
	while (n >= 8 * sizeof(word_t))
	{
		if (w[0] | w[1] | w[2] | w[3] | w[4] | w[5] | w[6] | w[7])
			return 0;
		w += 8;
		n -= 8 * sizeof(word_t);
	}
	
	while (n >= sizeof(word_t))
	{
		if (*w++)
			return 0;
		n -= sizeof(word_t);
	}
	
	s = (const unsigned char *)w;
	while (n--)
	{
		if (*s++)
			return 0;
	}
	
	return 1;
}

static ALWAYS_INLINE MALLOC_CALL(1, 2) HOT_CALL
void *__lgcalloc_impl(size_t nmemb, size_t size)
{
	size_t total_size;
	int overflow = __builtin_umull_overflow(
					nmemb, size, &total_size);

	if (UNLIKELY(overflow))
	{
		errno = ENOMEM;
		return NULL;
	}

	if (UNLIKELY(total_size > LGMALLOC_MAX_ALLOC_SIZE))
	{
		errno = EINVAL;
		return NULL;
	}

	void *alloc = __lgmalloc_wrapper(total_size);

	if (UNLIKELY(!alloc))
		return alloc;

	if (total_size >= LGMALLOC_MMAP_THRESHOLD)
		if (UNLIKELY(__is_already_zeroed(alloc, total_size)))
			return alloc;

	total_size = __bytes_to_clear(alloc, total_size);

	return __cpu_supports_avx2()
		?  __memzero_avx2(alloc, total_size)
		:  memset(alloc, 0, total_size);
}

MALLOC_CALL(1, 2)
void *__lgcalloc_wrapper(size_t nmemb, size_t size)
{
	return __lgcalloc_impl(nmemb, size);
}

EXTERN_STRONG_ALIAS(__lgcalloc_wrapper, lgcalloc);

/* -DFORCE_LGMALLOC_REPLACE_STDLIB */
#if defined(FORCE_LGMALLOC_REPLACE_STDLIB)
EXTERN_STRONG_ALIAS(lgcalloc, calloc);
#endif

/* -DWEAK_LGMALLOC_REPLACE_STDLIB */
#if defined(WEAK_LGMALLOC_REPLACE_STDLIB)
EXTERN_WEAK_ALIAS(lgcalloc, calloc);
#endif
