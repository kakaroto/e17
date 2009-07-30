/*
 * eon_rect.c
 *
 *  Created on: 04-feb-2009
 *      Author: jl
 */
#include "Eon.h"
#include "eon_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Rect_Private *)((Eon_Rect *)(d))->private)
struct _Eon_Rect_Private
{

};

static void _geometry_calc(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Rect *r = (Eon_Rect *)o;
	Eina_Rectangle geom;
	Eon_Coord x, y, w, h;

	eon_square_coords_get((Eon_Square *)r, &x, &y, &w, &h);
	eina_rectangle_coords_from(&geom, x.final, y.final, w.final,
			h.final);
#ifdef EON_DEBUG
	printf("[Eon_Rect] Setting geometry of size %d %d %d %d\n",
			x.final, y.final, w.final, h.final);
#endif
	ekeko_renderable_geometry_set((Ekeko_Renderable *)r, &geom);
}

static void _render(Eon_Shape *s, Eon_Engine *eng, void *engine_data, void *canvas_data, Eina_Rectangle *clip)
{
#ifdef EON_DEBUG
	printf("[Eon_Rect] Rendering rectangle %p into canvas\n", r);
#endif
	eon_engine_rect_render(eng, engine_data, canvas_data, clip);
}

static void _ctor(void *instance)
{
	Eon_Rect *r;
	Eon_Rect_Private *prv;

	r = (Eon_Rect*) instance;
	r->private = prv = ekeko_type_instance_private_get(eon_rect_type_get(), instance);
	r->parent.parent.render = _render;
	r->parent.parent.create = eon_engine_rect_create;
	ekeko_event_listener_add((Ekeko_Object *)r, EON_SQUARE_X_CHANGED, _geometry_calc, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)r, EON_SQUARE_Y_CHANGED, _geometry_calc, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)r, EON_SQUARE_W_CHANGED, _geometry_calc, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)r, EON_SQUARE_H_CHANGED, _geometry_calc, EINA_FALSE, NULL);
}

static void _dtor(void *rect)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_RECT_X;
Ekeko_Property_Id EON_RECT_Y;
Ekeko_Property_Id EON_RECT_W;
Ekeko_Property_Id EON_RECT_H;

EAPI Ekeko_Type *eon_rect_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_RECT, sizeof(Eon_Rect),
				sizeof(Eon_Rect_Private), eon_square_type_get(),
				_ctor, _dtor, eon_shape_appendable);
	}

	return type;
}

EAPI Eon_Rect * eon_rect_new(Eon_Canvas *c)
{
	Eon_Rect *r;

	r = ekeko_type_instance_new(eon_rect_type_get());
	ekeko_object_child_append((Ekeko_Object *)c, (Ekeko_Object *)r);

	return r;
}
