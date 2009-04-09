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
#if 0
/* check that the ydst is actually inside the destination area */
static inline Eina_Bool _check_ydst(Renderer_Repeater *f, int ydst)
{
	if ((ydst < f->dst.area.y) || (ydst > (f->dst.area.y + f->dst.area.h)))
		return EINA_FALSE;
	else
		return EINA_TRUE;
}
/* given a destination y retrieve the origin y based on origin area y and
 * origin area h
 */
static inline Eina_Bool _get_y(Renderer_Repeater *f, int ydst, int *ysrc, int say, int sah)
{
	if (f->mode & ENESIM_REPEATER_DIRECTION_Y)
	{
		*ysrc = (ydst - f->dst.area.y) % sah;
		return EINA_TRUE;
	}
	else
	{
		if (ydst > (f->dst.area.y + sah))
			return EINA_FALSE;
		else
		{
			*ysrc = say + (ydst - f->dst.area.y);
			return EINA_TRUE;
		}
	}
}
#endif
typedef Enesim_Repeater Enesim_Repeater_Lut[ENESIM_FORMATS][ENESIM_DIRECTIONS][ENESIM_FORMATS];

Enesim_Repeater_Lut *_repeaters;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_repeater_init(void)
{
	int numcpu;

	enesim_cpu_get(&numcpu);
	_repeaters = malloc(sizeof(Enesim_Repeater_Lut) * numcpu);
}
void enesim_repeater_shutdown(void)
{
	free(_repeaters);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void enesim_repeater_register(Enesim_Cpu *cpu, Enesim_Repeater rep,
		Enesim_Format sfmt, Enesim_Direction dir,
		Enesim_Format dfmt)
{
	unsigned int cpuid;
	Enesim_Repeater_Lut *t;

	cpuid = enesim_cpu_id_get(cpu);
	t = &_repeaters[cpuid];
	*t[sfmt][dir][dfmt] = rep;
}

EAPI Eina_Bool enesim_repeater_op_get(Enesim_Operator *op,
		Enesim_Cpu *cpu, Enesim_Format sfmt,
		Enesim_Repeater_Direction dir, Enesim_Format dfmt)
{
	unsigned int cpuid;
	Enesim_Repeater_Lut *t;
	Enesim_Repeater r;

	cpuid = enesim_cpu_id_get(cpu);
	t = &_repeaters[cpuid];
	r = *t[sfmt][dir][dfmt];
	if (r)
	{
		op->id = ENESIM_OPERATOR_REPEATER;
		op->cb = r;
		op->cpu = cpu;
		return EINA_TRUE;
	}
	else
		return EINA_FALSE;
}
