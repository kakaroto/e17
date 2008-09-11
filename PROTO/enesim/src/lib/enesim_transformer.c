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
#include "enesim_common.h"
#include "Enesim.h"
#include "enesim_private.h"
/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
/* TODO
 * decide if this should be on global or API
 */
static Enesim_Transformation_Type _transformation_get(Enesim_Matrix *m)
{
	if ((MATRIX_ZX(m) != 0) || (MATRIX_ZY(m) != 0) || (MATRIX_ZZ(m) != 1))
	            return ENESIM_TRANSFORMATION_PROJECTIVE;
	else
	{
		/* FIXME, once the identity handles the origin use the AFFINE;
		if ((MATRIX_XX(m) == 1) && (MATRIX_XY(m) == 0) && (MATRIX_XZ(m) == 0) &&
				(MATRIX_YX(m) == 0) && (MATRIX_YY(m) == 1) && (MATRIX_YZ(m) == 0))
			
			return ENESIM_TRANSFORMATION_IDENTITY;
		else
		*/
			return ENESIM_TRANSFORMATION_AFFINE;
	}
}

static void _transformation_debug(Enesim_Transformation *t)
{
#if 0
	printf("Transformation with rop = %d\n", t->rop);
	printf("Floating point matrix\n");
	_matrix_debug(t->matrix);
	printf("Fixed point matrix (16p16 format)\n");
	printf("XX = %u XY = %u XZ = %u\n", t->matrix_fixed[MATRIX_XX], t->matrix_fixed[MATRIX_XY], t->matrix_fixed[MATRIX_XZ]);
	printf("YX = %u YY = %u YZ = %u\n", t->matrix_fixed[MATRIX_YX], t->matrix_fixed[MATRIX_YY], t->matrix_fixed[MATRIX_YZ]);
	printf("ZX = %u ZY = %u ZZ = %u\n", t->matrix_fixed[MATRIX_ZX], t->matrix_fixed[MATRIX_ZY], t->matrix_fixed[MATRIX_ZZ]);
#endif
}

/* TODO
 * remove this
 */
#define INTERP_256(a, c0, c1) \
 ( (((((((c0) >> 8) & 0xff00ff) - (((c1) >> 8) & 0xff00ff)) * (a)) \
   + ((c1) & 0xff00ff00)) & 0xff00ff00) + \
   (((((((c0) & 0xff00ff) - ((c1) & 0xff00ff)) * (a)) >> 8) \
   + ((c1) & 0xff00ff)) & 0xff00ff) )
#define interp_256 INTERP_256

/*
 * TODO
 * this function is very expensive because we have convert every time to argb
 * in case of argb8888_pre means several muls and divs
 */
static unsigned int convolution2x2(Enesim_Surface_Data *data, 
		Enesim_Surface_Format fmt, Eina_F16p16 x, Eina_F16p16 y,
		unsigned w, unsigned int h)
{
	Enesim_Surface_Data tmp;
	unsigned int p3 = 0, p2 = 0, p1 = 0, p0 = 0;
	int ax, ay;
	int sx, sy;

	/* 8 bits error to alpha */
	ax = 1 + (eina_f16p16_fracc_get(x) >> 8);
	ay = 1 + (eina_f16p16_fracc_get(y) >> 8);
	/* integer values for the coordinates */
	sx = eina_f16p16_int_to(x);
	sy = eina_f16p16_int_to(y);

	if ((sx > -1) && (sy > -1))
	{
		p0 = enesim_surface_data_to_argb(data, fmt);
	}
	if ((sy > -1) && ((sx + 1) < w))
	{
		tmp = *data;
		
		enesim_surface_data_increment(&tmp, fmt, 1);
		p1 = enesim_surface_data_to_argb(&tmp, fmt);
	}
	if ((sy + 1) < h)
	{
		if (sx > -1)
		{
			tmp = *data;
			enesim_surface_data_increment(&tmp, fmt, w);
			p2 = enesim_surface_data_to_argb(&tmp, fmt);
		}
		if ((sx + 1) < w)
		{
			tmp = *data;
			enesim_surface_data_increment(&tmp, fmt, w + 1);
			p3 = enesim_surface_data_to_argb(&tmp, fmt);
		}
	}
	if (p0 != p1)
		p0 = interp_256(ax, p1, p0);
	if (p2 != p3)
		p2 = interp_256(ax, p3, p2);
	if (p0 != p2)
		p0 = interp_256(ay, p2, p0);
	return p0;
}
/* TODO
 * handle the origin
 */
