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
Enesim_Container * enesim_container_new(void *data, int vertices_ref)
{
	Enesim_Container *p;

	p = calloc(1, sizeof(Enesim_Container));
	p->data = data;
	p->a = edata_array_new(p, EDATA_ARRAY_ALLOC(_a_alloc),
		EDATA_ARRAY_FREE(_a_free));
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
	edata_array_element_new(p->a);
	p->point_curr->x = x;
	p->point_curr->y = y;
	*p->cmd_curr = cmd;
	p->num_vertices++;
	p->point_curr++;
	p->cmd_curr++;
}
