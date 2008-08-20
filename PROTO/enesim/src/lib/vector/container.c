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
#include "enesim_common.h"
#include "Enesim.h"
#include "enesim_private.h"
#include "container.h"

/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
static void _a_alloc(Enesim_Container *p, int num)
{
	p->points = realloc(p->points, sizeof(Enesim_Point) * num);
	p->cmds = realloc(p->cmds, sizeof(char) * num);
	/* FIXME only add the offset and call the alloc_cb in case the pointer
	 * has changed */
	/* in case the realloc returns a different pointer */
	p->point_curr = p->points + p->num_vertices;
	p->cmd_curr = p->cmds + p->num_vertices;
	/* every time we alloc new data call the alloc_cb */
	p->alloc_cb(p->data);
}

static void _a_free(Enesim_Container *p)
{
	free(p->points);
	free(p->cmds);
}
/*============================================================================*
 *                                 Global                                     * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
Enesim_Container * enesim_container_new(void *data, int num_vertices)
{
	Enesim_Container *p;

	p = calloc(1, sizeof(Enesim_Container));
	p->data = data;
	p->a = eina_array_new(p, EINA_ARRAY_ALLOC(_a_alloc),
		EINA_ARRAY_FREE(_a_free), num_vertices);
	return p;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
void * enesim_container_delete(Enesim_Container *p)
{
	free(p->points);
	free(p->cmds);
	
	return p->data;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
void enesim_container_vertex_add(Enesim_Container *p, float x, float y, char cmd)
{
	eina_array_element_add(p->a);
	p->point_curr->x = x;
	p->point_curr->y = y;
	*p->cmd_curr = cmd;
	p->num_vertices++;
	p->point_curr++;
	p->cmd_curr++;
}
