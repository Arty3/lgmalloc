/* ******************************************** */
/*                                              */
/*   profiling.c                                */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#include "internal/lgmalloc_global_include.h"
#include "api/lgmalloc_profiling.h"

/* Internal malloc profiling system
 * for production environments */

#include <execinfo.h>
#include <unwind.h>
#include <stddef.h>
#include <stdint.h>

#define __RET_ADDR_P(x)	\
	__builtin_return_address(x);

/* Keep structs opaque */

typedef struct __prof_config_t
{
	int				enabled;
	unsigned int	sample_rate;
	unsigned int	stack_depth;
}	prof_config_t;

typedef struct __lgmalloc_prof_stats_t
{
	size_t	current_allocs;
	size_t	current_bytes;
	size_t	total_allocs;
	size_t	total_frees;
	size_t	total_bytes;
}	lgmalloc_prof_stats_t;

typedef struct __lgmalloc_prof_site_t
{
	lgmalloc_call_site_id_t		id;
	lgmalloc_prof_site_rank_t	rank;
	lgmalloc_prof_site_freq_t	freq;
	int							line;
	const char					*file;
	const char					*func;
}	lgmalloc_prof_site_t;

static _Thread_local TLS_MODEL
prof_config_t *__current_thread_config_g = 0;

static ALWAYS_INLINE COLD_CALL
void __set_current_thread_config(prof_config_t *config)
{
	GUARANTEE(config, "config must not be NULL");
	__current_thread_config_g = config;
}

static ALWAYS_INLINE PURE HOT_CALL
prof_config_t *__unsafe_get_current_thread_config(void)
{
	return __current_thread_config_g;
}

NO_INLINE COLD_CALL FLATTEN
void __init_prof_system(void)
{
#ifdef LGMALLOC_ENABLE_PROFILING
#else
	DISCARD_BRANCH;
#endif
}

EXTERN_STRONG_ALIAS(__init_prof_system, init_prof_system);

static ALWAYS_INLINE HOT_CALL
prof_config_t *get_current_thread_config(void)
{
	if (UNLIKELY(!__unsafe_get_current_thread_config()))
		__init_prof_system();

	return __unsafe_get_current_thread_config();
}

int __lgmalloc_prof_reset_stats(void)
{
#ifdef LGMALLOC_ENABLE_PROFILING
	return LGMALLOC_PROF_RET_SUCCESS;
#else
	return LGMALLOC_PROF_RET_FAILURE;
#endif
}

void __lgmalloc_prof_enable(void)
{
#ifdef LGMALLOC_ENABLE_PROFILING
	prof_config_t *cfg = get_current_thread_config();
	GUARANTEE(cfg, "config must not be null");

	cfg->enabled = 1;
#else
	DISCARD_BRANCH;
#endif
}

void __lgmalloc_prof_disable(void)
{
#ifdef LGMALLOC_ENABLE_PROFILING
	prof_config_t *cfg = get_current_thread_config();
	GUARANTEE(cfg, "config must not be null");

	cfg->enabled = 0;
#else
	DISCARD_BRANCH;
#endif
}

int __lgmalloc_prof_enabled(void)
{
#ifdef LGMALLOC_ENABLE_PROFILING
	prof_config_t *cfg = get_current_thread_config();
	GUARANTEE(cfg, "config must not be null");

	return cfg->enabled;
#else
	return 0;
#endif
}

void __lgmalloc_prof_set_sample_rate(unsigned int rate)
{
#ifdef LGMALLOC_ENABLE_PROFILING
	prof_config_t *cfg = get_current_thread_config();
	GUARANTEE(cfg, "config must not be null");

	cfg->sample_rate = rate;
#else
	DISCARD_BRANCH;
#endif
}

void __lgmalloc_prof_set_stack_depth(unsigned int depth)
{
#ifdef LGMALLOC_ENABLE_PROFILING
	prof_config_t *cfg = get_current_thread_config();
	GUARANTEE(cfg, "config must not be null");

	cfg->stack_depth = depth;
#else
	DISCARD_BRANCH;
#endif
}

int __lgmalloc_prof_stats(lgmalloc_prof_stats_t *stats)
{
#ifdef LGMALLOC_ENABLE_PROFILING
	if (UNLIKELY(!stats))
		return LGMALLOC_PROF_RET_FAILURE;
#else
	return LGMALLOC_PROF_RET_FAILURE;
#endif
}

int __lgmalloc_prof_size_class_stats(
	lgmalloc_prof_stats_t **stats, size_t *count)
{
#ifdef LGMALLOC_ENABLE_PROFILING
	if (UNLIKELY(!stats))
		return LGMALLOC_PROF_RET_FAILURE;
#else
	return LGMALLOC_PROF_RET_FAILURE;
#endif
}

int __lgmalloc_prof_site_info(
	lgmalloc_call_site_id_t id,
	lgmalloc_prof_site_t *info)
{
#ifdef LGMALLOC_ENABLE_PROFILING
	if (UNLIKELY(!info))
		return LGMALLOC_PROF_RET_FAILURE;
#else
	return LGMALLOC_PROF_RET_FAILURE;
#endif
}

EXTERN_STRONG_ALIAS(__lgmalloc_prof_reset_stats, lgmalloc_prof_reset_stats);
EXTERN_STRONG_ALIAS(__lgmalloc_prof_enable, lgmalloc_prof_enable);
EXTERN_STRONG_ALIAS(__lgmalloc_prof_disable, lgmalloc_prof_disable);
EXTERN_STRONG_ALIAS(__lgmalloc_prof_enabled, lgmalloc_prof_enabled);
EXTERN_STRONG_ALIAS(__lgmalloc_prof_set_sample_rate, lgmalloc_prof_set_sample_rate);
EXTERN_STRONG_ALIAS(__lgmalloc_prof_set_stack_depth, lgmalloc_prof_set_stack_depth);
EXTERN_STRONG_ALIAS(__lgmalloc_prof_stats, lgmalloc_prof_stats);
EXTERN_STRONG_ALIAS(__lgmalloc_prof_size_class_stats, lgmalloc_prof_size_class_stats);
EXTERN_STRONG_ALIAS(__lgmalloc_prof_site_info, lgmalloc_prof_site_info);
