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
#define PRIVATE(d) ((Eon_External_Private *)((Eon_External *)(d))->private)

struct _Eon_External_Private
{
	struct {
		char *curr;
		char *prev;
		int changed;
	} file;
	// relative paths
	char *img_prefix;
	char *script_prefix;
};

static Eina_Hash *_parsers = NULL;

static void _file_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Canvas *c;
	Eina_Iterator *it;
	Eon_Parser *p;

	/* TODO on the sync call check for the file existance, permissions, etc */
	/* TODO on the async call load the file */
	/* if we do this on the POST state the final attributes changed count are
	 * wrong
	 */
	if (em->state == EVENT_MUTATION_STATE_POST)
		return;

	if (!_parsers)
		return;
	/* FIXME the parent of every object created should be the external itself
	 * not the canvas
	 */
	c = ekeko_object_parent_get(o);
	/**
	 * Load the needed file and return the topmost object
	 * FIXME should we support non complete documents?
	 */
	it = eina_hash_iterator_data_new(_parsers);
	while (eina_iterator_next(it, (void **)&p))
	{
		if (p->file_load(c, em->curr->value.string_value))
		{
			break;
		}
	}
	ekeko_object_dump((Ekeko_Object *)eon_canvas_document_get(c), ekeko_object_dump_printf);
	eina_iterator_free(it);
}

static void _ctor(void *instance)
{
	Eon_External *e;
	Eon_External_Private *prv;

	e = (Eon_External *) instance;
	e->private = prv = ekeko_type_instance_private_get(eon_external_type_get(), instance);
	ekeko_event_listener_add((Ekeko_Object *)e, EON_EXTERNAL_FILE_CHANGED, _file_change, EINA_FALSE, NULL);
}

static void _dtor(void *instance)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_parser_init(void)
{
	_parsers = eina_hash_string_superfast_new(NULL);
#ifdef BUILD_PARSER_EXPAT
	parser_expat_init();
#endif
#ifdef BUILD_PARSER_EXML
	parser_exml_init();
#endif
}

void eon_parser_shutdown(void)
{
	/* TODO remove the hash */
}

void eon_parser_register(const char *name, Eon_Parser *p)
{
	printf("Parser with name %s registered\n", name);
	eina_hash_add(_parsers, name, p);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_EXTERNAL_FILE;

EAPI Ekeko_Type *eon_external_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_EXTERNAL, sizeof(Eon_External),
				sizeof(Eon_External_Private), ekeko_object_type_get(),
				_ctor, _dtor, NULL);
		EON_EXTERNAL_FILE = EKEKO_TYPE_PROP_DOUBLE_ADD(type, "file", EKEKO_PROPERTY_STRING,
						OFFSET(Eon_External_Private, file.curr), OFFSET(Eon_External_Private, file.prev),
						OFFSET(Eon_External_Private, file.changed));
	}
	return type;
}

EAPI Eon_External * eon_external_new(Eon_Canvas *c)
{
	Eon_External *e;

	e = ekeko_type_instance_new(eon_external_type_get());
	ekeko_object_child_append((Ekeko_Object *)c, (Ekeko_Object *)e);

	return e;
}

EAPI void eon_external_file_set(Eon_External *e, const char *file)
{
	Ekeko_Value v;

	ekeko_value_str_from(&v, file);
	ekeko_object_property_value_set((Ekeko_Object *)e, "file", &v);
}

EAPI void eon_external_image_prefix_set(Eon_External *e, const char *path)
{

}

EAPI void eon_external_script_prefix_set(Eon_External *e, const char *path)
{

}
