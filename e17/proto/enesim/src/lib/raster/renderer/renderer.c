#include "Enesim.h"
#include "enesim_private.h"
#include "renderer.h"
/*============================================================================*
 *                                 Global                                     * 
 *============================================================================*/
Enesim_Renderer * enesim_renderer_new(void)
{
	Enesim_Renderer *r;

	r = calloc(1, sizeof(Enesim_Renderer));
	return r;
}
/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
EAPI void enesim_renderer_rop_set(Enesim_Renderer *r, int rop)
{
	assert(r);
	r->rop = rop;
}

EAPI void enesim_renderer_delete(Enesim_Renderer *r)
{
	assert(r);
	r->funcs->free(r);
	free(r);
}

EAPI void enesim_renderer_draw(Enesim_Renderer *r, Enesim_Scanline *sl, Enesim_Surface *dst)
{
	assert(r);
	assert(sl);
	assert(dst);
	r->funcs->draw(r, sl, dst);
}
