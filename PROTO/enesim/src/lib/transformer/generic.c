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
/*
 * TODO
 * this function is very expensive because we have convert every time to argb
 * in case of argb8888_pre means several muls and divs
 * [ 0 1 0 ]
 * [ 1 1 1 ]
 * [ 0 1 0 ]
 */
static inline void convolution2x2(Enesim_Surface_Data *data, Eina_F16p16 x, Eina_F16p16 y,
		unsigned w, unsigned int h, Enesim_Surface_Pixel *ret)
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
		p0 = enesim_surface_data_argb_to(data);
	}
	if ((sy > -1) && ((sx + 1) < w))
	{
		tmp = *data;
		
		enesim_surface_data_increment(&tmp, 1);
		p1 = enesim_surface_data_argb_to(&tmp);
	}
	if ((sy + 1) < h)
	{
		if (sx > -1)
		{
			tmp = *data;
			enesim_surface_data_increment(&tmp, w);
			p2 = enesim_surface_data_argb_to(&tmp);
		}
		if ((sx + 1) < w)
		{
			tmp = *data;
			enesim_surface_data_increment(&tmp, w + 1);
			p3 = enesim_surface_data_argb_to(&tmp);
		}
	}
	if (p0 != p1)
		p0 = interp_256(ax, p1, p0);
	if (p2 != p3)
		p2 = interp_256(ax, p3, p2);
	if (p0 != p2)
		p0 = interp_256(ay, p2, p0);
	
	ret->pixel.argb8888.plane0 = p0;
	ret->format = ENESIM_SURFACE_ARGB8888;
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
	spfnc = enesim_drawer_span_pixel_get(t->rop, ds->sdata.format, ss->sdata.format);
	if (!spfnc)
		return;
	
	enesim_surface_data_get(ss, &sdata);
	enesim_surface_data_get(ds, &ddata);
	enesim_surface_data_increment(&sdata, (srect->y * ss->w) + srect->x);
	enesim_surface_data_increment(&ddata, (drect->y * ds->w) + drect->x);
	h = drect->h;
	
	while (h--)
	{
		spfnc(&ddata, drect->w, &sdata, /* mul_color */NULL, NULL);
		enesim_surface_data_increment(&sdata, ss->w);
		enesim_surface_data_increment(&ddata, ds->w);
	}	
}

#define normal_generic_generic_projective_no_no(body)				\
	Enesim_Surface_Data sdata, ddata;					\
	Eina_F16p16 sx, sy, sz;							\
	Enesim_Drawer_Point ptfnc;						\
	Eina_F16p16 a, b, c, d, e, f, g, h, i, ox, oy;				\
	Enesim_Surface_Pixel color;						\
										\
	int hlen;								\
										\
	ptfnc = enesim_transformation_drawer_point_get(t, ds, ss);		\
	if (!ptfnc)								\
		return;								\
										\
	enesim_matrix_fixed_values_get(t->matrix, &a, &b, &c, &d,		\
		&e, &f, &g, &h, &i);						\
	sx = eina_f16p16_mul(a, drect->x) + eina_f16p16_mul(b, drect->y) + c;	\
	sy = eina_f16p16_mul(d, drect->x) + eina_f16p16_mul(e, drect->y) + f;	\
	sz = eina_f16p16_mul(g, drect->x) + eina_f16p16_mul(h, drect->y) + i;	\
	ox = eina_f16p16_float_from(t->ox);					\
	oy = eina_f16p16_float_from(t->oy);					\
										\
	enesim_surface_data_get(ss, &sdata);					\
	enesim_surface_data_get(ds, &ddata);					\
	enesim_surface_data_increment(&sdata, (srect->y * ss->w) + srect->x);	\
	enesim_surface_data_increment(&ddata, (drect->y * ds->w) + drect->x);	\
										\
	hlen = drect->h;							\
	while (hlen--)								\
	{									\
		Enesim_Surface_Data ddata2;					\
		Eina_F16p16 sxx, syy, szz;					\
		int wlen;							\
										\
		ddata2 = ddata;							\
		sxx = sx;							\
		syy = sy;							\
		szz = sz;							\
										\
		wlen = drect->w;						\
		while (wlen--)							\
		{								\
			int six, siy;						\
			Enesim_Surface_Data sdata2;				\
			Eina_F16p16 sxxx, syyy;					\
										\
			sdata2 = sdata;						\
			/* TODO set the destination to zero? */			\
			if (!szz)						\
				goto iterate;					\
										\
			sxxx = ((((long long int)sxx) << 16) / szz) + ox;	\
			syyy = ((((long long int)syy) << 16) / szz) + oy;	\
			six = eina_f16p16_int_to(sxxx);				\
			siy = eina_f16p16_int_to(syyy);				\
			/* check that we are inside the source rectangle */	\
			if ((eina_rectangle_xcoord_inside(srect, six)) &&	\
				(eina_rectangle_ycoord_inside(srect, siy)))	\
			{							\
				Enesim_Surface_Pixel argb;			\
										\
				enesim_surface_data_increment(&sdata2, (siy * ss->w) + six);	\
										\
				body						\
										\
				ptfnc(&ddata2, &color, t->color, NULL);		\
			}							\
iterate:									\
			enesim_surface_data_increment(&ddata2, 1);		\
			sxx += a;						\
			syy += d;						\
			szz += g;						\
		}								\
		enesim_surface_data_increment(&ddata, ds->w);			\
		sx += b;							\
		sy += e;							\
		sz += h;							\
	}									\

