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
#define PRIVATE(d) ((Eon_Setter_Private *)((Eon_Setter *)(d))->private)

static Ekeko_Type *_type;
struct _Eon_Setter_Private
{
	char *name;
	Ekeko_Value value;
};

static void _unset(Ekeko_Object *o, Ekeko_Object *rel)
{

}

/* Called whenever we need to set the value */
static void _set(Ekeko_Object *o, Ekeko_Object *rel)
{
	Eon_Setter *s = (Eon_Setter *)o;
	Eon_Setter_Private *prv = PRIVATE(o);

	printf("SETTING %s %d\n", prv->name, prv->value.type);
	ekeko_object_property_value_set(rel, prv->name, &prv->value);
}

static void _ctor(Ekeko_Object *o)
{
	Eon_Setter *s;
	Eon_Setter_Private *prv;

	s = (Eon_Setter *)o;
	s->private = prv = ekeko_type_instance_private_get(_type, o);
	/* default values */
	s->parent.set = _set;
	s->parent.unset = _unset;
}

static void _dtor(Ekeko_Object *o)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_setter_init(void)
{
	_type = ekeko_type_new(EON_TYPE_SETTER, sizeof(Eon_Setter),
			sizeof(Eon_Setter_Private),
			eon_style_applier_type_get(),
			_ctor, _dtor, NULL);
	EON_SETTER_PROPERTY = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "name",
			EKEKO_PROPERTY_STRING,
			OFFSET(Eon_Setter_Private, name));
	EON_SETTER_VALUE = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "value",
			EKEKO_PROPERTY_VALUE,
			OFFSET(Eon_Setter_Private, value));

	eon_type_register(_type, EON_TYPE_SETTER);
}

void eon_setter_shutdown(void)
{
	eon_type_unregister(_type);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_SETTER_PROPERTY;
Ekeko_Property_Id EON_SETTER_VALUE;

Eon_Setter * eon_setter_new(Eon_Document *d)
{
	Eon_Setter *s;

	s = eon_document_object_new(d, EON_TYPE_SETTER);

	return s;
}


char * eon_setter_name_get(Eon_Setter *s)
{
	Eon_Setter_Private *prv;

	prv = PRIVATE(s);
	return prv->name;
}

