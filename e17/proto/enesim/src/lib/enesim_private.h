#ifndef _ENESIM_PRIVATE_H
#define _ENESIM_PRIVATE_H
 
/** 
 * @defgroup Enesim_Internal_Group Internal Implementation
 * @{
 */

#include <stdlib.h>
#include <limits.h>
#include "Edata.h"

#ifndef DEBUG
#define NDEBUG
#define ENESIM_WARNING(format, ...)
#else
#define
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
static inline int enesim_rectangle_is_empty(Enesim_Rectangle *r)
{
	return ((r->w < 1) || (r->h < 1));
}
/**
 * To be documented
 * FIXME: To be fixed
 */
static inline int enesim_spans_common(int c1, int l1, int c2, int l2)
{
	return (!(((c2 + l2) <= c1) || (c2 >= (c1 + l1))));
}
/**
 * To be documented
 * FIXME: To be fixed
 */
static inline int enesim_rects_intersect(int x, int y, int w, int h, int xx,
		int yy, int ww, int hh)
{
	return (enesim_spans_common(x, w, xx, ww) && enesim_spans_common(y, h, yy, hh));
}
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

