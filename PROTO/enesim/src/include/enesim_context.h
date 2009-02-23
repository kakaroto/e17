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

#ifndef ENESIM_CONTEXT_H_
#define ENESIM_CONTEXT_H_

typedef struct _Enesim_Context Enesim_Context;

EAPI Enesim_Context * enesim_context_new(void);
EAPI void enesim_context_delete(Enesim_Context *c);
EAPI void enesim_context_color_set(Enesim_Context *c, uint32_t argb);
EAPI uint32_t enesim_context_color_get(Enesim_Context *c);
EAPI void enesim_context_rop_set(Enesim_Context *c, Enesim_Rop rop);

#endif /* ENESIM_CONTEXT_H_ */
