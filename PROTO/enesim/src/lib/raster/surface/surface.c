#include "Enesim.h"
#include "enesim_private.h"
#include "surface.h"
#include "fixed_16p16.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

// for now
#include "argb8888.h"
/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
static Surface_Backend *_backends[ENESIM_SURFACE_FORMATS] = {
		&argb8888_backend,
		&rgb565_backend,
};

static Enesim_Surface_Transformation_Type _transformation_get(float *t)
{
	if ((t[MATRIX_ZX] != 0) || (t[MATRIX_ZY] != 0) || (t[MATRIX_ZZ] != 1))
	            return ENESIM_SURFACE_TRANSFORMATION_PROJECTIVE;
	else
	{
		if ((t[MATRIX_XX] == 1) && (t[MATRIX_XY] == 0) && (t[MATRIX_XZ] == 0) &&
				(t[MATRIX_YX] == 0) && (t[MATRIX_YY] == 1) && (t[MATRIX_YZ] == 0))
			return ENESIM_SURFACE_TRANSFORMATION_IDENTITY;
		else
			return ENESIM_SURFACE_TRANSFORMATION_AFFINE;
	}
}

/* convert the transformation values to fixed point */
static void _transformation_to_fixed(float *t, enesim_16p16_t *td)
{
	td[0] = enesim_16p16_float_from(t[0]);
	td[1] = enesim_16p16_float_from(t[1]);
	td[2] = enesim_16p16_float_from(t[2]);
	td[3] = enesim_16p16_float_from(t[3]);
	td[4] = enesim_16p16_float_from(t[4]);
	td[5] = enesim_16p16_float_from(t[5]);
	td[6] = enesim_16p16_float_from(t[6]);
	td[7] = enesim_16p16_float_from(t[7]);
	td[8] = enesim_16p16_float_from(t[8]);
}
#if 0


#endif
/*============================================================================*
 *                                 Global                                     * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
Span_Color_Func enesim_surface_span_color_func_get(Enesim_Surface *s, int rop)
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
Span_Color_Mask_Func enesim_surface_span_color_mask_func_get(Enesim_Surface *s,
		int rop)
{
	return _backends[s->format]->rops[ENESIM_RENDERER_BLEND].sp_color_mask;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
Span_Pixel_Func enesim_surface_pixel_func_get(Enesim_Surface *s, Enesim_Surface *src,
		int rop)
{
	assert(s->format == src->format);
	return _backends[s->format]->rops[ENESIM_RENDERER_BLEND].sp_pixel;
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
	switch (s->format) {
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
		_backends[s->format]->premul(&s->data, s->w * s->h);
	/* setup the identity matrix */
	s->transformation.matrix[MATRIX_XX] = 1;
	s->transformation.matrix[MATRIX_YY] = 1;
	s->transformation.matrix[MATRIX_ZZ] = 1;
	s->transformation.type = ENESIM_SURFACE_TRANSFORMATION_IDENTITY;
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
	switch (s->format) {
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
	switch (s->format) {
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
		_backends[s->format]->premul(&s->data, s->w * s->h);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_transformation_set(Enesim_Surface *s, float *t)
{
	int i;
	
	assert(s);
	assert(t);
	
	for (i = 0; i < MATRIX_SIZE; i++)
	{
		s->transformation.matrix[i] = t[i];
		printf("%f\n", t[i]);
	}
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void
enesim_surface_draw(Enesim_Surface *s, Enesim_Rectangle *sr, Enesim_Surface *d, Enesim_Rectangle *dr, int mode, int smooth)
{
	Enesim_Rectangle csr, cdr;
	enesim_16p16_t ft[MATRIX_SIZE];
		
	assert(s);
	assert(d);
	
	/* setup the destination clipping */
	cdr.x = 0;
	cdr.y = 0;
	cdr.w = d->w;
	cdr.h = d->h;
	if (sr)
	{
		enesim_rectangle_rectangle_intersection_get(&cdr, dr);
		if (enesim_rectangle_is_empty(&cdr))
			return;
	}
	/* setup the source clipping */
	csr.x = 0;
	csr.y = 0;
	csr.w = s->w;
	csr.h = s->h;
	if (dr)
	{
		enesim_rectangle_rectangle_intersection_get(&csr, sr);
		if (enesim_rectangle_is_empty(&csr))
			return;
	}
	/* if we need to do some calcs with the matrix, transform it */
	// if (!(s->transformation.type & ENESIM_SURFACE_TRANSFORMATION_IDENTITY))
	_transformation_to_fixed(s->transformation.matrix, ft);
	/* check if we are going to scale */
	/* scaling */
	if ((cdr.w != csr.w) && (cdr.h != csr.h))
	{
		/* smooth scaling */
		if ((cdr.w > csr.w) && (cdr.h <= csr.h))
		{
			/* x upscaling, y downscaling */
		}
		else if ((cdr.w <= csr.w) && (cdr.h > csr.h))
		{
			/* x downscaling, y upscaling */
		}
		else if ((cdr.w > csr.w) && (cdr.h > csr.h))
		{
			/* x upscaling, y upscaling */
		}
	}
	/* not scaling */
	else
	{
		/* when using the multiplied variant check that the mul color is different
		 * than 255,255,255,255, if not use the version without mul
		 */
		//_backends[s->format].draw[s->transformation.type][ENESIM_SURFACE_NO_SCALE];
		/* check transformation type */
		//argb8888_transformed_blend(s, &csr, d, &cdr, ft);
		//argb8888_identity_op(s, d);
		//argb8888_c_draw_fill_affine_no_no(s, &csr, d, &cdr, ft);
		argb8888_c_draw_blend_mul_affine_no_no(s, &csr, d, &cdr, ft, 0xcccccccc);
	}
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void 
enesim_surface_unpremul(Enesim_Surface *s, ...)
{	
	Enesim_Surface_Data data;
	va_list va;
	
	assert(s);
	/* TODO check if we already had data */
	va_start(va, s);
	switch (s->format) {
	case ENESIM_SURFACE_ARGB8888:
		data.argb8888.data = va_arg(va, DATA32 *);
		break;

	case ENESIM_SURFACE_RGB565:
		data.rgb565.data = va_arg(va, DATA16 *);
		data.rgb565.alpha = va_arg(va, DATA8 *);
		break;

	default:
		break;
	}
	_backends[s->format]->unpremul(&s->data, &data, s->w * s->h);
	va_end(va);
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

EAPI void 
enesim_surface_delete(Enesim_Surface *s)
{
	assert(s);
	free(s);
}
