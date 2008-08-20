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
static Enesim_Transformation_Type _transformation_get(float *t)
{
	if ((t[MATRIX_ZX] != 0) || (t[MATRIX_ZY] != 0) || (t[MATRIX_ZZ] != 1))
	            return ENESIM_TRANSFORMATION_PROJECTIVE;
	else
	{
		if ((t[MATRIX_XX] == 1) && (t[MATRIX_XY] == 0) && (t[MATRIX_XZ] == 0) &&
				(t[MATRIX_YX] == 0) && (t[MATRIX_YY] == 1) && (t[MATRIX_YZ] == 0))
			return ENESIM_TRANSFORMATION_IDENTITY;
		else
			return ENESIM_TRANSFORMATION_AFFINE;
	}
}

/* convert the transformation values to fixed point */
static void _transformation_to_fixed(float *t, Eina_F16p16 *td)
{
	td[0] = eina_f16p16_float_from(t[0]);
	td[1] = eina_f16p16_float_from(t[1]);
	td[2] = eina_f16p16_float_from(t[2]);
	td[3] = eina_f16p16_float_from(t[3]);
	td[4] = eina_f16p16_float_from(t[4]);
	td[5] = eina_f16p16_float_from(t[5]);
	td[6] = eina_f16p16_float_from(t[6]);
	td[7] = eina_f16p16_float_from(t[7]);
	td[8] = eina_f16p16_float_from(t[8]);
}

static void _transformation_debug(Enesim_Transformation *t)
{
	
	printf("Transformation with rop = %d\n", t->rop);
	printf("Floating point matrix\n");
	printf("XX = %f XY = %f XZ = %f\n", t->matrix[MATRIX_XX], t->matrix[MATRIX_XY], t->matrix[MATRIX_XZ]);
	printf("YX = %f YY = %f YZ = %f\n", t->matrix[MATRIX_YX], t->matrix[MATRIX_YY], t->matrix[MATRIX_YZ]);
	printf("ZX = %f ZY = %f ZZ = %f\n", t->matrix[MATRIX_ZX], t->matrix[MATRIX_ZY], t->matrix[MATRIX_ZZ]);
	printf("Fixed point matrix (16p16 format)\n");
	printf("XX = %u XY = %u XZ = %u\n", t->matrix_fixed[MATRIX_XX], t->matrix_fixed[MATRIX_XY], t->matrix_fixed[MATRIX_XZ]);
	printf("YX = %u YY = %u YZ = %u\n", t->matrix_fixed[MATRIX_YX], t->matrix_fixed[MATRIX_YY], t->matrix_fixed[MATRIX_YZ]);
	printf("ZX = %u ZY = %u ZZ = %u\n", t->matrix_fixed[MATRIX_ZX], t->matrix_fixed[MATRIX_ZY], t->matrix_fixed[MATRIX_ZZ]);
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

static void transformer_identity_no_no(Enesim_Transformation *t, Enesim_Surface *ss,
		Eina_Rectangle *srect, Enesim_Surface *ds, Eina_Rectangle *drect)
{
	Enesim_Surface_Data sdata, ddata;
	Enesim_Drawer_Span spfnc;
	int h;
	
	enesim_surface_data_get(ss, &sdata);
	enesim_surface_data_get(ds, &ddata);
	enesim_surface_data_increment(&sdata, ss->format, (srect->y * ss->w) + srect->x);
	enesim_surface_data_increment(&ddata, ds->format, (drect->y * ds->w) + drect->x);
	h = drect->h;
	
	/* TODO, pixel or pixel_color */
	spfnc = enesim_drawer_span_pixel_get(t->rop, ds->format, ss->format);
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
	int h;
	
	sx = eina_f16p16_mul(t->matrix_fixed[MATRIX_XX], drect->x) + eina_f16p16_mul(t->matrix_fixed[MATRIX_XY], drect->y) + t->matrix_fixed[MATRIX_XZ];
	sy = eina_f16p16_mul(t->matrix_fixed[MATRIX_YX], drect->x) + eina_f16p16_mul(t->matrix_fixed[MATRIX_YY], drect->y) + t->matrix_fixed[MATRIX_YZ];
	
	enesim_surface_data_get(ds, &ddata);
	enesim_surface_data_get(ss, &sdata);
	enesim_surface_data_increment(&ddata, ds->format, (drect->y * ds->w) + drect->x);
	
	h = drect->h;
	while (h--)
	{
		Enesim_Surface_Data ddata2;
		int w;
		Eina_F16p16 sxx, syy;
		
		ddata2 = ddata;
		sxx = sx;
		syy = sy;

		w = drect->w;
		while (w--)
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
				ptfnc = enesim_drawer_point_color_get(t->rop, ds->format, argb);
				ptfnc(&ddata2, NULL, argb, NULL);
			}
			enesim_surface_data_increment(&ddata2, ds->format, 1);
			sxx += t->matrix_fixed[MATRIX_XX];
			syy += t->matrix_fixed[MATRIX_YX];
		}
		enesim_surface_data_increment(&ddata, ds->format, ds->w);
		sx += t->matrix_fixed[MATRIX_XY];
		sy += t->matrix_fixed[MATRIX_YY];
	}
}