static void transformer_identity_no_no(Enesim_Transformation *t, Enesim_Surface *ss,
		Eina_Rectangle *srect, Enesim_Surface *ds, Eina_Rectangle *drect)
{
	Enesim_Surface_Data sdata, ddata;
	Enesim_Drawer_Span spfnc;
	int h;
	
	/* TODO, pixel or pixel_color */
	spfnc = enesim_drawer_span_pixel_get(t->rop, ds->format, ss->format);
	if (!spfnc)
		return;
	
	enesim_surface_data_get(ss, &sdata);
	enesim_surface_data_get(ds, &ddata);
	enesim_surface_data_increment(&sdata, ss->format, (srect->y * ss->w) + srect->x);
	enesim_surface_data_increment(&ddata, ds->format, (drect->y * ds->w) + drect->x);
	h = drect->h;
	
	while (h--)
	{
		spfnc(&ddata, drect->w, &sdata, /* mul_color */0, NULL);
		enesim_surface_data_increment(&sdata, ss->format, ss->w);
		enesim_surface_data_increment(&ddata, ds->format, ds->w);
	}	
}

static void transformer_affine_no_no(Enesim_Transformation *t, Enesim_Surface *ss,
		Eina_Rectangle *srect, Enesim_Surface *ds, Eina_Rectangle *drect)
{
	Enesim_Surface_Data sdata, ddata;
	Eina_F16p16 sx, sy;
	Enesim_Drawer_Point ptfnc;
	Eina_F16p16 a, b, c, d, e, f, g, h, i;

	int hlen;

	/* force an alpha color, return if we dont have a point function */
	ptfnc = enesim_drawer_point_color_get(t->rop, ds->format, 0x55555555);
	if (!ptfnc)
		return;
	
	enesim_matrix_fixed_values_get(t->matrix, &a, &b, &c, &d, &e, &f, &g, &h, &i);
	c += eina_f16p16_float_from(t->ox);
	f += eina_f16p16_float_from(t->oy);
	sx = eina_f16p16_mul(a, drect->x) + eina_f16p16_mul(b, drect->y) + c;
	sy = eina_f16p16_mul(d, drect->x) + eina_f16p16_mul(e, drect->y) + f;
	
	enesim_surface_data_get(ds, &ddata);
	enesim_surface_data_get(ss, &sdata);
	enesim_surface_data_increment(&ddata, ds->format, (drect->y * ds->w) + drect->x);

	hlen = drect->h;
	while (hlen--)
	{
		Enesim_Surface_Data ddata2;
		int wlen;
		Eina_F16p16 sxx, syy;
		
		ddata2 = ddata;
		sxx = sx;
		syy = sy;

		wlen = drect->w;
		while (wlen--)
		{
			int six, siy;
			Enesim_Surface_Data sdata2;
			
			sdata2 = sdata;
			six = eina_f16p16_int_to(sxx);
			siy = eina_f16p16_int_to(syy);
			
			/* check that we are inside the source rectangle */
			if ((eina_rectangle_xcoord_inside(srect, six)) && (eina_rectangle_ycoord_inside(srect, siy)))
			{
				unsigned int argb;
				
				/* use 2x2 convolution kernel to interpolate */
				enesim_surface_data_increment(&sdata2, ss->format, (siy * ss->w) + six);
				argb = convolution2x2(&sdata2, ss->format, sxx, syy, ss->w, ss->h);
				ptfnc(&ddata2, NULL, argb, NULL);
			}
			enesim_surface_data_increment(&ddata2, ds->format, 1);
			sxx += a;
			syy += d;
		}
		enesim_surface_data_increment(&ddata, ds->format, ds->w);
		sx += b;
		sy += e;
	}
}

