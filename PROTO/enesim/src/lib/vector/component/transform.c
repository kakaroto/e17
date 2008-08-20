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
 * @file
 * @brief
 * @defgroup Transform_Internal_Group Transform
 * @ingroup Enesim_Internal_Group
 * @todo make sse optimized implementation
 * @{
 */

/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
static const char _name[] = "transform";

typedef struct _Transform
{
	Enesim_Component 	*c;
	float 			m[4];
} Transform;

static void enesim_transform_generate(void *data, int *num)
{
	Transform *d = data;
	float x, y;
	int cmd, i = 0;
	
	while ((i < *num) && 
		((cmd = enesim_reader_vertex_get(d->c->src, &x, &y)) != ENESIM_CMD_END))
	{
		float x2, y2;

		x2 = (x * d->m[0]) + (y * d->m[1]);
		y2 = (x * d->m[2]) + (y * d->m[3]);
		enesim_container_vertex_add(d->c->path, x2, y2, cmd);
		i++;
	}
	/* last vertex we generate must be an END command */
	if (i < *num)
	{
		enesim_container_vertex_add(d->c->path, 0, 0, ENESIM_CMD_END);
	}
	*num = i;
}

static void enesim_transform_free(void *data)
{
	Transform *d = data;

	free(d);
}

static void enesim_transform_init(Enesim_Component *c)
{
	Transform *d;

	d = calloc(1, sizeof(Transform));

	d->c = c;
	/* identity matrix on init */
	d->m[0] = 1;
	d->m[1] = 0;
	d->m[2] = 0;
	d->m[3] = 1;
	c->data = d;
	c->name = _name;
	c->type = ENESIM_COMPONENT_IO;
	c->generate = enesim_transform_generate;
	c->free = enesim_transform_free;
}
/** @} */
/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Component * enesim_transform_new(void)
{
	Enesim_Component *c;

	c = enesim_component_new(0);
	enesim_transform_init(c);
	return c;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_transform_matrix_set(Enesim_Component *c, float *matrix)
{
	Transform *d;

	d = c->data;
	d->m[0] = matrix[0];
	d->m[1] = matrix[1];
	d->m[2] = matrix[2];
	d->m[3] = matrix[3];
	enesim_component_notify(c);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_transform_matrix_rotate_get(Enesim_Component *c, float t, float *matrix)
{

}

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_transform_matrix_scale_get(Enesim_Component *c, float sx, float sy, float *matrix)
{

}