static void transformer_projective_no_no(Enesim_Transformation *t, Enesim_Surface *ss,
		Eina_Rectangle *srect, Enesim_Surface *ds, Eina_Rectangle *drect)
{
	
}

static Enesim_Transformer_Func _functions[ENESIM_TRANSFORMATIONS] = {
		[ENESIM_TRANSFORMATION_AFFINE] = &transformer_affine_no_no,
		[ENESIM_TRANSFORMATION_IDENTITY] = &transformer_identity_no_no,
};

/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * 
 */
EAPI void enesim_transformation_matrix_compose(float *st, float *dt)
{
	int i;
	float tmp[MATRIX_SIZE];
	
	tmp[MATRIX_XX] = (st[MATRIX_XX] * dt[MATRIX_XX]) + (st[MATRIX_XY] * dt[MATRIX_YX]) + (st[MATRIX_XZ] * dt[MATRIX_ZX]);
	tmp[MATRIX_XY] = (st[MATRIX_XX] * dt[MATRIX_XY]) + (st[MATRIX_XY] * dt[MATRIX_YY]) + (st[MATRIX_XZ] * dt[MATRIX_ZY]);
	tmp[MATRIX_XZ] = (st[MATRIX_XX] * dt[MATRIX_XZ]) + (st[MATRIX_XY] * dt[MATRIX_YZ]) + (st[MATRIX_XZ] * dt[MATRIX_ZZ]);
	
	tmp[MATRIX_YX] = (st[MATRIX_YX] * dt[MATRIX_XX]) + (st[MATRIX_YY] * dt[MATRIX_YX]) + (st[MATRIX_YZ] * dt[MATRIX_ZX]);
	tmp[MATRIX_YY] = (st[MATRIX_YX] * dt[MATRIX_XY]) + (st[MATRIX_YY] * dt[MATRIX_YY]) + (st[MATRIX_YZ] * dt[MATRIX_ZY]);
	tmp[MATRIX_YZ] = (st[MATRIX_YX] * dt[MATRIX_XZ]) + (st[MATRIX_YY] * dt[MATRIX_YZ]) + (st[MATRIX_YZ] * dt[MATRIX_ZZ]);

	tmp[MATRIX_ZX] = (st[MATRIX_ZX] * dt[MATRIX_XX]) + (st[MATRIX_ZY] * dt[MATRIX_YX]) + (st[MATRIX_ZZ] * dt[MATRIX_ZX]);
	tmp[MATRIX_ZY] = (st[MATRIX_ZX] * dt[MATRIX_XY]) + (st[MATRIX_ZY] * dt[MATRIX_YY]) + (st[MATRIX_ZZ] * dt[MATRIX_ZY]);
	tmp[MATRIX_ZZ] = (st[MATRIX_ZX] * dt[MATRIX_XZ]) + (st[MATRIX_ZY] * dt[MATRIX_YZ]) + (st[MATRIX_ZZ] * dt[MATRIX_ZZ]);

	for (i = 0; i < MATRIX_SIZE; i++)
		st[i] = tmp[i];
}
/**
 * 
 */
