/* EON - Canvas and Toolkit library
 * Copyright (C) 2008-2009 Jorge Luis Zapata
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
#include "Eon.h"
#include "eon_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Rect_Private *)((Eon_Rect *)(d))->private)

static Ekeko_Type *_type;
struct _Eon_Rect_Private
{
	float radius;
};

/* FIXME this can be go away */
static void _geometry_calc(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Rect *r = (Eon_Rect *)o;
	Eina_Rectangle geom;
	Eon_Coord x, y, w, h;

	eon_shape_square_coords_get((Eon_Shape_Square *)r, &x, &y, &w, &h);
	eina_rectangle_coords_from(&geom, x.final, y.final, w.final,
			h.final);
#ifdef EON_DEBUG
	printf("[Eon_Rect] Setting geometry of size %d %d %d %d\n",
			x.final, y.final, w.final, h.final);
#endif
	eon_paint_geometry_set((Eon_Shape *)r, &geom);
}

static void _render(Eon_Paint *p, Eon_Engine *eng, void *engine_data, void *canvas_data, Eina_Rectangle *clip)
{
#ifdef EON_DEBUG
	printf("[Eon_Rect] Rendering rectangle %p into canvas\n", r);
#endif
	eon_engine_rect_render(eng, engine_data, canvas_data, clip);
}

static Eina_Bool _is_inside(Eon_Paint *p, int x, int y)
{
	Eon_Shape *s = (Eon_Shape *)p;
	Eon_Rect *r = (Eon_Rect *)p;
	Eon_Rect_Private *prv;
	Enesim_Shape_Draw_Mode mode;
	Eon_Coord cx, cy, cw, ch;
	Eina_Rectangle rrect;
	Eina_Rectangle point;

	eon_shape_square_coords_get((Eon_Shape_Square *)s, &cx, &cy, &cw, &ch);

	/* TODO handle the rounded corners */
	mode = eon_shape_draw_mode_get(s);
	if (mode == ENESIM_SHAPE_DRAW_MODE_STROKE)
	{
		float sw;

		sw = eon_shape_stroke_width_get(s);

		if (x > (cx.final + sw) && (x < cx.final + cw.final - sw) &&
				y > (cy.final + sw) &&
				y < (cy.final + ch.final + sw))
			return EINA_FALSE;
		else
			return EINA_TRUE;
	}
	else
	{
		Enesim_Matrix_Type mtype;
		Enesim_Matrix m;

		eon_paint_matrix_get(p, &m);
        	mtype = enesim_matrix_type_get(&m);
	        if (mtype == ENESIM_MATRIX_IDENTITY)
			return EINA_TRUE;

	}
        /* we only handle the special case for a non-identity
	 * transformation, the bounding box of a rect non transformed
	 * is the same rect
	 */

	eina_rectangle_coords_from(&rrect, cx.final, cy.final,
			cw.final, ch.final);
	eina_rectangle_coords_from(&point, x, y, 1, 1);

	if (!eina_rectangles_intersect(&rrect, &point))
		return EINA_FALSE;

	return EINA_TRUE;
}

static void _ctor(void *instance)
{
	Eon_Rect *r;
	Eon_Rect_Private *prv;

	r = (Eon_Rect*) instance;
	r->private = prv = ekeko_type_instance_private_get(_type, instance);
	r->parent.parent.parent.render = _render;
	r->parent.parent.parent.create = eon_engine_rect_create;
	r->parent.parent.parent.is_inside = _is_inside;
	/* events */
	ekeko_event_listener_add((Ekeko_Object *)r, EON_SHAPE_SQUARE_X_CHANGED, _geometry_calc, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)r, EON_SHAPE_SQUARE_Y_CHANGED, _geometry_calc, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)r, EON_SHAPE_SQUARE_W_CHANGED, _geometry_calc, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)r, EON_SHAPE_SQUARE_H_CHANGED, _geometry_calc, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)r, EON_PAINT_MATRIX_CHANGED, _geometry_calc, EINA_FALSE, NULL);
}

static void _dtor(void *rect)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_rect_init(void)
{
	_type = ekeko_type_new(EON_TYPE_RECT, sizeof(Eon_Rect),
			sizeof(Eon_Rect_Private), eon_shape_square_type_get(),
			_ctor, _dtor, eon_shape_appendable);
	/* the properties */
	EON_RECT_CORNER_RADIUS = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "radius",
			EKEKO_PROPERTY_FLOAT,
			OFFSET(Eon_Rect_Private, radius));

	eon_type_register(_type, EON_TYPE_RECT);
}

void eon_rect_shutdown(void)
{
	eon_type_unregister(_type);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_RECT_CORNERS;
Ekeko_Property_Id EON_RECT_CORNER_RADIUS;

EAPI Eon_Rect * eon_rect_new(Eon_Document *d)
{
	Eon_Rect *r;

	r = eon_document_object_new(d, EON_TYPE_RECT);

	return r;
}

EAPI void eon_rect_corner_radius_set(Eon_Rect *r, float rad)
{
	Ekeko_Value v;

	ekeko_value_float_from(&v, rad);
	ekeko_object_property_value_set((Ekeko_Object *)r, "radius", &v);
}

EAPI float eon_rect_corner_radius_get(Eon_Rect *r)
{
	Eon_Rect_Private *prv = PRIVATE(r);

	return prv->radius;
}

