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
typedef Enesim_Raddist_1D Enesim_Raddist_1D_Lut[ENESIM_FORMATS][ENESIM_QUALITIES][ENESIM_FORMATS];
typedef Enesim_Raddist_2D Enesim_Raddist_2D_Lut[ENESIM_FORMATS][ENESIM_QUALITIES][ENESIM_FORMATS];

Enesim_Raddist_1D_Lut *_raddists1d;
Enesim_Raddist_2D_Lut *_raddists2d;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_raddist_init(void)
{
	Enesim_Cpu **cpus;
	int numcpu;
	int i;

	cpus = enesim_cpu_get(&numcpu);
	_raddists1d = calloc(numcpu, sizeof(Enesim_Raddist_1D_Lut));
	_raddists2d = calloc(numcpu, sizeof(Enesim_Raddist_2D_Lut));
	for (i = 0; i < numcpu; i++)
	{
		enesim_raddist_argb8888_init(cpus[i]);
	}
}

void enesim_raddist_shutdown(void)
{
	free(_raddists1d);
	free(_raddists2d);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void enesim_raddist_1d_register(Enesim_Cpu *cpu, Enesim_Raddist_1D tx,
		Enesim_Format sfmt, Enesim_Quality q, Enesim_Format dfmt)
{
	unsigned int cpuid;
	Enesim_Raddist_1D_Lut *t;

	cpuid = enesim_cpu_id_get(cpu);
	t = &_raddists1d[cpuid];
	*t[sfmt][q][dfmt] = tx;
}

EAPI Eina_Bool enesim_raddist_1d_op_get(Enesim_Operator *op,
		Enesim_Cpu *cpu, Enesim_Format sfmt, Enesim_Quality q,
		 Enesim_Format dfmt)
{
	unsigned int cpuid;
	Enesim_Raddist_1D_Lut *t;
	Enesim_Raddist_1D tx;

	cpuid = enesim_cpu_id_get(cpu);
	t = &_raddists1d[cpuid];
	tx = *t[sfmt][q][dfmt];
	if (tx)
	{
		op->id = ENESIM_OPERATOR_RADDIST1D;
		op->cb = tx;
		op->cpu = cpu;
		return EINA_TRUE;
	}
	else
		return EINA_FALSE;
}
