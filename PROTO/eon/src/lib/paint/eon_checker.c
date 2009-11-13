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
#define PRIVATE(d) ((Eon_Checker_Private *)((Eon_Checker *)(d))->private)

static Ekeko_Type * _type;
struct _Eon_Checker_Private
{
	Eon_Coord w, h;
	Eon_Color color1, color2;
};

static void _render(Eon_Paint *p, Eon_Engine *eng, void *engine_data, void *canvas_data, Eina_Rectangle *clip)
{
	eon_engine_checker_render(eng, engine_data, canvas_data, clip);
}

static void _ctor(void *instance)
{
	Eon_Checker *ch;
	Eon_Checker_Private *prv;

	ch = (Eon_Checker *)instance;
	ch->private = prv = ekeko_type_instance_private_get(_type, instance);
	ch->parent.parent.create = eon_engine_checker_create;
	ch->parent.parent.free = eon_engine_checker_delete;
	ch->parent.parent.render = _render;
}

static void _dtor(void *image)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_checker_init(void)
{
	_type = ekeko_type_new(EON_TYPE_CHECKER, sizeof(Eon_Checker),
			sizeof(Eon_Checker_Private), eon_paint_square_type_get(),
			_ctor, _dtor, eon_paint_appendable);
	EON_CHECKER_COLOR1 = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "color1",
			EON_PROPERTY_COLOR,
			OFFSET(Eon_Checker_Private, color1));
	EON_CHECKER_COLOR2 = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "color2",
			EON_PROPERTY_COLOR,
			OFFSET(Eon_Checker_Private, color2));
	EON_CHECKER_SW = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "sw",
			EON_PROPERTY_COORD,
			OFFSET(Eon_Checker_Private, w));
	EON_CHECKER_SH = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "sh",
			EON_PROPERTY_COORD,
			OFFSET(Eon_Checker_Private, h));

	eon_type_register(_type, EON_TYPE_CHECKER);
}

void eon_checker_shutdown(void)
{
	eon_type_unregister(_type);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_CHECKER_SW;
Ekeko_Property_Id EON_CHECKER_SH;
Ekeko_Property_Id EON_CHECKER_COLOR1;
Ekeko_Property_Id EON_CHECKER_COLOR2;

EAPI Eon_Checker * eon_checker_new(Eon_Document *d)
{
	Eon_Checker *ch;

	ch = eon_document_object_new(d, EON_TYPE_CHECKER);

	return ch;
}

EAPI Eon_Color eon_checker_color1_get(Eon_Checker *sq)
{
	Eon_Checker_Private *prv = PRIVATE(sq);

	return prv->color1;
}

EAPI Eon_Color eon_checker_color2_get(Eon_Checker *sq)
{
	Eon_Checker_Private *prv = PRIVATE(sq);

	return prv->color2;
}

EAPI void eon_checker_color1_set(Eon_Checker *sq, Eon_Color color)
{
	Ekeko_Value v;

	eon_value_color_from(&v, color);
	ekeko_object_property_value_set((Ekeko_Object *)sq, "color1", &v);
}

EAPI void eon_checker_color2_set(Eon_Checker *sq, Eon_Color color)
{
	Ekeko_Value v;

	eon_value_color_from(&v, color);
	ekeko_object_property_value_set((Ekeko_Object *)sq, "color2", &v);
}
