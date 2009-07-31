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
 * + add a surface drawer too, not only span based :)
 * + add common parameters to the renderer here like transformation matrix and quality
 * + add an x and y offset, to translate from user coords to renderer coords
 */

typedef void (*Enesim_Renderer_Span_Draw)(void *r, int x, int y, unsigned int len, uint32_t *dst);
typedef void (*Enesim_Renderer_Delete)(void *r);
typedef Eina_Bool (*Enesim_Renderer_State_Setup)(void *r);
typedef void (*Enesim_Renderer_State_Cleanup)(void *r);

struct _Enesim_Renderer
{
	int  type_id;
#ifdef DEBUG
	Enesim_Magic magic;
#endif
	Enesim_Renderer_Span_Draw span;
	Enesim_Renderer_Delete free;
	Enesim_Renderer_State_Setup state_setup;
	Enesim_Renderer_State_Cleanup state_cleanup;
	Eina_Bool changed;
	/* the origin of the paint */
	int ox, oy;
	/* a fixed point matrix */
	struct {
	    int axx, axy, axz;
	    int ayx, ayy, ayz;
	    int azx, azy, azz;
	    unsigned char is_identity : 1;
	    unsigned char is_affine : 1;
	}  matrix;
};

#define ENESIM_RENDERER_DELETE(f) ((Enesim_Renderer_Delete)(f))
#define ENESIM_RENDERER_SPAN_DRAW(f) ((Enesim_Renderer_Span_Draw)(f))
#define ENESIM_RENDERER_STATE_SETUP(f) ((Enesim_Renderer_State_Setup)(f))
#define ENESIM_RENDERER_STATE_CLEANUP(f) ((Enesim_Renderer_State_Cleanup)(f))

/* some built-in renderer type identifiers */
#define SURFACE_RENDERER (1)
//#define FIGURE_RENDERER (2)
#define IMAGE_RENDERER (3)
#define RECTANGLE_RENDERER (1 << 8)
#define CIRCLE_RENDERER (2 << 8)
#define ELLIPSE_RENDERER (3 << 8)

#endif
