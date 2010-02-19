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
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void * enesim_pool_data_alloc(Enesim_Pool *p, Enesim_Backend be,
		Enesim_Format fmt, uint32_t w, uint32_t h)
{
	if (p->data_alloc) return p->data_alloc(p, be, fmt, w, h);
}

void enesim_pool_data_free(Enesim_Pool *p, void *data)
{
	if (p->data_free) p->data_free(p, data);
}

void enesim_pool_free(Enesim_Pool *p)
{
	if (p->free) p->free(p);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
