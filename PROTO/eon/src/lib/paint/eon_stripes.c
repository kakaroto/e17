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
#define PRIVATE(d) ((Eon_Stripes_Private *)((Eon_Stripes *)(d))->private)

static Ekeko_Type *_type;
struct _Eon_Stripes_Private
{
	Eon_Color color1, color2;
	float thickness1, thickness2;
};

static void _render(Eon_Paint *p, Eon_Engine *eng, void *engine_data, void *canvas_data, Eina_Rectangle *clip)
{
	eon_engine_stripes_render(eng, engine_data, canvas_data, clip);
}

static void _ctor(void *instance)
{
	Eon_Stripes *sq;
	Eon_Stripes_Private *prv;

	sq = (Eon_Stripes *)instance;
	sq->private = prv = ekeko_type_instance_private_get(_type, instance);
	sq->parent.parent.create = eon_engine_stripes_create;
	sq->parent.parent.render = _render;
	sq->parent.parent.delete = eon_engine_stripes_delete;
}

static void _dtor(void *image)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_stripes_init(void)
{
	_type = ekeko_type_new(EON_TYPE_STRIPES, sizeof(Eon_Stripes),
			sizeof(Eon_Stripes_Private), eon_paint_square_type_get(),
			_ctor, _dtor, eon_paint_appendable);
	EON_STRIPES_COLOR1 = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "color1",
			EON_PROPERTY_COLOR,
			OFFSET(Eon_Stripes_Private, color1));
	EON_STRIPES_COLOR2 = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "color2",
			EON_PROPERTY_COLOR,
			OFFSET(Eon_Stripes_Private, color2));
	EON_STRIPES_THICKNESS1 = EKEKO_TYPE_PROP_SINGLE_ADD(_type,
			 "thickness1",
			EKEKO_PROPERTY_FLOAT,
			OFFSET(Eon_Stripes_Private, thickness1));
	EON_STRIPES_THICKNESS2 = EKEKO_TYPE_PROP_SINGLE_ADD(_type,
			"thickness2",
			EKEKO_PROPERTY_FLOAT,
			OFFSET(Eon_Stripes_Private, thickness2));

	eon_type_register(_type, EON_TYPE_STRIPES);
}

void eon_stripes_shutdown(void)
{
	eon_type_unregister(_type);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_STRIPES_COLOR1;
Ekeko_Property_Id EON_STRIPES_COLOR2;
Ekeko_Property_Id EON_STRIPES_THICKNESS1;
Ekeko_Property_Id EON_STRIPES_THICKNESS2;

EAPI Eon_Stripes * eon_stripes_new(Eon_Document *d)
{
	Eon_Stripes *s;

	s = eon_document_object_new(d, EON_TYPE_STRIPES);

	return s;
}

EAPI void eon_stripes_thickness1_set(Eon_Stripes *s, float th)
{
	Ekeko_Value v;

	ekeko_value_float_from(&v, th);
	ekeko_object_property_value_set((Ekeko_Object *)s, "thickness1", &v);
}

EAPI float eon_stripes_thickness1_get(Eon_Stripes *s)
{
	Eon_Stripes_Private *prv = PRIVATE(s);

	return prv->thickness1;
}

EAPI void eon_stripes_thickness2_set(Eon_Stripes *s, float th)
{
	Ekeko_Value v;

	ekeko_value_float_from(&v, th);
	ekeko_object_property_value_set((Ekeko_Object *)s, "thickness2", &v);
}

EAPI float eon_stripes_thickness2_get(Eon_Stripes *s)
{
	Eon_Stripes_Private *prv = PRIVATE(s);

	return prv->thickness2;
}

EAPI Eon_Color eon_stripes_color1_get(Eon_Stripes *s)
{
	Eon_Stripes_Private *prv = PRIVATE(s);

	return prv->color1;
}

EAPI Eon_Color eon_stripes_color2_get(Eon_Stripes *s)
{
	Eon_Stripes_Private *prv = PRIVATE(s);

	return prv->color2;
}

EAPI void eon_stripes_color1_set(Eon_Stripes *s, Eon_Color color)
{
	Ekeko_Value v;

	eon_value_color_from(&v, color);
	ekeko_object_property_value_set((Ekeko_Object *)s, "color1", &v);
}

EAPI void eon_stripes_color2_set(Eon_Stripes *s, Eon_Color color)
{
	Ekeko_Value v;

	eon_value_color_from(&v, color);
	ekeko_object_property_value_set((Ekeko_Object *)s, "color2", &v);
}
