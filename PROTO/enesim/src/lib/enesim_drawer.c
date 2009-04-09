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
/*
 * TODO
 * rop functions
 * pixel
 * color in ARGB format
 * pixel_color
 * mask_color
 * pixel_mask
 * TODO
 * color *done*
 * mask_color
 * pixel *done*
 * pixel_color
 * pixel_mask
 *
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*
 * A drawer should implement functions for every format in case of using
 * pixel source. For color source it should implement the function with
 * opaque value and no opaque.
 */
typedef struct _Enesim_Drawer
{
	/* Scanlines */
	Enesim_Drawer_Span sp_color[ENESIM_ROPS][ENESIM_FORMATS];
	Enesim_Drawer_Span sp_mask_color[ENESIM_ROPS][ENESIM_FORMATS][ENESIM_FORMATS];
	Enesim_Drawer_Span sp_pixel[ENESIM_ROPS][ENESIM_FORMATS][ENESIM_FORMATS];
	Enesim_Drawer_Span sp_pixel_color[ENESIM_ROPS][ENESIM_FORMATS][ENESIM_FORMATS];
	Enesim_Drawer_Span sp_pixel_mask[ENESIM_ROPS][ENESIM_FORMATS][ENESIM_FORMATS][ENESIM_FORMATS];
	/* Points */
	Enesim_Drawer_Point pt_color[ENESIM_ROPS][ENESIM_FORMATS];
	Enesim_Drawer_Point pt_mask_color[ENESIM_ROPS][ENESIM_FORMATS][ENESIM_FORMATS];
	Enesim_Drawer_Point pt_pixel[ENESIM_ROPS][ENESIM_FORMATS][ENESIM_FORMATS];
	Enesim_Drawer_Point pt_pixel_color[ENESIM_ROPS][ENESIM_FORMATS][ENESIM_FORMATS];
	Enesim_Drawer_Point pt_pixel_mask[ENESIM_ROPS][ENESIM_FORMATS][ENESIM_FORMATS][ENESIM_FORMATS];
} Enesim_Drawer;

