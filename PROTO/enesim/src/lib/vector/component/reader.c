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
#include "enesim_vector.h"
/**
 * TODO
 * flat a pipeline
 */

/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
static inline void _rewind(Enesim_Component_Reader *r)
{
	r->points = r->from->path->points;
	r->cmds = r->from->path->cmds;
	r->pos = 0;
}
/*============================================================================*
 *                                 Global                                     * 
 *============================================================================*/
/**
 *
 */
void enesim_reader_notify(Enesim_Component_Reader *r)
{
	/* FIXME update the pointers as the component might have had to
	 * realloc its data */
	if (r->to && !r->to->has_changed)
	{
		enesim_component_notify(r->to);
	}
}

/**
 * called when the path of a component has realloced its data 
 */
void enesim_reader_reference_update(Enesim_Component_Reader *r)
{
	r->points = r->from->path->points + r->pos;
	r->cmds = r->from->path->cmds + r->pos;
}

/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Component_Reader * enesim_reader_new(Enesim_Component *c)
{
	Enesim_Component_Reader *r;

	r = calloc(1, sizeof(Enesim_Component_Reader));
	r->from = c;
	_rewind(r);
	/* append the reader to the list of readers,
	 * if we add a reader to a component that has changed
	 * propagate that change to the other end of the reader
	 */
	eina_list_append(c->readers, r);
	return r;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_reader_rewind(Enesim_Component_Reader *r)
{
	_rewind(r);
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI int enesim_reader_vertex_get(Enesim_Component_Reader *r, float *x, float *y)
{
	int ret;
	int num = 1;

	assert(r->from);
	if (r->pos >= r->from->path->num_vertices)
	{
		if (!enesim_component_generate(r->from, &num))
			return ENESIM_CMD_END;
		if (num < 1)
			return ENESIM_CMD_END;
	}
	*x = r->points->x;
	*y = r->points->y;
	ret = *r->cmds;
	r->points++;
	r->pos++;
	r->cmds++;
	return ret;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI int enesim_reader_vertices_get(Enesim_Component_Reader *r, float *x, float *y, int *cmds)
{

  /* for now, return 0 */
  return 0;
}
