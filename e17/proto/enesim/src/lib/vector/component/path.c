#include <stdlib.h>

#include "Enesim.h"
#include "enesim_private.h"
#include "enesim_vector.h"

static const char _name[] = "path";

typedef struct _Path
{
	Enesim_Component *c;
} Path;

/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/

static void enesim_path_generate(void *data, int *num)
{
	Path *d = data;
	
}

static void enesim_path_free(void *data)
{
	Path *d = data;

	free(d);
}

static void enesim_path_init(Enesim_Component *c)
{
	Path *d;

	d = calloc(1, sizeof(Path));

	d->c = c;
	c->data = d;
	c->name = _name;
	c->type = ENESIM_COMPONENT_O;
	c->generate = enesim_path_generate;
	c->free = enesim_path_free;
}
/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Component * enesim_path_new(int num_vertices)
{
	Enesim_Component *c;

	c = enesim_component_new(num_vertices);
	enesim_path_init(c);
	return c;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_path_move_to(Enesim_Component *p, int x, int y)
{
	enesim_container_vertex_add(p->path, x, y, ENESIM_CMD_MOVE_TO);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_path_line_to(Enesim_Component *p, int x, int y)
{
	enesim_container_vertex_add(p->path, x, y, ENESIM_CMD_LINE_TO);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_path_close(Enesim_Component *p)
{
	enesim_container_vertex_add(p->path, 0, 0, ENESIM_CMD_END);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_path_curve3(Enesim_Component *p, float x1, float y1, float x2,
	float y2, float x3, float y3)
{
	enesim_container_vertex_add(p->path, x1, y1, ENESIM_CMD_CURVE3);
	enesim_container_vertex_add(p->path, x2, y2, ENESIM_CMD_CURVE3);
	enesim_container_vertex_add(p->path, x3, y3, ENESIM_CMD_CURVE3);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_path_curve4(Enesim_Component *p, float x1, float y1, float x2,
	float y2, float x3, float y3, float x4, float y4)
{
	enesim_container_vertex_add(p->path, x1, y1, ENESIM_CMD_CURVE4);
	enesim_container_vertex_add(p->path, x2, y2, ENESIM_CMD_CURVE4);
	enesim_container_vertex_add(p->path, x3, y3, ENESIM_CMD_CURVE4);
	enesim_container_vertex_add(p->path, x4, y4, ENESIM_CMD_CURVE4);
}
