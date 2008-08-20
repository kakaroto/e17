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
#ifndef _PATH_H
#define _PATH_H

typedef struct _Enesim_Container 	Enesim_Container; /**< */
typedef struct _Enesim_Point 	Enesim_Point; /**< */

/**
 *
 */
struct _Enesim_Point
{
	float x;
	float y;
};

/**
 *
 */
struct _Enesim_Container
{
	Eina_Array	 *a;
	Enesim_Point *points;
	char         *cmds;
	Enesim_Point *point_curr;
	char         *cmd_curr;
	int          num_vertices;

	void         *data;
	void        (*alloc_cb)(void *data);
};

Enesim_Container * 	enesim_container_new(void *data, int vertices_ref);
void * 		enesim_container_delete(Enesim_Container *p);
void 		enesim_container_vertex_add(Enesim_Container *p, float x, float y, char cmd);

#endif
