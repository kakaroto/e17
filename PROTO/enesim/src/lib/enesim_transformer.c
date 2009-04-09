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
/* FIXME this code is the old enesim_transformer.c, we need to make it span
 * wise so i will be refactored, isnt deleted yet as we might use some of this
 * interfaces for the newer renderer
 */
#include "Enesim.h"
#include "enesim_private.h"
#if 0
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Renderer_Transformer
{
	Enesim_Renderer r;
	Enesim_Quality quality;
	Enesim_Surface *src;
	struct {
		float x, y;
	} origin;
	struct {
		Enesim_Matrix inv;
		Eina_F16p16 fixed[9];
	} matrix;
} Renderer_Transformer;

static void _transformation_debug(Enesim_Renderer *t)
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


static Enesim_Renderer_Span _get(Renderer_Transformer *s, Enesim_Format *f)
{
	/* TODO check the matrix type */
	/* TODO check the src format */
	return ENESIM_RENDERER_SPAN(_affine);
}

static void _free(Renderer_Transformer *t)
{
	free(t);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Renderer * enesim_renderer_transformer_new(void)
{
	Renderer_Transformer *t;

	t = calloc(1, sizeof(Renderer_Transformer));
	t->r.free = ENESIM_RENDERER_FREE(_free);
	t->r.get = ENESIM_RENDERER_GET(_get);
	/* TODO setup an identity matrix */
	return &t->r;
}
/**
 *
 */
EAPI void enesim_renderer_transformer_matrix_set(Enesim_Renderer *r, Enesim_Matrix *m)
{
	Renderer_Transformer *t = (Renderer_Transformer *)r;
	float a, b, c, d, e, f, g, h, i;

	enesim_matrix_values_get(m, &a, &b, &c, &d, &e, &f, &g, &h, &i);
	enesim_matrix_values_set(&t->matrix.inv, a, b, c, d, e, f, g, h, i);
	enesim_matrix_fixed_values_get(&t->matrix.inv,
			&t->matrix.fixed[0], &t->matrix.fixed[1], &t->matrix.fixed[2],
			&t->matrix.fixed[3], &t->matrix.fixed[4], &t->matrix.fixed[5],
			&t->matrix.fixed[6], &t->matrix.fixed[7], &t->matrix.fixed[8]);
}
/**
 *
 */
EAPI void enesim_renderer_transformer_matrix_get(Enesim_Renderer *r, Enesim_Matrix *m)
{
	Renderer_Transformer *t = (Renderer_Transformer *)r;
	float a, b, c, d, e, f, g, h, i;

	enesim_matrix_values_get(&t->matrix.inv, &a, &b, &c, &d, &e, &f, &g, &h, &i);
	enesim_matrix_values_set(m, a, b, c, d, e, f, g, h, i);
}
#if 0
/**
 *
 */
EAPI void enesim_renderer_transformer_mask_set(Enesim_Renderer *r, Enesim_Surface *mask)
{
	Renderer_Transformer *t = (Renderer_Transformer *)r;
	t->mask = mask;
	if (t->color) t->color = NULL;
}
/**
 *
 */
EAPI void enesim_renderer_transformer_color_set(Enesim_Renderer *r, Enesim_Surface_Pixel *color)
{
	Renderer_Transformer *t = (Renderer_Transformer *)r;
	t->color = color;
	if (t->mask) t->mask = NULL;
}
#endif
/**
 *
 */
EAPI void enesim_renderer_transformer_src_set(Enesim_Renderer *r, Enesim_Surface *src)
{
	Renderer_Transformer *t = (Renderer_Transformer *)r;
	t->src = src;
}
/**
 *
 */
EAPI void enesim_renderer_transformer_quality_set(Enesim_Renderer *r, Enesim_Quality q)
{
	Renderer_Transformer *t = (Renderer_Transformer *)r;
	t->quality = q;
}
/**
 *
 */
EAPI Enesim_Quality enesim_renderer_transformer_quality_get(Enesim_Renderer *r)
{
	Renderer_Transformer *t = (Renderer_Transformer *)r;
	return t->quality;
}
/**
 *
 */
EAPI void enesim_renderer_transformer_origin_set(Enesim_Renderer *r, float ox, float oy)
{
	Renderer_Transformer *t = (Renderer_Transformer *)r;
	t->origin.x = ox;
	t->origin.y = oy;
}
/**
 *
 */
EAPI void enesim_renderer_transformer_origin_get(Enesim_Renderer *r, float *ox, float *oy)
{
	Renderer_Transformer *t = (Renderer_Transformer *)r;
	if (ox) *ox = t->origin.x;
	if (oy) *oy = t->origin.y;
}
#endif

/* TODO add the quality too */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef Enesim_Transformer Enesim_Transformer_Lut[ENESIM_FORMATS][ENESIM_MATRIX_TYPES][ENESIM_FORMATS];
Enesim_Transformer_Lut *_transformers;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_transformer_init(void)
{
	int numcpu;

	enesim_cpu_get(&numcpu);
	_transformers = malloc(sizeof(Enesim_Transformer_Lut) * numcpu);
}
void enesim_transformer_shutdown(void)
{
	free(_transformers);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void enesim_transformer_register(Enesim_Cpu *cpu, Enesim_Transformer tx,
		Enesim_Format sfmt, Enesim_Matrix_Type type,
		Enesim_Converter_Format dfmt)
{
	unsigned int cpuid;
	Enesim_Transformer_Lut *t;

	cpuid = enesim_cpu_id_get(cpu);
	t = &_transformers[cpuid];
	*t[sfmt][type][dfmt] = tx;
}

EAPI Eina_Bool enesim_transformer_op_get(Enesim_Operator *op,
		Enesim_Cpu *cpu, Enesim_Format sfmt, Enesim_Matrix_Type type,
		Enesim_Converter_Format dfmt)
{
	unsigned int cpuid;
	Enesim_Transformer_Lut *t;
	Enesim_Transformer tx;

	cpuid = enesim_cpu_id_get(cpu);
	t = &_transformers[cpuid];
	tx = *t[sfmt][type][dfmt];
	if (tx)
	{
		op->id = ENESIM_OPERATOR_TRANSFORMER;
		op->cb = tx;
		op->cpu = cpu;
		return EINA_TRUE;
	}
	else
		return EINA_FALSE;
}