static void transformer_projective_no_no(Enesim_Transformation *t, Enesim_Surface *ss,
		Eina_Rectangle *srect, Enesim_Surface *ds, Eina_Rectangle *drect)
{
	Enesim_Surface_Data sdata, ddata;
	Eina_F16p16 sx, sy, sz;
	Enesim_Drawer_Point ptfnc;
	Eina_F16p16 a, b, c, d, e, f, g, h, i, ox, oy;

	int hlen;

	/* force an alpha color, return if we dont have a point function */
	ptfnc = enesim_drawer_point_color_get(t->rop, ds->format, 0x55555555);
	if (!ptfnc)
		return;
	
	enesim_matrix_fixed_values_get(t->matrix, &a, &b, &c, &d, &e, &f, &g, &h, &i);
	sx = eina_f16p16_mul(a, drect->x) + eina_f16p16_mul(b, drect->y) + c;
	sy = eina_f16p16_mul(d, drect->x) + eina_f16p16_mul(e, drect->y) + f;
	sz = eina_f16p16_mul(g, drect->x) + eina_f16p16_mul(h, drect->y) + i;
	ox = eina_f16p16_float_from(t->ox);
	oy = eina_f16p16_float_from(t->oy);
	
	enesim_surface_data_get(ss, &sdata);
	enesim_surface_data_get(ds, &ddata);
	enesim_surface_data_increment(&sdata, ss->format, (srect->y * ss->w) + srect->x);
	enesim_surface_data_increment(&ddata, ds->format, (drect->y * ds->w) + drect->x);
	
	hlen = drect->h;
	while (hlen--)
	{
		Enesim_Surface_Data ddata2;
		Eina_F16p16 sxx, syy, szz;
		int wlen;
			
		ddata2 = ddata;
		sxx = sx;
		syy = sy;
		szz = sz;

		wlen = drect->w;
		while (wlen--)
		{
			int six, siy;
			Enesim_Surface_Data sdata2;
			Eina_F16p16 sxxx, syyy;
						
			sdata2 = sdata;
			/* TODO set the destination to zero? */
			if (!szz)
				goto iterate;
				
			sxxx = ((((long long int)sxx) << 16) / szz) + ox;
			syyy = ((((long long int)syy) << 16) / szz) + oy;
			six = eina_f16p16_int_to(sxxx);
			siy = eina_f16p16_int_to(syyy);
			/* check that we are inside the source rectangle */
			if ((eina_rectangle_xcoord_inside(srect, six)) && (eina_rectangle_ycoord_inside(srect, siy)))
			{
				unsigned int argb;
							
				/* use 2x2 convolution kernel to interpolate */
				enesim_surface_data_increment(&sdata2, ss->format, (siy * ss->w) + six);
				argb = convolution2x2(&sdata2, ss->format, sxxx, syyy, ss->w, ss->h);
				ptfnc(&ddata2, NULL, argb, NULL);
			}
iterate:
			enesim_surface_data_increment(&ddata2, ds->format, 1);
			sxx += a;
			syy += d;
			szz += g;
		}
		enesim_surface_data_increment(&ddata, ds->format, ds->w);
		sx += b;
		sy += e;
		sz += h;
	}
}

static Enesim_Transformer_Func _functions[ENESIM_TRANSFORMATIONS] = {
		[ENESIM_TRANSFORMATION_AFFINE] = &transformer_affine_no_no,
		[ENESIM_TRANSFORMATION_IDENTITY] = &transformer_identity_no_no,
		[ENESIM_TRANSFORMATION_PROJECTIVE] = &transformer_projective_no_no,
};

/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * 
 */
EAPI Enesim_Transformation * enesim_transformation_new(void)
{
	Enesim_Transformation *t;
	
	t = calloc(1, sizeof(Enesim_Transformation));
	t->matrix = enesim_matrix_new();
	ENESIM_MAGIC_SET(t, ENESIM_TRANSFORMATION_MAGIC);
	
	return t;
}
/**
 * 
 */
