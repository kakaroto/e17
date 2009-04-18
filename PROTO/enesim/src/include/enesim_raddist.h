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
#ifndef ENESIM_RADDIST_H_
#define ENESIM_RADDIST_H_

typedef void (*Enesim_Raddist_1D)(uint32_t *src, uint32_t spitch,
		uint32_t sw, uint32_t sh,
		float x0, float y0, float r0, float scale,
		uint32_t dx, uint32_t dy, uint32_t dlen,
		uint32_t *dst);
typedef void (*Enesim_Raddist_2D)(uint32_t *src,
		uint32_t spitch,  uint32_t sw, uint32_t sh,
		float ox, float oy,
		float xx, float xy, float xz,
		float yx, float yy, float yz,
		float zx, float zy, float zz,
		uint32_t dpitch,  uint32_t dw, uint32_t dh,
		uint32_t *dst);

EAPI void enesim_raddist_1d_register(Enesim_Cpu *cpu, Enesim_Raddist_1D tx,
		Enesim_Format sfmt, Enesim_Quality q,
		Enesim_Format dfmt);
EAPI Eina_Bool enesim_raddist_1d_op_get(Enesim_Operator *op,
		Enesim_Cpu *cpu, Enesim_Format sfmt, Enesim_Quality q,
		 Enesim_Format dfmt);


#endif /*ENESIM_RADDIST_H_*/