static Enesim_Drawer *_drawers;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_drawer_init(void)
{
	unsigned int numcpu;
	Enesim_Cpu **cpus;
	int i;

	cpus = enesim_cpu_get(&numcpu);
	/* allocate the possible drawers */
	_drawers = calloc(numcpu, sizeof(Enesim_Drawer));
	for (i = 0; i < numcpu; i++)
	{
		enesim_drawer_argb8888_init(cpus[i]);
	}
}
void enesim_drawer_shutdown(void)
{
#if 0
	/* last the generic drawer */
	enesim_drawer_generic_shutdown();
	enesim_drawer_argb8888_shutdown();
#endif
	free(_drawers);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool enesim_drawer_point_color_op_get(Enesim_Cpu *cpu, Enesim_Operator *op,
		Enesim_Rop rop, Enesim_Format dfmt, uint32_t color)
{
	Enesim_Drawer_Point p;
	unsigned int cpuid;

	/* TODO check if the color is opaque */
#if 0
	if ((rop == ENESIM_BLEND) && (alpha(color) == 0xff))
	{
		rop = RENESIM_FILL;
	}
#endif
	cpuid = enesim_cpu_id_get(cpu);
	p = _drawers[cpuid].pt_color[rop][dfmt];
	if (p)
	{
		op->cb = p;
		op->id = ENESIM_OPERATOR_DRAWER_POINT;
		op->cpu = cpu;
		return EINA_TRUE;
	}
	else
	{
		op->cb = NULL;
		return EINA_FALSE;
	}
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool enesim_drawer_point_mask_color_op_get(Enesim_Cpu *cpu, Enesim_Operator *op,
		Enesim_Rop rop, Enesim_Format dfmt, uint32_t color,
		Enesim_Format mfmt)
{
	Enesim_Drawer_Point p;
	unsigned int cpuid;

	cpuid = enesim_cpu_id_get(cpu);
	p = _drawers[cpuid].pt_mask_color[rop][dfmt][mfmt];
	if (p)
	{
		op->cb = p;
		op->id = ENESIM_OPERATOR_DRAWER_POINT;
		op->cpu = cpu;
		return EINA_TRUE;
	}
	else
	{
		op->cb = NULL;
		return EINA_FALSE;
	}
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool enesim_drawer_point_pixel_op_get(Enesim_Cpu *cpu, Enesim_Operator *op,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt)
{
	Enesim_Drawer_Point p;
	unsigned int cpuid;

	cpuid = enesim_cpu_id_get(cpu);
	p = _drawers[cpuid].pt_pixel[rop][dfmt][sfmt];
	if (p)
	{
		op->cb = p;
		op->id = ENESIM_OPERATOR_DRAWER_POINT;
		op->cpu = cpu;
		return EINA_TRUE;
	}
	else
	{
		op->cb = NULL;
		return EINA_FALSE;
	}
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool enesim_drawer_point_pixel_color_op_get(Enesim_Cpu *cpu, Enesim_Operator *op,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt,
		uint32_t color)
{
	Enesim_Drawer_Point p;
	unsigned int cpuid;

	cpuid = enesim_cpu_id_get(cpu);
	p = _drawers[cpuid].pt_pixel_color[rop][dfmt][sfmt];
	if (p)
	{
		op->cb = p;
		op->id = ENESIM_OPERATOR_DRAWER_POINT;
		op->cpu = cpu;
		return EINA_TRUE;
	}
	else
	{
		op->cb = NULL;
		return EINA_FALSE;
	}
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Eina_Bool enesim_drawer_point_pixel_mask_op_get(Enesim_Cpu *cpu, Enesim_Operator *op,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt,
		Enesim_Format mfmt)
{
	Enesim_Drawer_Point p;
	unsigned int cpuid;

	cpuid = enesim_cpu_id_get(cpu);
	p = _drawers[cpuid].pt_pixel_mask[rop][dfmt][sfmt][mfmt];
	if (p)
	{
		op->cb = p;
		op->id = ENESIM_OPERATOR_DRAWER_POINT;
		op->cpu = cpu;
		return EINA_TRUE;
	}
	else
	{
		op->cb = NULL;
		return EINA_FALSE;
	}
}

/* Span functions */

/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with color color
 */
EAPI Eina_Bool enesim_drawer_span_color_op_get(Enesim_Cpu *cpu, Enesim_Operator *op,
		Enesim_Rop rop, Enesim_Format dfmt, uint32_t color)
{
	Enesim_Drawer_Span s;
	unsigned int cpuid;
	/* TODO check if the color is opaque */
#if 0
	if ((rop == ENESIM_BLEND) && (alpha(color) == 0xff))
	{
		rop = RENESIM_FILL;
	}
#endif
	cpuid = enesim_cpu_id_get(cpu);
	s = _drawers[cpuid].sp_color[rop][dfmt];
	if (s)
	{
		op->cb = s;
		op->id = ENESIM_OPERATOR_DRAWER_SPAN;
		op->cpu = cpu;
		return EINA_TRUE;
	}
	else
	{
		op->cb = NULL;
		return EINA_FALSE;
	}
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with alpha values from the mask
 * and multiplying with color color
 */
EAPI Eina_Bool enesim_drawer_span_mask_color_op_get(Enesim_Cpu *cpu, Enesim_Operator *op,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format mfmt,
		uint32_t color)
{
	Enesim_Drawer_Span s;
	unsigned int cpuid;

	cpuid = enesim_cpu_id_get(cpu);
	s = _drawers[cpuid].sp_mask_color[rop][dfmt][mfmt];
	if (s)
	{
		op->cb = s;
		op->id = ENESIM_OPERATOR_DRAWER_SPAN;
		op->cpu = cpu;
		return EINA_TRUE;
	}
	else
	{
		op->cb = NULL;
		return EINA_FALSE;
	}
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with pixels of format sfmt
 */
EAPI Eina_Bool enesim_drawer_span_pixel_op_get(Enesim_Cpu *cpu, Enesim_Operator *op,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt)
{
	Enesim_Drawer_Span s;
	unsigned int cpuid;

	cpuid = enesim_cpu_id_get(cpu);
	s = _drawers[cpuid].sp_pixel[rop][dfmt][sfmt];
	if (s)
	{
		op->cb = s;
		op->id = ENESIM_OPERATOR_DRAWER_SPAN;
		op->cpu = cpu;
		return EINA_TRUE;
	}
	else
	{
		op->cb = NULL;
		return EINA_FALSE;
	}
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with pixels of format sfmt
 * multypling with color color
 */
EAPI Eina_Bool enesim_drawer_span_pixel_color_op_get(Enesim_Cpu *cpu, Enesim_Operator *op,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt,
		uint32_t color)
{
	Enesim_Drawer_Span s;
	unsigned int cpuid;

	cpuid = enesim_cpu_id_get(cpu);
	s = _drawers[cpuid].sp_pixel_color[rop][dfmt][sfmt];
	if (s)
	{
		op->cb = s;
		op->id = ENESIM_OPERATOR_DRAWER_SPAN;
		op->cpu = cpu;
		return EINA_TRUE;
	}
	else
	{
		op->cb = NULL;
		return EINA_FALSE;
	}
}
/**
 * Returns a function that will draw a span of pixels using the raster
 * operation rop for a surface format dfmt with alpha values from the mask
 * and multiplying with the pixel values from sfmt
 * @param
 * @param
 */
EAPI Eina_Bool enesim_drawer_span_pixel_mask_op_get(Enesim_Cpu *cpu, Enesim_Operator *op,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt,
		Enesim_Format mfmt)
{
	Enesim_Drawer_Span s;
	unsigned int cpuid;

	cpuid = enesim_cpu_id_get(cpu);
	s = _drawers[cpuid].sp_pixel_mask[rop][dfmt][sfmt][mfmt];
	if (s)
	{
		op->cb = s;
		op->id = ENESIM_OPERATOR_DRAWER_SPAN;
		op->cpu = cpu;
		return EINA_TRUE;
	}
	else
	{
		op->cb = NULL;
		return EINA_FALSE;
	}
}

EAPI void enesim_drawer_span_color_register(Enesim_Cpu *cpu,
		Enesim_Drawer_Span sp, Enesim_Rop rop, Enesim_Format dfmt)
{
	unsigned int cpuid;

	cpuid = enesim_cpu_id_get(cpu);
	_drawers[cpuid].sp_color[rop][dfmt] = sp;
}
EAPI void enesim_drawer_span_pixel_register(Enesim_Cpu *cpu,
		Enesim_Drawer_Span sp, Enesim_Rop rop, Enesim_Format dfmt,
		Enesim_Format sfmt)
{
	unsigned int cpuid;

	cpuid = enesim_cpu_id_get(cpu);
	_drawers[cpuid].sp_pixel[rop][dfmt][sfmt] = sp;
}
EAPI void enesim_drawer_span_mask_color_register(Enesim_Cpu *cpu,
		Enesim_Drawer_Span sp, Enesim_Rop rop, Enesim_Format dfmt,
		Enesim_Format mfmt)
{
	unsigned int cpuid;

	cpuid = enesim_cpu_id_get(cpu);
	_drawers[cpuid].sp_mask_color[rop][dfmt][mfmt] = sp;
}
EAPI void enesim_drawer_span_pixel_mask_register(Enesim_Cpu *cpu,
		Enesim_Drawer_Span sp, Enesim_Rop rop, Enesim_Format dfmt,
		Enesim_Format sfmt, Enesim_Format mfmt)
{
	unsigned int cpuid;

	cpuid = enesim_cpu_id_get(cpu);
	_drawers[cpuid].sp_pixel_mask[rop][dfmt][sfmt][mfmt] = sp;
}
EAPI void enesim_drawer_span_pixel_color_register(Enesim_Cpu *cpu,
		Enesim_Drawer_Span sp, Enesim_Rop rop, Enesim_Format dfmt,
		Enesim_Format sfmt)
{
	unsigned int cpuid;

	cpuid = enesim_cpu_id_get(cpu);
	_drawers[cpuid].sp_pixel_color[rop][dfmt][sfmt] = sp;
}
