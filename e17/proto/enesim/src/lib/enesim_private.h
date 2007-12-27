#ifndef _ENESIM_PRIVATE_H
#define _ENESIM_PRIVATE_H
 
/** 
 * @defgroup Enesim_Internal_Group Internal Implementation
 * @{
 */

#include <stdlib.h>
#include <limits.h>


#define DEBUG

#ifndef DEBUG
#define NDEBUG
#define ENESIM_WARNING(format, ...)
#else
#define ENESIM_WARNING(format, ...) \
fprintf(stderr, "[Enesim] (%s:%d - %s()):\n" format "\n\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#endif

#include <assert.h>

#ifdef MIN
#undef MIN
#endif
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/* FIXME move the above to other place */

/**
 * To be documented
 * FIXME: To be fixed
 */
static inline int enesim_hline_cut(int x, int *w, int *rx, int *rw, int cx)
{

	if ((x <= cx) && (x + *w > cx))	
	{
		int x2;

		x2 = x + *w;
		*w = cx - x;
		*rx = cx;
		*rw = x2 - cx;
		return 1;
	}
	return 0;
}

/** @} */
#endif

