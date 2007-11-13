#include <stdlib.h>

#include "Enesim.h"
#include "enesim_private.h"
#include "rasterizer.h"
#include "scanline.h"

/*============================================================================*
 *                                 Global                                     * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
Enesim_Rasterizer * enesim_rasterizer_new(void *data, Enesim_Rasterizer_Func *funcs,
		Enesim_Rectangle boundaries)
{
	Enesim_Rasterizer *r;

	r = calloc(1, sizeof(Enesim_Rasterizer));
	r->funcs = funcs;
	r->data = data;
	r->boundaries = boundaries;
	return r;
}
/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_rasterizer_vertex_add(Enesim_Rasterizer *r, float x, float y)
{
	assert(r);
	assert(r->funcs);
	assert(r->funcs->vertex_add);
	
	r->funcs->vertex_add(r->data, x, y);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_rasterizer_generate(Enesim_Rasterizer *r, Enesim_Scanline *s)
{
	assert(r);
	assert(r->funcs);
	assert(r->funcs->generate);
	
	r->funcs->generate(r->data, s);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_rasterizer_delete(Enesim_Rasterizer *r)
{
	assert(r);
	assert(r->funcs);
	assert(r->funcs->delete);
	
	r->funcs->delete(r->data);
	free(r);
}