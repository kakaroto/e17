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
#define PRIVATE(d) ((Eon_Trigger_Private *)((Eon_Trigger_Object *)(d))->private)

static Ekeko_Type *_type;
struct _Eon_Trigger_Private
{
	char *name;
	Ekeko_Value v;
};

static void _ctor(Ekeko_Object *o)
{
	Eon_Trigger_Object *t;
	Eon_Trigger_Private *prv;

	t = (Eon_Trigger *)o;
	t->private = prv = ekeko_type_instance_private_get(_type, instance);
	/* default values */
}

static void _dtor(Ekeko_Object *o)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_trigger_init(void)
{
	_type = ekeko_type_new(EON_TYPE_TRIGGER, sizeof(Eon_Trigger_Object),
			sizeof(Eon_Trigger_Private), ekeko_object_type_get(),
			_ctor, _dtor, NULL);
	eon_type_register(_type, EON_TYPE_TRIGGER);
}

void eon_trigger_shutdown(void)
{
	eon_type_unregister(_type);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Eon_Trigger_Object * eon_trigger_new(Eon_Document *d)
{
	Eon_Trigger_Object *t;

	t = eon_document_object_new(d, EON_TYPE_TRIGGER);

	return t;
}

