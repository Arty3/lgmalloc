/* ******************************************** */
/*                                              */
/*   lgmalloc_profiling.h                       */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

#ifndef __LGMALLOC_PROFILING_H
#define __LGMALLOC_PROFILING_H

#include <stddef.h>
#include <stdint.h>

#define LGMALLOC_PROF_RET_FAILURE	0
#define LGMALLOC_PROF_RET_SUCCESS	1

/* All data is thread local */

typedef enum __lgmalloc_prof_site_freq_t
{
	LGMALLOC_PROF_SITE_VERY_COLD,
	LGMALLOC_PROF_SITE_COLD,
	LGMALLOC_PROF_SITE_MODERATE,
	LGMALLOC_PROF_SITE_HOT,
	LGMALLOC_PROF_SITE_VERY_HOT
}	lgmalloc_prof_site_freq_t;

typedef unsigned long					lgmalloc_prof_site_rank_t;
typedef unsigned long					lgmalloc_call_site_id_t;

typedef struct __lgmalloc_prof_stats_t	lgmalloc_prof_stats_t;
typedef struct __lgmalloc_prof_site_t	lgmalloc_prof_site_t;

int lgmalloc_prof_reset_stats(void);

void lgmalloc_prof_enable(void);
void lgmalloc_prof_disable(void);
int  lgmalloc_prof_enabled(void);

void lgmalloc_prof_set_sample_rate(unsigned int rate);
void lgmalloc_prof_set_stack_depth(unsigned int depth);

int lgmalloc_prof_stats(lgmalloc_prof_stats_t *stats);
int lgmalloc_prof_size_class_stats(lgmalloc_prof_stats_t **stats, size_t *count);
int lgmalloc_prof_site_info(lgmalloc_call_site_id_t id, lgmalloc_prof_site_t *info);

#endif /* __LGMALLOC_PROFILING_H */
