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
#define PRIVATE(d) ((Eon_Checker_Private *)((Eon_Checker *)(d))->prv)

static Ekeko_Type * _type;
struct _Eon_Checker_Private
{
	Enesim_Renderer *renderer;
	int relative;
	/* properties */
	Eon_Coord sw, sh;
	Eon_Color color1, color2;
};

static void _geometry_changed(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Paint_Geometry_Change *gch = (Eon_Paint_Geometry_Change *)e;
	Eon_Checker *ch = (Eon_Checker *)data;
	Eon_Checker_Private *prv = PRIVATE(ch);

	if (prv->sw.type == EON_COORD_RELATIVE)
	{
		eon_coord_length_relative_calculate(&prv->sw, gch->geom.w,
				&prv->sw.final);
	}
	if (prv->sh.type == EON_COORD_RELATIVE)
	{
		eon_coord_length_relative_calculate(&prv->sh, gch->geom.h,
				&prv->sh.final);
	}
}

static void _sw_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Checker_Private *prv = PRIVATE(o);
	Eon_Coord *prev, *curr;
	Eina_Rectangle geom;

	prev = em->prev->value.pointer_value;
	curr = em->curr->value.pointer_value;

	eon_paint_geometry_get((Eon_Paint *)o, &geom);
	eon_coord_length_change2(o, curr, prev, &prv->relative,
			geom.w, o, EON_PAINT_GEOMETRY_CHANGED,
			_geometry_changed);
}

static void _sh_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Checker_Private *prv = PRIVATE(o);
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
	//eon_engine_checker_render(eng, engine_data, canvas_data, clip);
}
/*----------------------------------------------------------------------------*
 *                                Properties                                  *
 *----------------------------------------------------------------------------*/
static void _color1_set(Eon_Checker *ch, Eon_Color color)
{

}

static void _color2_set(Eon_Checker *ch, Eon_Color color)
{

}
/*----------------------------------------------------------------------------*
 *                                  Events                                    *
 *----------------------------------------------------------------------------*/
static void _property_get(Ekeko_Object *o, Ekeko_Event *e, void *data)
{

}
static void _property_set(Ekeko_Object *o, Ekeko_Event *e, void *data)
{

}
/*----------------------------------------------------------------------------*
 *                           Base Type functions                              *
 *----------------------------------------------------------------------------*/
static void _ctor(Ekeko_Object *o)
{
	Eon_Checker *ch;
	Eon_Checker_Private *prv;

	ch = (Eon_Checker *)o;
	ch->prv = prv = ekeko_type_instance_private_get(_type, o);
	ch->parent.parent.process = eon_paint_process;
	//ch->parent.parent.render = _render;
	/* add the properties */
	EON_CHECKER_COLOR1 = ekeko_object_property_add(o, "color1",
			EON_PROPERTY_COLOR);
	EON_CHECKER_COLOR2 = ekeko_object_property_add(o, "color2",
			EON_PROPERTY_COLOR);
	EON_CHECKER_SW = ekeko_object_property_add(o, "sw",
			EON_PROPERTY_COORD);
	EON_CHECKER_SH = ekeko_object_property_add(o, "sh",
			EON_PROPERTY_COORD);
	/* events */
	ekeko_event_listener_add(o, EKEKO_EVENT_VALUE_GET,
			_property_get, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, EKEKO_EVENT_VALUE_SET,
			_property_set, EINA_FALSE, NULL);
}

static void _dtor(Ekeko_Object *o)
{
	/* remove the properties */
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_checker_init(void)
{
	_type = ekeko_type_new(EON_TYPE_CHECKER, sizeof(Eon_Checker),
			sizeof(Eon_Checker_Private), eon_paint_type_get(),
			_ctor, _dtor, eon_paint_appendable);

	eon_type_register(_type, EON_TYPE_CHECKER);
}

void eon_checker_shutdown(void)
{
	eon_type_unregister(_type);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_CHECKER_SW = NULL;
Ekeko_Property_Id EON_CHECKER_SH = NULL;
Ekeko_Property_Id EON_CHECKER_COLOR1 = NULL;
Ekeko_Property_Id EON_CHECKER_COLOR2 = NULL;

/**
 *
 */
EAPI Eon_Checker * eon_checker_new(Eon_Document *d)
{
	Eon_Checker *ch;

	ch = (Eon_Checker *)eon_document_object_new(d, EON_TYPE_CHECKER);

	return ch;
}
/**
 *
 */
EAPI void eon_checker_sw_get(Eon_Checker *ch, Eon_Coord *sw)
{
	Eon_Checker_Private *prv;

	prv = PRIVATE(ch);
	if (sw) *sw = prv->sw;
}
/**
 *
 */
EAPI void eon_checker_sw_rel_set(Eon_Checker *ch, int sw)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_relative_set(&coord, sw);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)ch, "sw", &v);
}
/**
 *
 */
EAPI void eon_checker_sw_set(Eon_Checker *ch, int sw)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_absolute_set(&coord, sw);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)ch, "sw", &v);
}
/**
 *
 */
EAPI void eon_checker_sh_get(Eon_Checker *ch, Eon_Coord *sh)
{
	Eon_Checker_Private *prv;

	prv = PRIVATE(ch);
	if (sh) *sh = prv->sh;
}
/**
 *
 */
EAPI void eon_checker_sh_set(Eon_Checker *ch, int sh)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_absolute_set(&coord, sh);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)ch, "sh", &v);
}
/**
 *
 */
EAPI void eon_checker_sh_rel_set(Eon_Checker *ch, int sh)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_relative_set(&coord, sh);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)ch, "sh", &v);
}
/**
 *
 */
EAPI Eon_Color eon_checker_color1_get(Eon_Checker *sq)
{
	Eon_Checker_Private *prv = PRIVATE(sq);

	return prv->color1;
}
/**
 *
 */
EAPI Eon_Color eon_checker_color2_get(Eon_Checker *sq)
{
	Eon_Checker_Private *prv = PRIVATE(sq);

	return prv->color2;
}
/**
 *
 */
EAPI void eon_checker_color1_set(Eon_Checker *sq, Eon_Color color)
{
	Ekeko_Value v;

	eon_value_color_from(&v, color);
	ekeko_object_property_value_set((Ekeko_Object *)sq, "color1", &v);
}
/**
 *
 */
EAPI void eon_checker_color2_set(Eon_Checker *sq, Eon_Color color)
{
	Ekeko_Value v;

	eon_value_color_from(&v, color);
	ekeko_object_property_value_set((Ekeko_Object *)sq, "color2", &v);
}
