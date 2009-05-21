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

#include "_fast.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_dispmap_argb8888_init(Enesim_Cpu *cpu)
{
	/* TODO check if the cpu is the host */
	enesim_dispmap_1d_register(cpu,
			_dm_argb8888_fast_argb8888,
			ENESIM_FORMAT_ARGB8888,	ENESIM_FAST, ENESIM_FORMAT_ARGB8888);
}
