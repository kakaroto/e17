#include <stdlib.h>

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
	edata_list_append(c->readers, r);
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
