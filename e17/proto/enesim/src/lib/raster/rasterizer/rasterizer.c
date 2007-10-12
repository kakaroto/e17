#include <stdlib.h>

#include "Enesim.h"
#include "enesim_private.h"
#include "rasterizer.h"
#include "scanline.h"

/**
 * TODO
 * add a function/struct element to set up window boundaries i.e 
 * destination surface/clip
 * add a function/struct element to set up the odd/even/zero fill rule
 */

/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Rasterizer * enesim_rasterizer_new(void)
{
	Enesim_Rasterizer *r;

	r = calloc(1, sizeof(Enesim_Rasterizer));
	r->funcs = &cpsc;
	r->data = cpsc.create();
	return r;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_rasterizer_vertex_add(Enesim_Rasterizer *r, float x, float y)
{
	r->funcs->vertex_add(r->data, x, y);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_rasterizer_generate(Enesim_Rasterizer *r, Enesim_Scanline *s)
{
	r->funcs->generate(r->data, s);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_rasterizer_boundings_set(Enesim_Rasterizer *r, int x, int y, int w, int h)
{
	r->bound.x = x;
	r->bound.y = y;
	r->bound.w = w;
	r->bound.h = h;
}
