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
typedef Enesim_Converter_1D Enesim_Converter_1D_From_Lut[ENESIM_FORMATS][ENESIM_CONVERTER_FORMATS];
typedef Enesim_Converter_1D Enesim_Converter_1D_To_Lut[ENESIM_CONVERTER_FORMATS][ENESIM_FORMATS];

typedef Enesim_Converter_2D Enesim_Converter_2D_Lut[ENESIM_FORMATS][ENESIM_ROTATOR_ANGLES][ENESIM_CONVERTER_FORMATS];

Enesim_Converter_1D_From_Lut *_converters_from1d;
Enesim_Converter_1D_To_Lut *_converters_to1d;
Enesim_Converter_2D_Lut *_converters2d;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_converter_init(void)
{
	Enesim_Cpu **cpus;
	int numcpu;
	int i;

	cpus = enesim_cpu_get(&numcpu);
	_converters_from1d = calloc(numcpu, sizeof(Enesim_Converter_1D_From_Lut));
	_converters_to1d = calloc(numcpu, sizeof(Enesim_Converter_1D_From_Lut));
	_converters2d = calloc(numcpu, sizeof(Enesim_Converter_2D_Lut));
	for (i = 0; i < numcpu; i++)
	{
		enesim_converter_argb8888_init(cpus[i]);
		enesim_converter_rgb565_init(cpus[i]);
	}
}
void enesim_converter_shutdown(void)
{
	free(_converters_from1d);
	free(_converters_to1d);
	free(_converters2d);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void enesim_converter_1d_from_register(Enesim_Converter_1D cnv, Enesim_Cpu *cpu,
		Enesim_Format sfmt, Enesim_Converter_Format dfmt)
{
	unsigned int cpuid;
	Enesim_Converter_1D_From_Lut *t;

	cpuid = enesim_cpu_id_get(cpu);
	t = &_converters_from1d[cpuid];
	*t[sfmt][dfmt] = cnv;
}

EAPI void enesim_converter_1d_to_register(Enesim_Converter_1D cnv, Enesim_Cpu *cpu,
		Enesim_Converter_Format sfmt, Enesim_Format dfmt)
{
	unsigned int cpuid;
	Enesim_Converter_1D_To_Lut *t;

	cpuid = enesim_cpu_id_get(cpu);
	t = &_converters_to1d[cpuid];
	*t[sfmt][dfmt] = cnv;
}

EAPI void enesim_converter_2d_register(Enesim_Converter_2D cnv, Enesim_Cpu *cpu,
		Enesim_Format sfmt, Enesim_Rotator_Angle angle, Enesim_Converter_Format dfmt)
{
	unsigned int cpuid;
	Enesim_Converter_2D_Lut *t;

	cpuid = enesim_cpu_id_get(cpu);
	t = &_converters2d[cpuid];
	*t[sfmt][angle][dfmt] = cnv;
}

EAPI Eina_Bool enesim_converter_1d_from_get(Enesim_Operator *op, Enesim_Cpu *cpu, Enesim_Format sfmt,
		Enesim_Converter_Format dfmt)
{
	unsigned int cpuid;
	Enesim_Converter_1D_From_Lut *t;
	Enesim_Converter_1D c;

	cpuid = enesim_cpu_id_get(cpu);
	t = &_converters_from1d[cpuid];
	c = *t[sfmt][dfmt];
	if (c)
	{
		op->cpu = cpu;
		op->id = ENESIM_OPERATOR_CONVERTER1D;
		op->cb = c;
		return EINA_TRUE;
	}
	else
	{
		op->cb = NULL;
		return EINA_FALSE;
	}
}

EAPI Eina_Bool enesim_converter_1d_to_get(Enesim_Operator *op, Enesim_Cpu *cpu, Enesim_Converter_Format sfmt,
		Enesim_Format dfmt)
{
	unsigned int cpuid;
	Enesim_Converter_1D_To_Lut *t;
	Enesim_Converter_1D c;

	cpuid = enesim_cpu_id_get(cpu);
	t = &_converters_to1d[cpuid];
	c = *t[sfmt][dfmt];
	if (c)
	{
		op->cpu = cpu;
		op->id = ENESIM_OPERATOR_CONVERTER1D;
		op->cb = c;
		return EINA_TRUE;
	}
	else
	{
		op->cb = NULL;
		return EINA_FALSE;
	}
}


EAPI Eina_Bool enesim_converter_2d_get(Enesim_Operator *op, Enesim_Cpu *cpu, Enesim_Format sfmt,
		Enesim_Rotator_Angle angle, Enesim_Converter_Format dfmt)
{
	unsigned int cpuid;
	Enesim_Converter_2D_Lut *t;
	Enesim_Converter_2D c;

	cpuid = enesim_cpu_id_get(cpu);
	t = &_converters2d[cpuid];
	c = *t[sfmt][angle][dfmt];
	if (c)
	{
		op->cpu = cpu;
		op->id = ENESIM_OPERATOR_CONVERTER2D;
		op->cb = c;
		return EINA_TRUE;
	}
	else
	{
		op->cb = NULL;
		return EINA_FALSE;
	}
}
/**
 *
 */
EAPI Enesim_Converter_Format enesim_converter_format_get(uint8_t aoffset, uint8_t alen,
		uint8_t roffset, uint8_t rlen, uint8_t goffset, uint8_t glen,
		uint8_t boffset, uint8_t blen)
{

}
/**
 *
 */
EAPI Eina_Bool enesim_converter_format_cmp(Enesim_Format fmt, Enesim_Converter_Format cfmt)
{
	if (fmt == ENESIM_FORMAT_ARGB8888 && cfmt == ENESIM_CONVERTER_ARGB8888_PRE)
		return EINA_TRUE;
	else
		return EINA_FALSE;
}
