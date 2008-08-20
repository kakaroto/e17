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
#ifndef RASTERIZER_H_
#define RASTERIZER_H_

/**
 * @todo
 * - on vertex add, calculate the bounding box
 * - make each rasterizer to export what kind of scanlines it supports
 * - add a function/struct element to set up window boundaries i.e 
 * destination surface/clip
 * - add a function/struct element to set up the odd/even/zero fill rule
 * - im not sure about using a rectangle for the boundaries, maybe only
 * width and height?
 * - support passing edges directly instead of vertices (useful for
 *   subpaths)
 */

typedef void (*Enesim_Rasterizer_Vertex_Add)(void *r, float x, float y);
typedef void (*Enesim_Rasterizer_Generate)(void *r);
typedef void (*Enesim_Rasterizer_Delete)(void *r);
typedef struct _Enesim_Rasterizer_Func
{
	Enesim_Rasterizer_Vertex_Add vertex_add;
	Enesim_Rasterizer_Generate generate;
	Enesim_Rasterizer_Delete delete;
} Enesim_Rasterizer_Func;

#define ENESIM_RASTERIZER_VERTEX_ADD(func) ((Enesim_Rasterizer_Vertex_Add)func)
#define ENESIM_RASTERIZER_GENERATE(func) ((Enesim_Rasterizer_Generate)func)
#define ENESIM_RASTERIZER_DELETE(func) ((Enesim_Rasterizer_Delete)func)

struct _Enesim_Rasterizer
{
	int                      types;
	Eina_Rectangle         boundaries;
	Enesim_Scanline_Callback scanline_callback;
	Enesim_Rasterizer_Func   *funcs;
	void                     *data;
	void                     *user_data;
};

Enesim_Rasterizer * enesim_rasterizer_new(void *data, Enesim_Rasterizer_Func
	*func, Eina_Rectangle boundaries, int types);
Enesim_Rasterizer * enesim_rasterizer_kiia8_new(Eina_Rectangle boundaries);
Enesim_Rasterizer * enesim_rasterizer_kiia16_new(Eina_Rectangle boundaries);

#endif