#define normal_generic_generic_affine_no_no(body)				\
	Enesim_Surface_Data sdata, ddata;					\
	Eina_F16p16 sx, sy;							\
	Enesim_Drawer_Point ptfnc;						\
	Eina_F16p16 a, b, c, d, e, f, g, h, i;					\
	Enesim_Surface_Pixel color;						\
										\
	int hlen;								\
										\
	ptfnc = enesim_transformation_drawer_point_get(t, ds, ss);		\
	if (!ptfnc)								\
		return;								\
										\
	enesim_matrix_fixed_values_get(t->matrix, &a, &b, &c, &d,		\
			&e, &f, &g, &h, &i);					\
	c += eina_f16p16_float_from(t->ox);					\
	f += eina_f16p16_float_from(t->oy);					\
	sx = eina_f16p16_mul(a, drect->x) + eina_f16p16_mul(b, drect->y) + c;	\
	sy = eina_f16p16_mul(d, drect->x) + eina_f16p16_mul(e, drect->y) + f;	\
										\
	enesim_surface_data_get(ds, &ddata);					\
	enesim_surface_data_get(ss, &sdata);					\
	enesim_surface_data_increment(&ddata, (drect->y * ds->w) + drect->x);	\
										\
	hlen = drect->h;							\
	while (hlen--)								\
	{									\
		Enesim_Surface_Data ddata2;					\
		int wlen;							\
		Eina_F16p16 sxx, syy;						\
										\
		ddata2 = ddata;							\
		sxx = sx;							\
		syy = sy;							\
										\
		wlen = drect->w;						\
		while (wlen--)							\
		{								\
			int six, siy;						\
			Enesim_Surface_Data sdata2;				\
										\
			sdata2 = sdata;						\
			six = eina_f16p16_int_to(sxx);				\
			siy = eina_f16p16_int_to(syy);				\
										\
			/* check that we are inside the source rectangle */	\
			if ((eina_rectangle_xcoord_inside(srect, six)) &&	\
				(eina_rectangle_ycoord_inside(srect, siy)))	\
			{							\
				Enesim_Surface_Pixel argb;			\
										\
				enesim_surface_data_increment(&sdata2,		\
					(siy * ss->w) + six); 			\
				body 						\
				ptfnc(&ddata2, &color, t->color, NULL);		\
			}							\
			enesim_surface_data_increment(&ddata2, 1);		\
			sxx += a;						\
			syy += d;						\
		}								\
		enesim_surface_data_increment(&ddata, ds->w);			\
		sx += b;							\
		sy += e;							\
	}									\


#define mask_generic_generic_affine_no_no(body) body


static void normal_generic_affine_fast_no_no(Enesim_Transformation *t, Enesim_Surface *ss,
		Eina_Rectangle *srect, Enesim_Surface *ds, Eina_Rectangle *drect)
{
	normal_generic_generic_affine_no_no(
		enesim_surface_data_pixel_get(&sdata2, &argb);
		enesim_surface_pixel_convert(&argb, &color, ddata.format);
	);
}


static void normal_generic_affine_good_no_no(Enesim_Transformation *t, Enesim_Surface *ss,
		Eina_Rectangle *srect, Enesim_Surface *ds, Eina_Rectangle *drect)
{
	normal_generic_generic_affine_no_no(
		/* use 2x2 convolution kernel to interpolate */
		convolution2x2(&sdata2, sxx, syy, ss->w, ss->h, &argb);
		enesim_surface_pixel_convert(&argb, &color, ddata.format);
	);
}

static void normal_generic_projective_fast_no_no(Enesim_Transformation *t, Enesim_Surface *ss,
		Eina_Rectangle *srect, Enesim_Surface *ds, Eina_Rectangle *drect)
{
	normal_generic_generic_projective_no_no(
		enesim_surface_data_pixel_get(&sdata2, &argb);
		enesim_surface_pixel_convert(&argb, &color, ddata.format);
	);
}

static void normal_generic_projective_good_no_no(Enesim_Transformation *t, Enesim_Surface *ss,
		Eina_Rectangle *srect, Enesim_Surface *ds, Eina_Rectangle *drect)
{
	normal_generic_generic_projective_no_no(
		/* use 2x2 convolution kernel to interpolate */
		convolution2x2(&sdata2, sxx, syy, ss->w, ss->h, &argb);
		enesim_surface_pixel_convert(&argb, &color, ddata.format);
	);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Enesim_Transformer_Generic generic_tx = {
	.normal[ENESIM_TRANSFORMATION_AFFINE][ENESIM_GOOD] = normal_generic_affine_good_no_no,
	.normal[ENESIM_TRANSFORMATION_AFFINE][ENESIM_FAST] = normal_generic_affine_fast_no_no,
	.normal[ENESIM_TRANSFORMATION_PROJECTIVE][ENESIM_GOOD] = normal_generic_projective_good_no_no,
	.normal[ENESIM_TRANSFORMATION_PROJECTIVE][ENESIM_FAST] = normal_generic_projective_fast_no_no,
};
