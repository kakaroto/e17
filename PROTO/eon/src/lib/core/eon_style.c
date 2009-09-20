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
#define PRIVATE(d) ((Eon_Style_Private *)((Eon_Style *)(d))->private)

struct _Eon_Style_Private
{
};

/*
 * Called whenever a child is appended to the style element
 * we only support paint objects here, whenever the show/hide property is
 * changed dont allow a show
 */
static void _child_append_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;

	//ekeko_event_listener_add(e->target, EKEKO_OBJECT_ID_CHANGED, _id_change, EINA_FALSE, d);
}

static void _ctor(void *instance)
{

}

static void _dtor(void *canvas)
{

}

static Eina_Bool _appendable(void *parent, void *child)
{
	if (!ekeko_type_instance_is_of(child, EON_TYPE_PAINT))
		return EINA_FALSE;
	return EINA_TRUE;
}

static Ekeko_Type *_style_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_STYLE, sizeof(Eon_Style),
				sizeof(Eon_Style_Private), ekeko_object_type_get(), _ctor,
				_dtor, _appendable);
	}

	return type;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eon_Style * eon_style_new(void)
{
	Eon_Style *s;

	s = ekeko_type_instance_new(_style_type_get());
	return s;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

