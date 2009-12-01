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
	int relative;
	/* properties */
	Eon_Color color1, color2;
	Eon_Coord hthick, vthick;
	Eon_Coord hspace, vspace;
};

static void _geometry_changed(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Paint_Geometry_Change *gch = (Eon_Paint_Geometry_Change *)e;
	Eon_Grid *ch = (Eon_Grid *)data;
	Eon_Grid_Private *prv = PRIVATE(ch);

	if (prv->hspace.type == EON_COORD_RELATIVE)
	{
		eon_coord_length_relative_calculate(&prv->hspace, gch->geom.w,
				&prv->hspace.final);
	}
	if (prv->vspace.type == EON_COORD_RELATIVE)
	{
		eon_coord_length_relative_calculate(&prv->vspace, gch->geom.h,
				&prv->vspace.final);
	}
	if (prv->hthick.type == EON_COORD_RELATIVE)
	{
		eon_coord_length_relative_calculate(&prv->hthick, gch->geom.w,
				&prv->hthick.final);
	}
	if (prv->vthick.type == EON_COORD_RELATIVE)
	{
		eon_coord_length_relative_calculate(&prv->vthick, gch->geom.h,
				&prv->vthick.final);
	}
}

static void _hspace_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Grid_Private *prv = PRIVATE(o);
	Eon_Coord *prev, *curr;
	Eina_Rectangle geom;

	prev = em->prev->value.pointer_value;
	curr = em->curr->value.pointer_value;

	eon_paint_geometry_get((Eon_Paint *)o, &geom);
	eon_coord_length_change2(o, curr, prev, &prv->relative,
			geom.w, o, EON_PAINT_GEOMETRY_CHANGED,
			_geometry_changed);
}

static void _vspace_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Grid_Private *prv = PRIVATE(o);
	Eon_Coord *prev, *curr;
	Eina_Rectangle geom;

	prev = em->prev->value.pointer_value;
	curr = em->curr->value.pointer_value;

	eon_paint_geometry_get((Eon_Paint *)o, &geom);
	eon_coord_length_change2(o, curr, prev, &prv->relative,
			geom.h, o, EON_PAINT_GEOMETRY_CHANGED,
			_geometry_changed);
}

static void _hthick_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Grid_Private *prv = PRIVATE(o);
	Eon_Coord *prev, *curr;
	Eina_Rectangle geom;

	prev = em->prev->value.pointer_value;
	curr = em->curr->value.pointer_value;

	eon_paint_geometry_get((Eon_Paint *)o, &geom);
	eon_coord_length_change2(o, curr, prev, &prv->relative,
			geom.w, o, EON_PAINT_GEOMETRY_CHANGED,
			_geometry_changed);
}

static void _vthick_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Grid_Private *prv = PRIVATE(o);
	Eon_Coord *prev, *curr;
	Eina_Rectangle geom;

	prev = em->prev->value.pointer_value;
	curr = em->curr->value.pointer_value;

	eon_paint_geometry_get((Eon_Paint *)o, &geom);
	eon_coord_length_change2(o, curr, prev, &prv->relative,
			geom.h, o, EON_PAINT_GEOMETRY_CHANGED,
			_geometry_changed);
}

static void _render(Eon_Paint *p, Eon_Engine *eng, void *engine_data, void *canvas_data, Eina_Rectangle *clip)
{
	eon_engine_grid_render(eng, engine_data, canvas_data, clip);
}

static void _ctor(Ekeko_Object *o)
{
	Eon_Grid *ch;
	Eon_Grid_Private *prv;

	ch = (Eon_Grid *)o;
	ch->prv = prv = ekeko_type_instance_private_get(_type, o);
	ch->parent.parent.process = eon_paint_process;
	ch->parent.parent.create = eon_engine_grid_create;
	ch->parent.parent.free = eon_engine_grid_delete;
	ch->parent.parent.render = _render;
	ekeko_event_listener_add(o, EON_GRID_HSPACE_CHANGED,
			_hspace_change, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, EON_GRID_VSPACE_CHANGED,
			_vspace_change, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, EON_GRID_HTHICK_CHANGED,
			_hthick_change, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, EON_GRID_VTHICK_CHANGED,
			_vthick_change, EINA_FALSE, NULL);
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

EAPI void eon_grid_vspace_get(Eon_Grid *g, Eon_Coord *vs)
{
	Eon_Grid_Private *prv;

	prv = PRIVATE(g);
	if (vs) *vs = prv->vspace;
}

EAPI void eon_grid_vspace_rel_set(Eon_Grid *g, int vs)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_relative_set(&coord, vs);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)g, "vspace", &v);
}

EAPI void eon_grid_vspace_set(Eon_Grid *g, int vs)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_absolute_set(&coord, vs);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)g, "vspace", &v);
}

EAPI void eon_grid_hspace_get(Eon_Grid *g, Eon_Coord *hs)
{
	Eon_Grid_Private *prv;

	prv = PRIVATE(g);
	if (hs) *hs = prv->hspace;
}

EAPI void eon_grid_hspace_rel_set(Eon_Grid *g, int hs)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_relative_set(&coord, hs);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)g, "hspace", &v);
}

EAPI void eon_grid_hspace_set(Eon_Grid *g, int hs)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_absolute_set(&coord, hs);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)g, "hspace", &v);
}

EAPI void eon_grid_hthick_get(Eon_Grid *g, Eon_Coord *ht)
{
	Eon_Grid_Private *prv;

	prv = PRIVATE(g);
	if (ht) *ht = prv->hthick;
}

EAPI void eon_grid_hthick_set(Eon_Grid *g, int ht)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_absolute_set(&coord, ht);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)g, "hthick", &v);
}

EAPI void eon_grid_hthick_rel_set(Eon_Grid *g, int ht)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_relative_set(&coord, ht);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)g, "hthick", &v);
}

EAPI void eon_grid_vthick_get(Eon_Grid *g, Eon_Coord *vt)
{
	Eon_Grid_Private *prv;

	prv = PRIVATE(g);
	if (vt) *vt = prv->vthick;
}

EAPI void eon_grid_vthick_set(Eon_Grid *g, int vt)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_absolute_set(&coord, vt);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)g, "vthick", &v);
}

EAPI void eon_grid_vthick_rel_set(Eon_Grid *g, int vt)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_relative_set(&coord, vt);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)g, "vthick", &v);
}


EAPI Eon_Color eon_grid_color1_get(Eon_Grid *sq)
{
	Eon_Grid_Private *prv = PRIVATE(sq);

	return prv->color1;
}

EAPI void eon_grid_color1_set(Eon_Grid *sq, Eon_Color color)
{
	Ekeko_Value v;

	eon_value_color_from(&v, color);
	ekeko_object_property_value_set((Ekeko_Object *)sq, "color1", &v);
}

EAPI Eon_Color eon_grid_color2_get(Eon_Grid *sq)
{
	Eon_Grid_Private *prv = PRIVATE(sq);

	return prv->color2;
}


EAPI void eon_grid_color2_set(Eon_Grid *sq, Eon_Color color)
{
	Ekeko_Value v;

	eon_value_color_from(&v, color);
	ekeko_object_property_value_set((Ekeko_Object *)sq, "color2", &v);
}
