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

/* TODO
 * + add a way to setup and delete the state of a renderer
 * + add a dirty flag for eveyr renderer, so in case of calling a span_draw()
 * with a dirty renderable, internally we will call the create_state function
 * + add a surface drawer too, not only span based :)
 * + add common parameters to the renderer here like transformation matrix and quality
 */

typedef void (*Enesim_Renderer_Span_Draw)(void *r, int x, int y, unsigned int len, uint32_t *dst);
typedef void (*Enesim_Renderer_Delete)(void *r);

struct _Enesim_Renderer
{
#ifdef DEBUG
	Enesim_Magic magic;
#endif
	Enesim_Renderer_Span_Draw span;
	Enesim_Renderer_Delete free;
};

#define ENESIM_RENDERER_DELETE(f) ((Enesim_Renderer_Delete)(f))
#define ENESIM_RENDERER_SPAN_DRAW(f) ((Enesim_Renderer_Span_Draw)(f))

#endif
