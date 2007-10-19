#include <stdlib.h>

#include "Enesim.h"
#include "enesim_private.h"
#include "renderer.h"
#include "surface.h"
#include "scanline.h"
#include "alias.h"

/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
typedef struct _Fill_Surface
{
	Enesim_Surface *s;
	int mode;
	Enesim_Rectangle sarea;
	Enesim_Rectangle darea;
} Fill_Surface;


static inline void _draw_alias_sl(Fill_Surface *f, Scanline_Alias_Sl *sl, Enesim_Surface *dst)
{
	int offset;

#define DRECT f->darea
#define SRECT f->sarea
	/*
	 * +---+---+  +---+---+
	 * | S | S |  | S |   |
	 * +---+---+  +---+   |
	 * |       |  |       |
	 * +-------+  +-------+
	 */
	if (!(f->mode & ENESIM_SURFACE_REPEAT_Y))
	{
		/* scanline outside vertical area */
		if (sl->y > DRECT.y + SRECT.h - 1)
		{
			// FIXME fill base color ?
			return;		
		}
		/* scanline inside vertical area */
		offset = ((sl->y - DRECT.y) + SRECT.y) * f->s->w + SRECT.x;
	}
	/*
	 * +---+---+  +---+---+
	 * | S |   |  | S | S |
	 * +---+   +  +---+---+
	 * | S |   |  | S | S |
	 * +-------+  +-------+
	 */
	else
	{
		/* scanline inside vertical area */
		offset = (((sl->y - DRECT.y) % SRECT.h)  + SRECT.y) * f->s->w + SRECT.x;
	}
	/*
	 * +---+---+  +---+---+
	 * | S |   |  | S |   |
	 * +---+   +  +---+   |
	 * | S |   |  |       |
	 * +-------+  +-------+
	 */
	/* simple cases are done, now the complex ones */
	if (!(f->mode & ENESIM_SURFACE_REPEAT_X))
	{
		/* scanline inside horizontal area */
		/* scanline outside horizontal area */
	}
	/*
	 * +---+---+  +---+---+
	 * | S | S |  | S | S |
	 * +---+---+  +---+---|
	 * |       |  | S | S |
	 * +-------+  +-------+
	 */
	else
	{
		/* check if the scanline is inside the dst rect
		 * +---+          +---+      +---+     +---+
		 * | Ds|---   s---|-D |    s-|-D-|-    |sD |
		 * +---+          +----      +---+     +---+
		 */
	}
}

static inline void _draw_alias(Enesim_Renderer *r, Scanline_Alias *sl, Enesim_Surface *dst)
{
	Fill_Surface *f;
	Scanline_Alias_Sl *s;
	int nsl;	
	int i;

	f = r->data;
	nsl = sl->num_sls;
	s = sl->sls;
	
	for (i = 0; i < nsl; i++)
	{
		_draw_alias_sl(f, s, dst);
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
EAPI Enesim_Renderer * enesim_fill_surface_new(void)
{
	Fill_Surface *f;
	Enesim_Renderer *r;

	f = calloc(1, sizeof(Fill_Surface));
	
	r = enesim_renderer_new();
	r->data = f;
	r->funcs = &f_func;

	return r;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_fill_surface_surface_set(Enesim_Renderer *r, Enesim_Surface *s)
{
	Fill_Surface *f;

	assert(r);
	assert(s);
	f = r->data;
	f->s = s;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_fill_surface_mode_set(Enesim_Renderer *r, int mode)
{
	Fill_Surface *f;
	
	assert(r);
	f = r->data;
	f->mode = mode;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_fill_surface_dst_area_set(Enesim_Renderer *r, int x, int y, int w, int h)
{
	Fill_Surface *f;	
	assert(r);
	
	x = (x < 0) ? 0 : x;
	y = (y < 0) ? 0 : y;
	w = (w < 0) ? 0 : w;
	h = (h < 0) ? 0 : h;
	enesim_rectangle_from_coords(&f->darea, x, y, w, h);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_fill_surface_src_area_set(Enesim_Renderer *r, int x, int y, int w, int h)
{
	Fill_Surface *f;
	assert(r);
	
	x = (x < 0) ? 0 : x;
	y = (y < 0) ? 0 : y;
	w = (w < 0) ? 0 : w;
	h = (h < 0) ? 0 : h;
	enesim_rectangle_from_coords(&f->sarea, x, y, w, h);
}
