#include "Enesim.h"
#include "enesim_private.h"
#include "renderer.h"
#include "surface.h"
#include "scanline.h"
#include "alias.h"

/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
typedef struct _Fill_Color
{
	DATA32	color;
} Fill_Color;

static void _draw_alias(Enesim_Renderer *r, Scanline_Alias *sl, Enesim_Surface *dst)
{
	Fill_Color *f;
	Scanline_Alias_Sl *s;
	Span_Color_Func cfnc;
	int nsl;
	int offset;
	int i;

	f = r->data;
	nsl = sl->num_sls;
	s = sl->sls;

	cfnc = enesim_surface_span_color_get(dst, r->rop);
	for (i = 0; i < nsl; i++)
	{
		offset = (dst->w * s->y) + s->x;
		//printf("%d %d %d\n", s->y, s->x, s->w);
		cfnc(&dst->data, offset, f->color, s->w);
		s++;
	}
}

static void _draw(Enesim_Renderer *r, Enesim_Scanline *sl, Enesim_Surface *dst)
{
	_draw_alias(r, sl->data, dst);
}

static void _free(Enesim_Renderer *r)
{
	free(r->data);
}

static Enesim_Renderer_Func f_func = {
	.draw 	= _draw,
	.free 	= _free,
};
/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Renderer * enesim_fill_color_new(void)
{
	Fill_Color *f;
	Enesim_Renderer *r;

	f = calloc(1, sizeof(Fill_Color));
	
	r = enesim_renderer_new();
	r->data = f;
	r->funcs = &f_func;

	return r;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_fill_color_color_set(Enesim_Renderer *r, DATA32 color)
{
	Fill_Color *f;

	assert(r);
	f = r->data;
	f->color = color;
}
