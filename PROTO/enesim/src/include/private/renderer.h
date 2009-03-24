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

typedef Enesim_Renderer_Span (*Enesim_Renderer_Get)(Enesim_Renderer *r, Enesim_Format *f);
typedef void (*Enesim_Renderer_Free)(Enesim_Renderer *r);

struct _Enesim_Renderer
{
#ifdef DEBUG
	Enesim_Magic magic;
#endif
	Enesim_Renderer_Get get;
	Enesim_Renderer_Free free;
};

#define ENESIM_RENDERER_GET(f) ((Enesim_Renderer_Get)(f))
#define ENESIM_RENDERER_FREE(f) ((Enesim_Renderer_Free)(f))
#define ENESIM_RENDERER_SPAN(f) ((Enesim_Renderer_Span)(f))

Enesim_Renderer * enesim_renderer_new(void);

#endif
