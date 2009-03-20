/* ENESIM - Direct Rendering Library
 * Copyright (C) 2007-2008 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#include "Enesim.h"
#include "enesim_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Renderer_Repeater
{
	int mode;
	/* FIXME we dont support different area.w,h for mask and src */
	struct
	{
		Enesim_Surface_Data *data;
		Eina_Rectangle area;
		int w;
		int h;
	} mask, src;
	struct
	{
		Eina_Rectangle area;
		int w;
		int h;
	} dst;
} Renderer_Repeater;

/* TODO replace sw with the pitch */
static inline void _offset(Renderer_Repeater *f, int y, int *offset, int ax, int ah, int sw)
{
	/* Avoid the floating point exception in case the width is 0 */
	if (ah == 0)
		*offset = 0;
	else
		*offset = (((y - f->dst.area.y) % ah) * sw) + ax;
}

#define repeater_generic(setup, left_setup)					\
	Enesim_Drawer_Span spfnc;						\
	Eina_Rectangle slrect, dr;						\
	Enesim_Surface_Data sdata, mdata, ddata;				\
	Renderer_Repeater *f = r->data;						\
	int ssw, ssh; /* source surface width and height */			\
	int dsw, dsh; /* destination surface width and height */		\
	int paw, pah; /* pattern area width and height */			\
										\
	/*printf("0 %d %d %d, mode = %x\n", x, y, len, f->mode);*/		\
	/* check if the scanline intersects with the destination rectangle */	\
	if (!(f->mode & ENESIM_SURFACE_REPEAT_X))				\
	{									\
		paw = MIN(f->src.area.w, f->dst.area.w);			\
	}									\
	else									\
	{									\
		paw = f->dst.area.w;						\
	}									\
	if (!(f->mode & ENESIM_SURFACE_REPEAT_Y))				\
	{									\
		pah = MIN(f->src.area.w, f->dst.area.h);			\
	}									\
	else									\
	{									\
		pah = f->dst.area.h;						\
	}									\
	/* check that the span actually intersects the pattern area */		\
	eina_rectangle_coords_from(&slrect, x, y, len, 1);			\
	eina_rectangle_coords_from(&dr, f->dst.area.x, f->dst.area.y, paw, pah);\
	if (!eina_rectangle_intersection(&slrect, &dr))				\
	{									\
		return;								\
	}									\
	/* common setup */							\
	/* the source data should be at offset SRECT.x and sl->y already */	\
	sdata = *f->src.data;							\
	ddata = *dst;								\
	/* specific setup */							\
	setup;									\
	/* left */								\
	/*printf("1 %d %d %d %d\n", slrect.x, slrect.y, slrect.w, slrect.h);*/	\
	if (slrect.x > f->dst.area.x)						\
	{									\
		Enesim_Surface_Data stmp, mtmp;					\
		int llen;							\
		int diff = slrect.x - f->dst.area.x;				\
										\
		/* common setup */						\
		stmp = sdata;							\
		stmp.plane0 += diff;						\
		/* specific setup */						\
		left_setup;							\
		llen = MIN(slrect.w, diff);					\
		spfnc(&ddata, llen, &stmp, /* mul_color */0, &mtmp);		\
		ddata.plane0 += llen;						\
		/*printf("left %d\n", len);*/					\
		slrect.x += llen;						\
		slrect.w -= llen;						\
	}									\
	/* we dont have anything left to draw */				\
	if (!slrect.w)								\
		return;								\
										\
	/*printf("2 %d %d %d %d\n", slrect.x, slrect.y, slrect.w, slrect.h);*/	\
	/* middle */								\
	{									\
		int mlen = MIN(paw, f->src.area.w);				\
		int count = slrect.w / mlen;					\
		int total = count * mlen;					\
		int i;								\
		/*printf("middle = %d %d\n", count, len);*/			\
		for (i = 0; i < count; i++)					\
		{								\
			spfnc(&ddata, mlen, &sdata, /* mul_color */0, &mdata);	\
			ddata.plane0 += mlen;					\
		}								\
		slrect.w -= total;						\
		slrect.x += total;						\
	}									\
	/*printf("3 %d %d %d %d\n", slrect.x, slrect.y, slrect.w, slrect.h);*/	\
	/* right */								\
	if (slrect.w)								\
	{									\
		/*printf("right %d\n", slrect.w);*/				\
		spfnc(&ddata, slrect.w, &sdata, /* mul_color */0, &mdata);	\
	}

/* repeater span functions in the form repeater_DSTFORMAT_SRCFORMAT_MASKFORMAT */
static void repeater_argb88888_argb88888_argb88888(Enesim_Renderer *r, int x,
		int y, int len, Enesim_Surface_Data *dst)
{
	repeater_generic(
		{
			mdata = *f->mask.data;
			spfnc = enesim_drawer_span_pixel_mask_get(ENESIM_FILL, dst->format, f->src.data->format, f->mask.data->format);
		},
		{
			mtmp = mdata;
			mtmp.plane0 += diff;
		}
	);
}

static void repeater_argb88888_argb8888_none(Enesim_Renderer *r, int x, int y,
		int len, Enesim_Surface_Data *dst)
{
	repeater_generic(
		{
			spfnc = enesim_drawer_span_pixel_get(ENESIM_FILL, dst->format, f->src.data->format);
		},
		{}
	);
}

