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
#ifndef ENESIM_DISPMAP_H_
#define ENESIM_DISPMAP_H_

typedef void (*Enesim_Dispmap_1D)(uint32_t *src, uint32_t spitch,
		uint32_t sw, uint32_t sh,
		float scale, uint32_t *map,
		uint32_t dx, uint32_t dy, uint32_t dlen,
		uint32_t *dst);
typedef void (*Enesim_Dispmap_2D)(uint32_t *src,
		uint32_t spitch,  uint32_t sw, uint32_t sh,
		float scale, uint32_t *map,
		uint32_t dx, uint32_t dy, uint32_t dlen,
		uint32_t *dst);

EAPI void enesim_dispmap_1d_register(Enesim_Cpu *cpu, Enesim_Dispmap_1D dm,
		Enesim_Format sfmt, Enesim_Quality q,
		Enesim_Format dfmt);
EAPI Eina_Bool enesim_dispmap_1d_op_get(Enesim_Operator *op,
		Enesim_Cpu *cpu, Enesim_Format sfmt, Enesim_Quality q,
		 Enesim_Format dfmt);


#endif /*ENESIM_DISPMAP_H_*/
