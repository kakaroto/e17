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
#define PRIVATE(d) ((Eon_Grid_Private *)((Eon_Grid *)(d))->prv)

static Ekeko_Type * _type;
struct _Eon_Grid_Private
{
	Eon_Color color1, color2;
	Eon_Coord hthick, vthick;
	Eon_Coord hspace, vspace;
};

static void _render(Eon_Paint *p, Eon_Engine *eng, void *engine_data, void *canvas_data, Eina_Rectangle *clip)
{
	eon_engine_grid_render(eng, engine_data, canvas_data, clip);
}

static void _ctor(void *instance)
{
	Eon_Grid *ch;
	Eon_Grid_Private *prv;

	ch = (Eon_Grid *)instance;
	ch->prv = prv = ekeko_type_instance_private_get(_type, instance);
	ch->parent.parent.create = eon_engine_grid_create;
	ch->parent.parent.delete = eon_engine_grid_delete;
	ch->parent.parent.render = _render;
}

static void _dtor(void *image)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_grid_init(void)
{
	_type = ekeko_type_new(EON_TYPE_GRID, sizeof(Eon_Grid),
			sizeof(Eon_Grid_Private), eon_paint_square_type_get(),
			_ctor, _dtor, eon_paint_appendable);
	EON_GRID_COLOR1 = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "color1",
			EON_PROPERTY_COLOR,
			OFFSET(Eon_Grid_Private, color1));
	EON_GRID_COLOR2 = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "color2",
			EON_PROPERTY_COLOR,
			OFFSET(Eon_Grid_Private, color2));
	EON_GRID_HSPACE = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "hspace",
			EON_PROPERTY_COORD,
			OFFSET(Eon_Grid_Private, hspace));
	EON_GRID_VSPACE = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "vspace",
			EON_PROPERTY_COORD,
			OFFSET(Eon_Grid_Private, vspace));
	EON_GRID_HTHICK = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "hthick",
			EON_PROPERTY_COORD,
			OFFSET(Eon_Grid_Private, hthick));
	EON_GRID_VTHICK = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "vthick",
			EON_PROPERTY_COORD,
			OFFSET(Eon_Grid_Private, vthick));

	eon_type_register(_type, EON_TYPE_GRID);
}

void eon_grid_shutdown(void)
{
	eon_type_unregister(_type);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_GRID_COLOR1;
Ekeko_Property_Id EON_GRID_COLOR2;
Ekeko_Property_Id EON_GRID_VSPACE;
Ekeko_Property_Id EON_GRID_HSPACE;
Ekeko_Property_Id EON_GRID_VTHICK;
Ekeko_Property_Id EON_GRID_HTHICK;

EAPI Eon_Grid * eon_grid_new(Eon_Document *d)
{
	Eon_Grid *ch;

	ch = eon_document_object_new(d, EON_TYPE_GRID);

	return ch;
}

EAPI Eon_Color eon_grid_color1_get(Eon_Grid *sq)
{
	Eon_Grid_Private *prv = PRIVATE(sq);

	return prv->color1;
}

EAPI Eon_Color eon_grid_color2_get(Eon_Grid *sq)
{
	Eon_Grid_Private *prv = PRIVATE(sq);

	return prv->color2;
}

EAPI void eon_grid_color1_set(Eon_Grid *sq, Eon_Color color)
{
	Ekeko_Value v;

	eon_value_color_from(&v, color);
	ekeko_object_property_value_set((Ekeko_Object *)sq, "color1", &v);
}

EAPI void eon_grid_color2_set(Eon_Grid *sq, Eon_Color color)
{
	Ekeko_Value v;

	eon_value_color_from(&v, color);
	ekeko_object_property_value_set((Ekeko_Object *)sq, "color2", &v);
}