EAPI void enesim_transformation_matrix_translate(float *t, float tx, float ty)
{
	t[MATRIX_XX] = 1;
	t[MATRIX_XY] = 0;
	t[MATRIX_XZ] = tx;
	t[MATRIX_YX] = 0;
	t[MATRIX_YY] = 1;
	t[MATRIX_YZ] = ty;
	t[MATRIX_ZX] = 0;
	t[MATRIX_ZY] = 0;
	t[MATRIX_ZZ] = 1;	
}
/**
 * 
 */
EAPI void enesim_transformation_matrix_scale(float *t, float sx, float sy)
{
	t[MATRIX_XX] = sx;
	t[MATRIX_XY] = 0;
	t[MATRIX_XZ] = 0;
	t[MATRIX_YX] = 0;
	t[MATRIX_YY] = sy;
	t[MATRIX_YZ] = 0;
	t[MATRIX_ZX] = 0;
	t[MATRIX_ZY] = 0;
	t[MATRIX_ZZ] = 1;
}
/**
 * 
 */
EAPI void enesim_transformation_matrix_rotate(float *t, float rad)
{
	float c = cos(rad);
	float s = sin(rad);
	
	printf("%f %f\n", c, s);
	t[MATRIX_XX] = c;
	t[MATRIX_XY] = -s;
	t[MATRIX_XZ] = 0;
	t[MATRIX_YX] = s;
	t[MATRIX_YY] = c;
	t[MATRIX_YZ] = 0;
	t[MATRIX_ZX] = 0;
	t[MATRIX_ZY] = 0;
	t[MATRIX_ZZ] = 1;	
}
/**
 * 
 */
EAPI void enesim_transformation_matrix_identity(float *t)
{
	t[MATRIX_XX] = 1;
	t[MATRIX_XY] = 0;
	t[MATRIX_XZ] = 0;
	t[MATRIX_YX] = 0;
	t[MATRIX_YY] = 1;
	t[MATRIX_YZ] = 0;
	t[MATRIX_ZX] = 0;
	t[MATRIX_ZY] = 0;
	t[MATRIX_ZZ] = 1;
}
/**
 * 
 */
EAPI Enesim_Transformation * enesim_transformation_new(void)
{
	Enesim_Transformation *t;
	
	t = calloc(1, sizeof(Enesim_Transformation));
	/* setup the identity matrix */
	t->matrix[MATRIX_XX] = 1;
	t->matrix[MATRIX_YY] = 1;
	t->matrix[MATRIX_ZZ] = 1;
	_transformation_to_fixed(t->matrix, t->matrix_fixed);
	t->type = ENESIM_TRANSFORMATION_IDENTITY;
	ENESIM_MAGIC_SET(t, ENESIM_TRANSFORMATION_MAGIC);
	
	return t;
}
/**
 * 
 */
EAPI void enesim_transformation_set(Enesim_Transformation *t, float *tx)
{
	int i;
	
	ENESIM_ASSERT(t, ENESIM_ERROR_HANDLE_INVALID);
	ENESIM_MAGIC_CHECK(t, ENESIM_TRANSFORMATION_MAGIC);
	
	for (i = 0; i < MATRIX_SIZE; i++)
	{
		t->matrix[i] = tx[i];
	}
	/* TODO the type should on the fixed or on the float */
	_transformation_to_fixed(t->matrix, t->matrix_fixed);
	t->type = _transformation_get(t->matrix);
	printf("Transformation type = %d\n", t->type);
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
	if (!(tfunc = _functions[t->type]))
	{
		//ENESIM_ERROR(ENESIM_ERROR_TRANSFORMATION_NOT_SUPPORTED);
		return EINA_FALSE;
	}
	printf("%p\n", tfunc);
	tfunc(t, s, sr, d, dr);
	return EINA_TRUE;
}
