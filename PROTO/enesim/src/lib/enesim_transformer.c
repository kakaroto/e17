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


extern Enesim_Transformer argb8888_tx;
extern Enesim_Transformer_Generic generic_tx;

static Enesim_Transformer *transformer[ENESIM_SURFACE_FORMATS] = {
		[ENESIM_SURFACE_ARGB8888] = &argb8888_tx,
};


Enesim_Transformer_Func _transformer_get(Enesim_Transformation *t,
		Enesim_Surface *s, Enesim_Surface *d)
{
	Enesim_Transformer_Func tfunc = NULL;

	if (t->mask)
	{
		if (transformer[d->sdata.format])
			tfunc = transformer[d->sdata.format]->mask[s->sdata.format][t->mask->sdata.format][_transformation_get(&t->matrix)][t->quality];
	}
	else
	{
		if (transformer[d->sdata.format])
			tfunc = transformer[d->sdata.format]->normal[s->sdata.format][_transformation_get(&t->matrix)][t->quality];
	}
	/* handle here the generic transformer */
	if (!tfunc)
	{
		if (t->mask)
			tfunc = generic_tx.mask[_transformation_get(&t->matrix)][t->quality];
		else
			tfunc = generic_tx.normal[_transformation_get(&t->matrix)][t->quality];
	}
	return tfunc;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Enesim_Drawer_Point enesim_transformation_drawer_point_get(Enesim_Transformation *t,
		Enesim_Surface *d,
		Enesim_Surface *s)
{
	Enesim_Drawer_Point pfunc;
	Enesim_Surface_Pixel src;

	/* TODO convert the color to the destination format */
	/* get a transparent pixel source */
	enesim_surface_pixel_components_from(&src, s->sdata.format, 0xaa, 0xff, 0xff, 0xff);
	if (t->mask)
	{
		Enesim_Surface_Pixel mask;
		/* get a transparent color for the mask format, to force a
		 * real operation */
		enesim_surface_pixel_components_from(&mask, t->mask->sdata.format, 0xaa, 0xff, 0xff, 0xff);
		return enesim_drawer_point_get(t->rop, d->sdata.format, &src, NULL, &mask);
	}
	else
		return enesim_drawer_point_get(t->rop, d->sdata.format, &src, t->color, NULL);
}
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
	enesim_matrix_identity(&t->matrix);
	ENESIM_MAGIC_SET(t, ENESIM_TRANSFORMATION_MAGIC);

	return t;
}
/**
 *
 */
EAPI void enesim_transformation_delete(Enesim_Transformation *t)
{
	free(t);
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
	enesim_matrix_values_set(&t->matrix, a, b, c, d, e, f, g, h, i);
}
/**
 *
 */
EAPI void enesim_transformation_matrix_get(Enesim_Transformation *t, Enesim_Matrix *m)
{
	float a, b, c, d, e, f, g, h, i;

	enesim_matrix_values_get(&t->matrix, &a, &b, &c, &d, &e, &f, &g, &h, &i);
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
EAPI void enesim_transformation_mask_set(Enesim_Transformation *t, Enesim_Surface *mask)
{
	t->mask = mask;
	if (t->color) t->color = NULL;
}
/**
 *
 */
EAPI void enesim_transformation_color_set(Enesim_Transformation *t, Enesim_Surface_Pixel *color)
{
	t->color = color;
	if (t->mask) t->mask = NULL;
}

/**
 *
 */
EAPI void enesim_transformation_border_set(Enesim_Transformation *tx, int l, int t, int r, int b)
{
	tx->border.l = l;
	tx->border.t = t;
	tx->border.r = r;
	tx->border.b = b;
	tx->border.used = EINA_TRUE;
}
/**
 *
 */
EAPI void enesim_transformation_quality_set(Enesim_Transformation *tx, Enesim_Quality q)
{
	tx->quality = q;
}
/**
 *
 */
EAPI Enesim_Quality enesim_transformation_quality_get(Enesim_Transformation *tx)
{
	return tx->quality;
}
/**
 *
 */
EAPI void enesim_transformation_border_unset(Enesim_Transformation *t)
{
	t->border.used = EINA_FALSE;
}
/**
 *
 */
EAPI Eina_Bool enesim_transformation_border_get(Enesim_Transformation *tx, int *l, int *t, int *r, int *b)
{
	if (l) *l = tx->border.l;
	if (t) *t = tx->border.t;
	if (r) *r = tx->border.r;
	if (b) *b = tx->border.b;
	return tx->border.used;
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


/**
 *
 */
EAPI Eina_Bool enesim_transformation_apply(Enesim_Transformation *t,
		Enesim_Surface *s, Eina_Rectangle *sr, Enesim_Surface *d,
		Eina_Rectangle *dr)
{
	Eina_Rectangle csr, cdr;
	Enesim_Transformer_Func tfunc = NULL;
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
	/* TODO check that the mask should be of the same size of the src */
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
	/* TODO get the ptfunc from the new function and return if NULL */
	tfunc = _transformer_get(t, s, d);
	if (!tfunc)
		return EINA_FALSE;
	tfunc(t, s, sr, d, dr);
	return EINA_TRUE;
}
