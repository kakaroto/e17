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
#define PRIVATE(d) ((Eon_Text_Private *)((Eon_Text *)(d))->private)
struct _Eon_Text_Private
{
	Eon_Coord x, y;
	char *str;
	int strw; /* the width of the string on pixels */
	int strh; /* the height of the string on pixels */
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
	/* TODO get the min between the user's x, y and the strw/h */
	ekeko_renderable_geometry_set((Ekeko_Renderable *)r, &geom);
}

static void _str_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	/* FIXME get the bounding box of the text */
	_geometry_calc(o, e, data);
}

static void _render(Eon_Shape *s, Eon_Engine *eng, void *engine_data, void *canvas_data, Eina_Rectangle *clip)
{
	Eon_Text *t;
	Eon_Text_Private *prv;

	t = (Eon_Text *)s;

#ifdef EON_DEBUG
	printf("[Eon_Text] Rendering text %p into canvas %p\n", t, c);
#endif
	eon_engine_text_render(eng, engine_data, canvas_data, clip);
}

static void _ctor(void *instance)
{
	Eon_Text *t;
	Eon_Text_Private *prv;

	t = (Eon_Text *) instance;
	t->private = prv = ekeko_type_instance_private_get(eon_text_type_get(), instance);
	t->parent.parent.parent.render = _render;
	t->parent.parent.parent.create = eon_engine_text_create;
	/* events */
	ekeko_event_listener_add((Ekeko_Object *)t, EON_SQUARE_X_CHANGED, _geometry_calc, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)t, EON_SQUARE_Y_CHANGED, _geometry_calc, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)t, EON_TEXT_STR_CHANGED, _str_change, EINA_FALSE, NULL);
}

static void _dtor(void *instance)
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
Ekeko_Property_Id EON_TEXT_STR;

EAPI Ekeko_Type *eon_text_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_TEXT, sizeof(Eon_Text),
				sizeof(Eon_Text_Private), eon_square_type_get(),
				_ctor, _dtor, _appendable);
		EON_TEXT_STR = EKEKO_TYPE_PROP_SINGLE_ADD(type, "str", EKEKO_PROPERTY_STRING, OFFSET(Eon_Text_Private, str));
	}

	return type;
}

EAPI Eon_Text * eon_text_new(Eon_Canvas *c)
{
	Eon_Text *t;

	t = ekeko_type_instance_new(eon_text_type_get());
	ekeko_object_child_append((Ekeko_Object *)c, (Ekeko_Object *)t);

	return t;
}

EAPI void eon_text_string_set(Eon_Text *t, const char *str)
{
	Ekeko_Value v;

	ekeko_value_str_from(&v, str);
	ekeko_object_property_value_set((Ekeko_Object *)t, "str", &v);
}

EAPI const char * eon_text_string_get(Eon_Text *t)
{
	Eon_Text_Private *prv;

	prv = PRIVATE(t);
	return prv->str;
}

EAPI void eon_text_x_rel_set(Eon_Text *t, int x)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, x, EON_COORD_RELATIVE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)t, "x", &v);
}

EAPI void eon_text_x_set(Eon_Text *t, int x)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, x, EON_COORD_ABSOLUTE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)t, "x", &v);
}

EAPI void eon_text_y_set(Eon_Text *t, int y)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, y, EON_COORD_ABSOLUTE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)t, "y", &v);
}

EAPI void eon_text_y_rel_set(Eon_Text *t, int y)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, y, EON_COORD_RELATIVE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)t, "y", &v);
}
