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
typedef Enesim_Transformer_1D Enesim_Transformer_1D_Lut[ENESIM_FORMATS][ENESIM_MATRIX_TYPES][ENESIM_QUALITIES][ENESIM_FORMATS];
typedef Enesim_Transformer_2D Enesim_Transformer_2D_Lut[ENESIM_FORMATS][ENESIM_MATRIX_TYPES][ENESIM_QUALITIES][ENESIM_FORMATS];

Enesim_Transformer_1D_Lut *_transformers1d;
Enesim_Transformer_2D_Lut *_transformers2d;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_transformer_init(void)
{
	Enesim_Cpu **cpus;
	int numcpu;
	int i;

	cpus = enesim_cpu_get(&numcpu);
	_transformers1d = calloc(numcpu, sizeof(Enesim_Transformer_1D_Lut));
	_transformers2d = calloc(numcpu, sizeof(Enesim_Transformer_2D_Lut));
	for (i = 0; i < numcpu; i++)
	{
		enesim_transformer_argb8888_init(cpus[i]);
	}
}

void enesim_transformer_shutdown(void)
{
	free(_transformers1d);
	free(_transformers2d);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void enesim_transformer_1d_register(Enesim_Cpu *cpu, Enesim_Transformer_1D tx,
		Enesim_Format sfmt, Enesim_Matrix_Type type, Enesim_Quality q,
		Enesim_Format dfmt)
{
	unsigned int cpuid;
	Enesim_Transformer_1D_Lut *t;

	cpuid = enesim_cpu_id_get(cpu);
	t = &_transformers1d[cpuid];
	*t[sfmt][type][q][dfmt] = tx;
}

EAPI Eina_Bool enesim_transformer_1d_op_get(Enesim_Operator *op,
		Enesim_Cpu *cpu, Enesim_Format sfmt, Enesim_Matrix_Type type,
		Enesim_Quality q, Enesim_Format dfmt)
{
	unsigned int cpuid;
	Enesim_Transformer_1D_Lut *t;
	Enesim_Transformer_1D tx;

	cpuid = enesim_cpu_id_get(cpu);
	t = &_transformers1d[cpuid];
	tx = *t[sfmt][type][q][dfmt];
	if (tx)
	{
		op->id = ENESIM_OPERATOR_TRANSFORMER1D;
		op->cb = tx;
		op->cpu = cpu;
		return EINA_TRUE;
	}
	else
		return EINA_FALSE;
}
