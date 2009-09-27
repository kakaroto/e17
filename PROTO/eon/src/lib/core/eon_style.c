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
/* TODO
 * the style can have only setters and triggers (maybe later animations?)
 * add a class, useful to know the property meanings on setters
 * whenever this object gets referenced check the type and stop propagation
 * if it isnt the needed type
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Style_Private *)((Eon_Style *)(d))->prv)

static Ekeko_Type *_type;
struct _Eon_Style_Private
{
	char *type;
	Eina_List *references;
};

static void _apply_all(Eon_Style *s, Ekeko_Object *rel)
{
	Eon_Style_Private *prv = PRIVATE(s);
	Ekeko_Object *o;

	/* for each child apply the style */
	o = ekeko_object_child_first_get(s);
	while (o)
	{
		Eon_Style_Applier *ap = (Eon_Style_Applier *)o;

		ap->set(ap, rel);
		o = ekeko_object_next(o);
	}
}

/* whenever some property changes on the childs call the apply() again */
static void _child_prop_change_cb(Ekeko_Object *o, Ekeko_Event *e, void *data)
{

}

/*
 * Called whenever a child is appended to the style element
 * we only support paint objects here, whenever the show/hide property is
 * changed dont allow a show
 */
static void _child_append_cb(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;

	//ekeko_event_listener_add(e->target, EKEKO_OBJECT_ID_CHANGED, _id_change, EINA_FALSE, d);
}

/* called whenever the object gets referenced */
static void _referenced_cb(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Style *s = (Eon_Style *)o;
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Style_Private *prv = PRIVATE(o);


	/* check that the object that gets the style is of the needed type */
	if (!ekeko_type_instance_is_of(em->related, prv->type))
	{
		/* TODO stop propagation */
		return;
	}
	/* TODO set the style */
	prv->references = eina_list_append(prv->references, em->related);
	_apply_all(s, em->related);
}

static void _ctor(Ekeko_Object *o)
{
	Eon_Style *s;
	Eon_Style_Private *prv;

	s = (Eon_Style *)o;
	s->prv = prv = ekeko_type_instance_private_get(_type, o);

	ekeko_event_listener_add(o, EKEKO_EVENT_OBJECT_REFERENCED,
			_referenced_cb, EINA_FALSE, NULL);
}

static void _dtor(Ekeko_Object *o)
{

}

static Eina_Bool _appendable(void *parent, void *child)
{
	if (!ekeko_type_instance_is_of(child, EON_TYPE_SETTER))
		return EINA_FALSE;
	return EINA_TRUE;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_style_init(void)
{
	_type = ekeko_type_new(EON_TYPE_STYLE, sizeof(Eon_Style),
			sizeof(Eon_Style_Private),
			ekeko_object_type_get(), _ctor, _dtor,
			_appendable);

	EON_STYLE_TYPE = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "type",
			EKEKO_PROPERTY_STRING,
			OFFSET(Eon_Style_Private, type));

	eon_type_register(_type, EON_TYPE_STYLE);
}

void eon_style_shutdown(void)
{
	eon_type_unregister(_type);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_STYLE_TYPE;

EAPI Eon_Style * eon_style_new(Eon_Document *d)
{
	Eon_Style *s;

	s = eon_document_object_new(d, EON_TYPE_STYLE);

	return s;
}
