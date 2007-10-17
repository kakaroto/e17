#include "Enesim.h"
#include "enesim_private.h"
#include "surface.h"

#include <stdlib.h>
#include <stdarg.h>
/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
static Surface_Backend *_backends[ENESIM_SURFACE_FORMATS] = {
		&argb8888_backend,
		&rgb565_backend,
};
#if 0
static Enesim_Surface_Func *_funcs[ENESIM_SURFACE_FORMATS] = {
	argb8888_funcs,
	rgb565_funcs,
};
static Conv_Func *_conv_funcs[ENESIM_SURFACE_FORMATS] = {
	argb8888_conv_funcs,
	rgb565_conv_funcs,
};
#endif
/*============================================================================*
 *                                 Global                                     * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
Span_Color_Func enesim_surface_span_color_get(Enesim_Surface *s, int rop)
{
#if 0
	if ((rop == ENESIM_RENDERER_BLEND) && (s->flags & ENESIM_SURFACE_DIRTY))
	{

	}
#endif
	return _backends[s->format]->rops[ENESIM_RENDERER_BLEND].sp_color;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
void enesim_surface_premul(Enesim_Surface *s)
{
}
/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Surface *
enesim_surface_new(Enesim_Surface_Format f, int w, int h, Enesim_Surface_Flag flags, ...)
{
	Enesim_Surface *s;
	va_list va;

	s = calloc(1, sizeof(Enesim_Surface));
	s->w = w;
	s->h = h;
	s->format = f;
	s->flags = flags;

	va_start(va, flags);
	switch (s->format)
	{
		case ENESIM_SURFACE_ARGB8888:
		s->data.argb8888.data = va_arg(va, DATA32 *);
		break;

		case ENESIM_SURFACE_RGB565:
		s->data.rgb565.data = va_arg(va, DATA16 *);
		s->data.rgb565.alpha = va_arg(va, DATA8 *);
		break;

		default:
		break;
	}
	va_end(va);
	if (!(s->flags & ENESIM_SURFACE_PREMUL))
		enesim_surface_premul(s);
	return s;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_size_get(Enesim_Surface *s, int *w, int *h)
{
	assert(s);
	if (w) *w = s->w;
	if (h) *h = s->h;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_surface_size_set(Enesim_Surface *s, int w, int h)
{
	assert(s);
	s->w = w;
	s->h = h;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_data_get(Enesim_Surface *s, ...)
{
	DATA32 	**d32;
	DATA16 	**d16;
	DATA8 	**d8;
	va_list va;
	
	assert(s);
	va_start(va, s);
	switch (s->format)
	{
		case ENESIM_SURFACE_ARGB8888:
		d32 = va_arg(va, DATA32 **);
		*d32 = s->data.argb8888.data;
		break;

		case ENESIM_SURFACE_RGB565:
		d16 = va_arg(va, DATA16 **);
		*d16 = s->data.rgb565.data;
		d8 = va_arg(va, DATA8 **);
		*d8 = s->data.rgb565.alpha;
		break;

		default:
		break;
	}
	va_end(va);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Surface_Format
enesim_surface_format_get(Enesim_Surface *s)
{
	assert(s);
	return s->format;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI int
enesim_surface_flag_get(Enesim_Surface *s)
{
	assert(s);
	return s->flags;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void 
enesim_surface_flag_set(Enesim_Surface *s, Enesim_Surface_Flag flags)
{
	assert(s);
	s->flags = flags;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void 
enesim_surface_data_set(Enesim_Surface *s, Enesim_Surface_Format f, ...)
{
	va_list va;
	
	assert(s);
	/* TODO check if we already had data */
	va_start(va, f);
	switch (s->format)
	{
		case ENESIM_SURFACE_ARGB8888:
		s->data.argb8888.data = va_arg(va, DATA32 *);
		break;

		case ENESIM_SURFACE_RGB565:
		s->data.rgb565.data = va_arg(va, DATA16 *);
		s->data.rgb565.alpha = va_arg(va, DATA8 *);
		break;

		default:
		break;
	}
	va_end(va);
	if (!(s->flags & ENESIM_SURFACE_PREMUL))
			enesim_surface_premul(s);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void 
enesim_surface_resize(Enesim_Surface *s, Enesim_Rectangle *srect, Enesim_Surface *d, Enesim_Rectangle *drect, int mode)
{

}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void 
enesim_surface_unpremul(Enesim_Surface *s)
{
	DATA32 *d, *e;
	int len;
	
	assert(s);
	/* FIXME this is only for 32bpp */
	len = s->w * s->h;
#define ARGB_JOIN(a,r,g,b) (((a) << 24) + ((r) << 16) + ((g) << 8) + (b))
#define A_VAL(p) ((DATA8 *)(p))[3]
#define R_VAL(p) ((DATA8 *)(p))[2]
#define G_VAL(p) ((DATA8 *)(p))[1]
#define B_VAL(p) ((DATA8 *)(p))[0]
	d = s->data.argb8888.data;
	e = d + len;
	while (d < e)
	{
		DATA32 a = (*d >> 24);

		if ((a > 0) && (a < 255))
			*d = ARGB_JOIN(a, (R_VAL(d) * 255) / a, (G_VAL(d) * 255)
					/ a, (B_VAL(d) * 255) / a);
		d++;
	}
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void 
enesim_surface_convert(Enesim_Surface *s, Enesim_Surface *d)
{
	assert(s);
	assert(d);
	if (s->format == d->format) return;
	/* TODO call the correct convert function based on the src
	 * and dst format, the src and dst flags, etc
	 */
}
