#ifndef _ENESIM_PRIVATE_H
#define _ENESIM_PRIVATE_H
 
/** 
 * @defgroup Enesim_Internal_Group Internal Implementation
 * @{
 */

#include <assert.h>
#include "Edata.h"

/* FIXME move the above to other place */
typedef struct _Enesim_Rectangle
{
	int 	x;
	int 	y;
	int 	w;
	int 	h;
} Enesim_Rectangle;

static inline int enesim_rectangle_is_empty(Enesim_Rectangle *r)
{
	return ((r->w < 1) || (r->h < 1));
}

/** @} */
#endif

