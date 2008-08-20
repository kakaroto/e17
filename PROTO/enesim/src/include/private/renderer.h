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
#ifndef RENDERER_H_
#define RENDERER_H_

/**
 *
 * @todo
 * - move this interface to a new one, remove the scanline ptr and use
 *   the type instead
 */

typedef struct _Enesim_Renderer_Func
{
	Eina_Bool (*draw)(Enesim_Renderer *r, int type, void *sl, Enesim_Surface *dst);
	void (*free)(Enesim_Renderer *r);
} Enesim_Renderer_Func;

struct _Enesim_Renderer
{
	int rop;
#ifdef DEBUG
	Enesim_Magic magic;
#endif
	void *data;
	Enesim_Renderer_Func *funcs;
};

Enesim_Renderer * enesim_renderer_new(void);

#endif
