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
/* TODO handle correctly the file_change, if we are or not attached into
 * a document, etc
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Script_Private *)((Eon_Script *)(d))->prv)

struct _Eon_Script_Private
{
	char *file;
};

static Eina_Hash *_scripts = NULL;

static void _file_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Script *s = (Eon_Script *)o;
	Eon_Script_Private *prv = PRIVATE(s);
	Eon_Canvas *c;
	Ekeko_Object *p;

	p = ekeko_object_parent_get(o);
	while (p)
	{
		if (!strcmp(ekeko_object_type_name_get(p), EON_TYPE_DOCUMENT))
			break;
		p = ekeko_object_parent_get(p);
	}
	if (!p)
	{
		printf("NOOOOOOOOOOOOOOOOOOOO\n");
		return;
	}
	eon_document_script_load(p, em->curr->value.string_value);
}

static void _ctor(void *instance)
{
	Eon_Script *e;
	Eon_Script_Private *prv;

	e = (Eon_Script *) instance;
	e->prv = prv = ekeko_type_instance_private_get(eon_script_type_get(), instance);
	ekeko_event_listener_add((Ekeko_Object *)e, EON_SCRIPT_FILE_CHANGED, _file_change, EINA_FALSE, NULL);
}

static void _dtor(void *instance)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_script_init(void)
{
	_scripts = eina_hash_string_superfast_new(NULL);
#ifdef BUILD_SCRIPT_NEKO
	script_neko_init();
#endif
}

void eon_script_shutdown(void)
{
	/* TODO remove the hash */
}

void eon_script_register(const char *type, Eon_Script_Module *m)
{
	printf("Script with name %s registered\n", type);
	eina_hash_add(_scripts, type, m);
}

Eon_Script_Module * eon_script_get(const char *type)
{
	return eina_hash_find(_scripts, type);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_SCRIPT_FILE;

EAPI Ekeko_Type *eon_script_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_SCRIPT, sizeof(Eon_Script),
				sizeof(Eon_Script_Private), ekeko_object_type_get(),
				_ctor, _dtor, NULL);
		EON_SCRIPT_FILE = EKEKO_TYPE_PROP_SINGLE_ADD(type, "file", EKEKO_PROPERTY_STRING, OFFSET(Eon_Script_Private, file));
	}
	return type;
}

EAPI Eon_Script * eon_script_new(void)
{
	Eon_Script *e;

	e = ekeko_type_instance_new(eon_script_type_get());

	return e;
}

EAPI void eon_script_file_set(Eon_Script *e, const char *file)
{
	Ekeko_Value v;

	ekeko_value_str_from(&v, file);
	ekeko_object_property_value_set((Ekeko_Object *)e, "file", &v);
}
