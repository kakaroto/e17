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

typedef void (*Enesim_Rasterizer_Vertex_Add)(void *d, float x, float y);
typedef void (*Enesim_Rasterizer_Generate)(void *r, Eina_Rectangle *rect, Enesim_Scanline_Callback cb, void *data);
typedef void (*Enesim_Rasterizer_Reset)(void *r);
typedef void (*Enesim_Rasterizer_Delete)(void *r);

#define ENESIM_RASTERIZER_VERTEX_ADD(func) ((Enesim_Rasterizer_Vertex_Add)func)
#define ENESIM_RASTERIZER_GENERATE(func) ((Enesim_Rasterizer_Generate)func)
#define ENESIM_RASTERIZER_DELETE(func) ((Enesim_Rasterizer_Delete)func)
#define ENESIM_RASTERIZER_RESET(func) ((Enesim_Rasterizer_Reset)func)

struct _Enesim_Rasterizer
{
	EINA_MAGIC;
	Enesim_Rasterizer_Vertex_Add vertex_add;
	Enesim_Rasterizer_Generate generate;
	Enesim_Rasterizer_Delete delete;
	Enesim_Rasterizer_Reset reset;
	void *data;
};

void enesim_rasterizer_init(Enesim_Rasterizer *r);
Enesim_Rasterizer * enesim_rasterizer_kiia8_new(Eina_Rectangle boundaries);
Enesim_Rasterizer * enesim_rasterizer_kiia16_new(Eina_Rectangle boundaries);

#endif
