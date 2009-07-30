/*
 * eon_polygon.c
 */
#include "Eon.h"
#include "eon_private.h"
#include <limits.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Polygon_Private *)((Eon_Polygon *)(d))->private)
struct _Eon_Polygon_Private
{
	int xmin, xmax, ymin, ymax; /* bounding box */
	/* our list of points, an engine might want to have its own */
	Eina_List *points;
	void *engine_data;
};

typedef struct _Polygon_Coord
{
	int x, y;
} Polygon_Coord;

/*
 * Called whenever a point has been added/removed/set???
 * Re-calc the bounding box of the polygon
 */
//static void _point_appended_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
static void _update_geometry(Eon_Polygon *p)
{
	Eon_Polygon_Private *prv = PRIVATE(p);
	Eina_Rectangle geom;

	eina_rectangle_coords_from(&geom, prv->xmin, prv->ymin, prv->xmax - prv->xmin,  prv->ymax - prv->ymin);
	ekeko_renderable_geometry_set((Ekeko_Renderable *)p, &geom);
}


/*
 * When the polygon has been appended to a canvas get the engine
 * and call polygon_new(), if the polygon has points call
 * polygon_point_add()
 */
static void _appended_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Object *parent;
	Eon_Polygon_Private *prv = PRIVATE(o);

	if (!prv->points)
		return;

	if ((parent = ekeko_object_parent_get(o)))
	{
		Eon_Engine *eng;
		Eon_Document *d;
		Eina_Iterator *it;
		Polygon_Coord coord;
		void *ed;

		d = eon_canvas_document_get((Eon_Canvas *) parent);
		if (!d)
			return;

		eng = eon_document_engine_get(d);
		/* iterate over the list of points and add them to the engine */

		ed = eon_shape_engine_data_get((Eon_Shape *)o);
		it = eina_list_iterator_new(prv->points);
		while (eina_iterator_next(it, (void **)&coord))
		{
			eon_engine_polygon_point_add(eng, ed, coord.x, coord.y);
		}
	}
}
/*
 * When the quality of the shape has changed, check if we have a polygon
 * if so, delete it, create a new one and re-append the points
 */
static void _quality_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{

}

static void _render(Eon_Shape *s, Eon_Engine *eng, void *engine_data, void *canvas_data, Eina_Rectangle *clip)
{
#ifdef EON_DEBUG
	Eon_Polygon *p;

	p = (Eon_Polygon *)s;
	printf("[Eon_Polygon] Rendering polygon %p", p);
#endif
	eon_engine_polygon_render(eng, engine_data, canvas_data, clip);
}

static void _ctor(void *instance)
{
	Eon_Polygon *p;
	Eon_Polygon_Private *prv;

	p = (Eon_Polygon*) instance;
	p->private = prv = ekeko_type_instance_private_get(eon_polygon_type_get(), instance);
	p->parent.render = _render;
	p->parent.create = eon_engine_polygon_create;
	/* setup the limits */
	prv->xmin = prv->ymin = INT_MAX;
	prv->xmax = prv->ymax = INT_MIN;
	ekeko_event_listener_add((Ekeko_Object *)p, EKEKO_EVENT_OBJECT_APPEND, _appended_cb, EINA_FALSE, NULL);
}

static void _dtor(void *polygon)
{

}

static Eina_Bool _appendable(void *instance, void *child)
{
	if (!ekeko_type_instance_is_of(child, EON_TYPE_ANIMATION))
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ekeko_Type *eon_polygon_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_POLYGON, sizeof(Eon_Polygon),
				sizeof(Eon_Polygon_Private), eon_shape_type_get(),
				_ctor, _dtor, eon_shape_appendable);
	}

	return type;
}

EAPI Eon_Polygon * eon_polygon_new(Eon_Canvas *c)
{
	Eon_Polygon *p;

	p = ekeko_type_instance_new(eon_polygon_type_get());
	ekeko_object_child_append((Ekeko_Object *)c, (Ekeko_Object *)p);

	return p;
}

EAPI void eon_polygon_point_add(Eon_Polygon *p, int x, int y)
{
	Eon_Polygon_Private *prv;
	Polygon_Coord *point;
	Ekeko_Object *parent;

	prv = PRIVATE(p);
	point = malloc(sizeof(Polygon_Coord));
	point->x = x;
	point->y = y;

	if (x < prv->xmin)
		prv->xmin = x;
	else if (x > prv->xmax)
		prv->xmax = x;
	if (y < prv->ymin)
		prv->ymin = y;
	else if (y > prv->ymax)
		prv->ymax = y;

	prv->points = eina_list_append(prv->points, point);
	if ((parent = ekeko_object_parent_get(p)))
	{
		Eon_Document *d;
		Eon_Engine *eng;

		d = eon_canvas_document_get((Eon_Canvas *) parent);
		if (!d)
			return;
		eng = eon_document_engine_get(d);
		eon_engine_polygon_point_add(eng, eon_shape_engine_data_get((Eon_Shape *)p), x, y);
		_update_geometry(p);
	}
}

EAPI void eon_polygon_clear(Eon_Polygon *p)
{
	/* TODO */
}
