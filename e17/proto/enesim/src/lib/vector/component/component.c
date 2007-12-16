#include <stdlib.h>

#include "Enesim.h"
#include "enesim_private.h"
#include "enesim_vector.h"

/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
/* called whenever the pointer to the path has changed */
static void _alloc_cb(void *data)
{
	Enesim_Component_Reader *r;
	Enesim_Component *c = data;
	
	edata_list_first_goto(c->readers);
	while ((r = edata_list_next(c->readers)))
	{
		enesim_reader_reference_update(r);
	}
}
/*============================================================================*
 *                                 Global                                     * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
Enesim_Component * enesim_component_new(int num_vertices)
{
	Enesim_Component *c;

	c = calloc(1, sizeof(Enesim_Component));
	c->readers = edata_list_new();
	c->path = enesim_container_new(c, num_vertices);
	/* ABSTRACT THIS */
	c->path->alloc_cb = _alloc_cb;
	return c;
}

/**
 * To be documented
 * FIXME: To be fixed
 */
void enesim_component_notify(Enesim_Component *c)
{
	Enesim_Component_Reader *r;
	
	if (c->has_changed) return;
	
	c->has_changed = 1;
	edata_list_first_goto(c->readers);
	while ((r = edata_list_next(c->readers)))
	{
		enesim_reader_notify(r);
	}
}

/**
 * To be documented
 * FIXME: To be fixed
 */
/* should create maximum num vertices */
int enesim_component_generate(Enesim_Component *c, int *num)
{
	/* if the component is of type input-ouput and we dont
	 * have a source of vertices, just return */
	if ((c->type == ENESIM_COMPONENT_IO) && (!c->src))
	{
		return 0;
	}
	if (c->generate)
	{
		c->generate(c->data, num);
		return 1;
	}
	return 0;
}

/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_component_delete(Enesim_Component *c)
{
	if (c->free)
		c->free(c->data);
	/* TODO what about all the readers attached */
	free(c);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI int enesim_component_source_set(Enesim_Component *c, Enesim_Component *src)
{
	assert(c);
	assert(src);

	/* output component only */
	if (c->type == ENESIM_COMPONENT_O)
		return -ENESIM_ERROR_INVAL;
	/* same source, do nothing */
	if ((c->src) && (c->src->from == src))
		return ENESIM_ERROR_NONE;
	if (!c->src)
		c->src = enesim_reader_new(src);
	/* notify the change */
	enesim_component_notify(c);

	return ENESIM_ERROR_NONE;
}