static Enesim_Renderer_Span _get(Enesim_Renderer *r, Enesim_Format *f)
{
	Renderer_Repeater *s;

	s = r->data;
	/* check that the mask and src areas are of the same size */
#if 0
	if (!s->src.data)
	{
		return NULL;
	}
	if (s->mask.data && ((s->mask.area.w != s->src.area.w) || (s->mask.area.h != s->src.area.h)))
	{
		return NULL;
	}
#endif
	if (s->mask.data)
		return repeater_argb88888_argb88888_argb88888;
	else
		return repeater_argb88888_argb8888_none;
}

static void _free(Enesim_Renderer *r)
{
	free(r->data);
}

static Enesim_Renderer_Func f_func = {
	.get = _get,
	.free = _free,
};
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Renderer * enesim_renderer_repeater_new(void)
{
	Renderer_Repeater *f;
	Enesim_Renderer *r;

	f = calloc(1, sizeof(Renderer_Repeater));

	r = enesim_renderer_new();
	r->data = f;
	r->funcs = &f_func;

	return r;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_repeater_mode_set(Enesim_Renderer *r, int mode)
{
	Renderer_Repeater *f;

	assert(r);
	f = r->data;
	f->mode = mode;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_repeater_dst_area_set(Enesim_Renderer *r, int x, int y, int w, int h)
{
	Renderer_Repeater *f;

	assert(r);
	f = r->data;
	assert(f);

	x = (x < 0) ? 0 : x;
	y = (y < 0) ? 0 : y;
	w = (w < 0) ? 0 : w;
	h = (h < 0) ? 0 : h;
	eina_rectangle_coords_from(&f->dst.area, x, y, w, h);
	/* TODO check the dst surface boundings and pick the min */
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_repeater_src_area_set(Enesim_Renderer *r, int x, int y, int w, int h)
{
	Renderer_Repeater *f;

	assert(r);
	f = r->data;
	assert(f);

	x = (x < 0) ? 0 : x;
	y = (y < 0) ? 0 : y;
	w = (w < 0) ? 0 : w;
	h = (h < 0) ? 0 : h;
	eina_rectangle_coords_from(&f->src.area, x, y, w, h);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_renderer_repeater_mask_area_set(Enesim_Renderer *r, int x, int y, int w, int h)
{
	Renderer_Repeater *f;

	assert(r);
	f = r->data;
	assert(f);

	x = (x < 0) ? 0 : x;
	y = (y < 0) ? 0 : y;
	w = (w < 0) ? 0 : w;
	h = (h < 0) ? 0 : h;
	eina_rectangle_coords_from(&f->mask.area, x, y, w, h);
}

EAPI void enesim_renderer_repeater_src_set(Enesim_Renderer *r, Enesim_Surface_Data *sdata)
{
	Renderer_Repeater *f;

	f = r->data;
	f->src.data = sdata;
}

EAPI void enesim_renderer_repeater_src_size_set(Enesim_Renderer *r, int sw, int sh)
{
	Renderer_Repeater *f;

	f = r->data;
	f->src.w = sw;
	f->src.h = sh;
}

EAPI void enesim_renderer_repeater_mask_size_set(Enesim_Renderer *r, int sw, int sh)
{
	Renderer_Repeater *f;

	f = r->data;
	f->mask.w = sw;
	f->mask.h = sh;
}

EAPI void enesim_renderer_repeater_src_unset(Enesim_Renderer *r)
{
	Renderer_Repeater *f;

	f = r->data;
	f->src.data = NULL;
}


EAPI void enesim_renderer_repeater_mask_set(Enesim_Renderer *r, Enesim_Surface_Data *mdata)
{
	Renderer_Repeater *f;

	f = r->data;
	f->mask.data = mdata;
}

EAPI void enesim_renderer_repeater_mask_unset(Enesim_Renderer *r)
{
	Renderer_Repeater *f;

	f = r->data;
	f->mask.data = NULL;
}

EAPI void enesim_renderer_repeater_src_offset(Enesim_Renderer *r, int y, int *offset)
{
	Renderer_Repeater *f;

	f = r->data;
	_offset(f, y, offset, f->src.area.x, f->src.area.h, f->src.w);
}

EAPI void enesim_renderer_repeater_mask_offset(Enesim_Renderer *r, int y, int *offset)
{
	Renderer_Repeater *f;

	f = r->data;
	_offset(f, y, offset, f->mask.area.x, f->mask.area.h, f->mask.w);
}

EAPI Eina_Bool enesim_renderer_repeater_src_y(Enesim_Renderer *r, int ydst, int *ysrc)
{
	Renderer_Repeater *f;

	f = r->data;
	if ((ydst < f->dst.area.y) || (ydst > (f->dst.area.y + f->dst.area.h)))
		return EINA_FALSE;
	if (f->mode & ENESIM_SURFACE_REPEAT_Y)
	{
		*ysrc = (ydst - f->dst.area.y) % f->src.area.h;
		return EINA_TRUE;
	}
	else
	{
		if (ydst > (f->dst.area.y + f->src.area.h))
			return EINA_FALSE;
		else
		{
			*ysrc = f->src.area.y + (ydst - f->dst.area.y);
			return EINA_TRUE;
		}
	}
}