EAPI void enesim_transformation_matrix_set(Enesim_Transformation *t, Enesim_Matrix *m)
{
	float a, b, c, d, e, f, g, h, i;
	
	ENESIM_ASSERT(t, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_MAGIC_CHECK(t, ENESIM_TRANSFORMATION_MAGIC);
	
	enesim_matrix_values_get(m, &a, &b, &c, &d, &e, &f, &g, &h, &i);
	enesim_matrix_values_set(t->matrix, a, b, c, d, e, f, g, h, i);
}
/**
 * 
 */
EAPI void enesim_transformation_matrix_get(Enesim_Transformation *t, Enesim_Matrix *m)
{
	float a, b, c, d, e, f, g, h, i;

	enesim_matrix_values_get(t->matrix, &a, &b, &c, &d, &e, &f, &g, &h, &i);
	enesim_matrix_values_set(m, a, b, c, d, e, f, g, h, i);
}
/**
 * 
 */
EAPI void enesim_transformation_rop_set(Enesim_Transformation *t, Enesim_Rop rop)
{
	t->rop = rop;
}
/**
 * 
 */
EAPI Eina_Bool enesim_transformation_apply(Enesim_Transformation *t,
		Enesim_Surface *s, Eina_Rectangle *sr, Enesim_Surface *d,
		Eina_Rectangle *dr)
{
	Eina_Rectangle csr, cdr;
	Enesim_Transformer_Func tfunc;
	Enesim_Scale xscale, yscale;
	
	ENESIM_ASSERT(t, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_ASSERT(s, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_ASSERT(d, ENESIM_ERROR_HANDLE_INVALID);
	
	ENESIM_MAGIC_CHECK(t, ENESIM_TRANSFORMATION_MAGIC);
	ENESIM_MAGIC_CHECK(s, ENESIM_SURFACE_MAGIC);
	ENESIM_MAGIC_CHECK(d, ENESIM_SURFACE_MAGIC);
	
	xscale = ENESIM_SCALE_NO;
	yscale = ENESIM_SCALE_NO;
	
	/* TODO check if we are out of bounds */
	/* setup the destination clipping */
	cdr.x = 0;
	cdr.y = 0;
	cdr.w = d->w;
	cdr.h = d->h;
	if (sr)
	{
		/* TODO check the return value of the intersection */
		if (eina_rectangle_intersection(&cdr, dr) == EINA_FALSE)
			return EINA_FALSE;
		if (eina_rectangle_is_empty(&cdr))
		{
			//ENESIM_ERROR(ENESIM_ERROR_DSTRECT_INVALID);
			return EINA_FALSE;
		}
	}
	/* setup the source clipping */
	csr.x = 0;
	csr.y = 0;
	csr.w = s->w;
	csr.h = s->h;
	if (dr)
	{
		/* TODO check the return value of the intersection */
		if (eina_rectangle_intersection(&csr, sr) == EINA_FALSE)
			return EINA_FALSE;
		if (eina_rectangle_is_empty(&csr))
		{
			//ENESIM_ERROR(ENESIM_ERROR_SRCRECT_INVALID);
			return EINA_FALSE;
		}
	}
	/* check if we are going to scale */
	/* x scaling */
	if (cdr.w > csr.w)
		xscale = ENESIM_SCALE_UP; 
	else if (cdr.w < csr.w)
		xscale = ENESIM_SCALE_DOWN;
	/* y scaling */
	if (cdr.h > csr.h)
		yscale = ENESIM_SCALE_UP;
	else if (cdr.h < csr.h)
		yscale = ENESIM_SCALE_DOWN;
	/* get the correct transfomer function */
	/* TODO use xscale and yscale */
	if (!(tfunc = _functions[_transformation_get(t->matrix)]))
	{
		//ENESIM_ERROR(ENESIM_ERROR_TRANSFORMATION_NOT_SUPPORTED);
		return EINA_FALSE;
	}
	tfunc(t, s, sr, d, dr);
	return EINA_TRUE;
}
/**
 * 
 */
EAPI void enesim_transformation_origin_set(Enesim_Transformation *t, float ox, float oy)
{
	t->ox = ox;
	t->oy = oy;
}
/**
 * 
 */
EAPI void enesim_transformation_origin_get(Enesim_Transformation *t, float *ox, float *oy)
{
	if (ox) *ox = t->ox;
	if (oy) *oy = t->oy;
}
