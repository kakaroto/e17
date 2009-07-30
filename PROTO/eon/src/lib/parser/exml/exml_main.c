/*
 * exml_main.c
 *
 *  Created on: 14-jul-2009
 *      Author: jl
 */
#include <stdio.h>
#include <string.h>

#include "Eon.h"
#include "EXML.h"

/* a list of callbacks for objects that need a two pass parsing */
Eina_List *parsed_cb = NULL;
/* global for now until we have a way to get the document from every object */
Eon_Document *doc;

#define PARSER_DEBUG 0
/*
 * The testsuite is an application that parses xml files with eon scenes
 * TODO make eon type system to be defined at eon_init() to avoid strcmp => eon_foo
 * attributes of type VALUE are relative to another attribute, how to handle it?
 *
 */
char *engine = "sdl";
char *image_dir = "./";

void parse(EXML *exml, Ekeko_Object *doc);

typedef void (*callback)(char *value, char *attr, void *data);

typedef struct _Parser_Callback
{
	char *value;
	char *attr;
	void *data;
	callback cb;
} Parser_Callback;

static void _add_callback(callback cb, char *value, char *attr, void *data)
{
	Parser_Callback *pc;

	pc = malloc(sizeof(Parser_Callback));
	pc->value = value;
	pc->data = data;
	pc->attr = attr;
	pc->cb = cb;
	parsed_cb = eina_list_append(parsed_cb, pc);
}


static void _event_callback(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	printf("EVENT CALLBACK %s!!!\n", (char *)data);
	/* call the function from the VM */
	eon_document_script_execute(doc, (char *)data, o);
}

static void _object_id_get(char *value, char *attr, void *data)
{
	printf("called\n");
}

static void _anim_id_get(char *value, char *attr, void *data)
{
	Ekeko_Object *anim = data;
	Ekeko_Object *oid;
	Ekeko_Value v;
	Eon_Trigger t;
	char *tmp;
	char *ttmp;
	char *token;

	if (!value)
		return;
	tmp = strdup(value);
	ttmp = tmp + 1;
	token = strtok(ttmp, ".");
	oid = eon_document_object_get_by_id(doc, token);
	if (!oid)
		goto out;
	t.obj = oid;
	token = strtok(NULL, ".");
	if (!token)
		goto out;
	t.event = token;
	eon_value_trigger_from(&v, &t);
	ekeko_object_property_value_set(anim, attr, &v);
out:
	free(tmp);
}

/* parsing of the different attributes */
void object_eon_attribute_set(Ekeko_Object *o, Ekeko_Value_Type type, char *attr, char *value)
{
	Ekeko_Value v;

	if (type == EON_PROPERTY_COORD)
	{
		Eon_Coord c;
		char *tmp;

		tmp = strdup(value);
		eon_parser_coord_str_from(&c, tmp);
		eon_value_coord_from(&v, &c);
		ekeko_object_property_value_set(o, attr, &v);
		free(tmp);
	}
	else if (type == EON_PROPERTY_CLOCK)
	{
		Eon_Clock c;
		char *tmp;

		tmp = strdup(value);
		eon_parser_clock_str_from(&c, tmp);
		eon_value_clock_from(&v, &c);
		ekeko_object_property_value_set(o, attr, &v);
		free(tmp);
	}
	else if (type == EON_PROPERTY_TRIGGER)
	{
		Eon_Trigger t;
		char *tmp = strdup(value);

		/* FIXME the object can be referenced by #id :) */
		/* #id.event */
		if (*value == '#')
		{
			char *token;
			char *ttmp = tmp + 1;
			Ekeko_Object *oid;

			token = strtok(ttmp, ".");
			oid = eon_document_object_get_by_id(doc, token);
			if (!oid)
			{
				/* queue this once the document has been completely parsed */
				_add_callback(_anim_id_get, value, attr, o);
				return;
			}
			t.obj = oid;
			token = strtok(NULL, ".");
			t.event = token;
		}
		else
		{
			t.obj = ekeko_object_parent_get(o);
			t.event = value;
		}
		eon_value_trigger_from(&v, &t);
		ekeko_object_property_value_set(o, attr, &v);
		free(tmp);
	}
	else if (type == EON_PROPERTY_MATRIX)
	{
		Enesim_Matrix m;

		if (eon_parser_matrix_str_from(&m, value))
		{
			eon_value_matrix_from(&v, &m);
			ekeko_object_property_value_set(o, attr, &v);
		}
	}
	else if (type == EON_PROPERTY_COLOR)
	{
		Eon_Color c = 0;

		eon_parser_color_str_from(&c, value);
		eon_value_color_from(&v, c);
		ekeko_object_property_value_set(o, attr, &v);
	}
}


void object_value_attribute_set(Ekeko_Object *o, Ekeko_Value_Type type, char *attr, char *name)
{
	/* if it is an animation get the prop name */
}

void object_attribute_set(Ekeko_Object *o, Ekeko_Value_Type type, char *attr, char *name)
{
	Ekeko_Value v;

	v.type = 0;
	/* properties with special values */
	/* rop */
	if (!strcmp(attr, "rop"))
	{
		eon_parser_rop_str_from(&v, name);
		ekeko_object_property_value_set(o, attr, &v);
		return;
	}
	/* quality */
	/* properties with simple values */
	switch (type)
	{
		case EKEKO_PROPERTY_INT:
		ekeko_value_int_from(&v, strtol(name, NULL, 0));
		ekeko_object_property_value_set(o, attr, &v);
		break;

		case EKEKO_PROPERTY_VALUE:
		{
			if (ekeko_type_instance_is_of(o, "Eon_Animation_Key"))
			{
				Ekeko_Object *p;
				Ekeko_Value_Type vtype;

				p = ekeko_object_parent_get(o);
				vtype = eon_animation_prop_type_get(p);
				object_attribute_set(o, vtype, attr, name);
			}
#if 0
			/* FIXME remove "Eon_Animation" */
			if (ekeko_type_instance_is_of(o, "Eon_Animation"))
			{
				Ekeko_Object *parent;
				Ekeko_Value_Type vtype;

				/* get the name of the attribute to animate */
				/* FIXME remove "name" */
				parent = ekeko_object_parent_get(o);
				if (!parent)
					return;

				if (ekeko_type_instance_is_of(o, "Eon_Animation_Basic"))
				{
					Ekeko_Value nvalue;
					Ekeko_Property *prop;

					ekeko_object_property_value_get(o, "name", &nvalue);
					/* in case of anim tag, handle every type */
					prop = ekeko_object_property_get(parent, nvalue.value.string_value);
					vtype = ekeko_property_value_type_get(prop);
#if PARSER_DEBUG
					printf("[PARSER] Going to set a value for prop %d %s with %d\n", nvalue.type, nvalue.value.string_value, vtype);
#endif
				}
				else if (ekeko_type_instance_is_of(o, "Eon_Animation_Matrix"))
				{
					/* in case of animmatrix, this is handled differently */
					vtype = EKEKO_PROPERTY_FLOAT;
				}
				object_attribute_set(o, vtype, attr, name);
			}
#endif
		}
		return;

		case EKEKO_PROPERTY_FLOAT:
		ekeko_value_float_from(&v, strtof(name, NULL));
		ekeko_object_property_value_set(o, attr, &v);
		break;

		case EKEKO_PROPERTY_BOOL:
		case EKEKO_PROPERTY_CHAR:
		case EKEKO_PROPERTY_DOUBLE:
		case EKEKO_PROPERTY_SHORT:
		case EKEKO_PROPERTY_LONG:
		case EKEKO_PROPERTY_RECTANGLE:
		case EKEKO_PROPERTY_POINTER:
		return;

		case EKEKO_PROPERTY_OBJECT:
		{
			Ekeko_Object *oid;
			char *tmp = strdup(name);

			if (*tmp == '#')
			{
				char *ttmp = tmp + 1;
				Ekeko_Object * oid;

				oid = eon_document_object_get_by_id(doc, ttmp);
				if (!oid)
				{
					/* queue this once the document has been completely parsed */
					_add_callback(_object_id_get, tmp, attr, o);
					return;
				}
				else
				{
					ekeko_value_object_from(&v, oid);
					ekeko_object_property_value_set(o, attr, &v);
					free(tmp);
				}
			}
		}
		return;

		case EKEKO_PROPERTY_STRING:
		{
			/* FIXME remove "Eon_Image" */
			if (ekeko_type_instance_is_of(o, "Eon_Image") && !strcmp(attr, "file"))
			{
				char real_file[256];

				sprintf(real_file, "%s/%s", image_dir, name);
				ekeko_value_str_from(&v, strdup(real_file));
				ekeko_object_property_value_set(o, attr, &v);
			}
			else
			{
				ekeko_value_str_from(&v, strdup(name));
				ekeko_object_property_value_set(o, attr, &v);
			}
		}
		break;

		default:
		object_eon_attribute_set(o, type, attr, name);
		return;
	}
}

void attributes_set(void *value, void *data)
{
	Ecore_Hash_Node *n = value;
	Ekeko_Object *o = data;
	Ekeko_Property *prop;
	Ekeko_Value_Type type;

	//printf("[PARSER] Setting attribute %s\n", (char *)n->key);
	prop = ekeko_object_property_get(o, n->key);
	if (!prop)
	{
		if (strncmp(n->key, "on", 2))
			return;
		/* handle the onXXX event types */
		else
		{
			printf("registering event %s\n", (char *)n->key + 2);
			ekeko_event_listener_add(o, n->key + 2, _event_callback, EINA_FALSE, strdup(n->value));
			return;
		}
	}
	/* in case the object is an animation skip the prop attribute
	 * as it is already set
	 */
	/* FIXME remove "Eon_Animation" */
	if (ekeko_type_instance_is_of(o, "Eon_Animation"))
	{
		if (ekeko_property_id_get(prop) == EON_ANIMATION_PROPERTY)
		{
			return;
		}
	}

	type = ekeko_property_value_type_get(prop);
	object_attribute_set(o, type, n->key, n->value);
}

Ekeko_Object * tag_create(char *tag, EXML *exml, Ekeko_Object *parent)
{
	EXML_Node *n;
	Ekeko_Object *o = NULL;

	n = exml_get(exml);
#if PARSER_DEBUG
	printf("[PARSER] creating tag %s for parent %s\n", tag, ekeko_object_type_name_get(parent));
#endif
	if (!strcmp(tag, "eon"))
	{
		o = (Ekeko_Object *)eon_canvas_new((Eon_Canvas *)parent);
	}
	else if (!strcmp(tag, "script"))
	{
		o = eon_parser_script_new(parent);
		ekeko_object_child_append(parent, o);
	}
	else if (!strcmp(tag, "rect"))
	{
		o = eon_parser_rect_new(parent);
	}
	else if (!strcmp(tag, "circle"))
	{
		o = eon_parser_circle_new(parent);
	}
	else if (!strcmp(tag, "image"))
	{
		o = (Ekeko_Object *)eon_image_new();
		ekeko_object_child_append(parent, o);
	}
	else if (!strcmp(tag, "hswitch"))
	{
		o = (Ekeko_Object *)eon_hswitch_new();
		ekeko_object_child_append(parent, o);
	}
	else if (!strcmp(tag, "fade"))
	{
		o = (Ekeko_Object *)eon_fade_new();
		ekeko_object_child_append(parent, o);
	}
	else if (!strcmp(tag, "sqpattern"))
	{
		o = (Ekeko_Object *)eon_checker_new();
		ekeko_object_child_append(parent, o);
	}
	else if (!strcmp(tag, "poly"))
	{
		char *value;

		o = eon_parser_polygon_new(parent);
		value = ecore_hash_get(n->attributes, "points");
		if (!value)
		{
			return o;
		}
		eon_parser_polygon_points_str_from(o, value);
	}
	else if (!strcmp(tag, "anim") || !strcmp(tag, "animMatrix"))
	{
		char *value;

		if (!strcmp(tag, "animMatrix"))
			o = (Ekeko_Object *)eon_animation_matrix_new();
		else
			o = (Ekeko_Object *)eon_animation_basic_new();
		if (!o)
			return NULL;
		ekeko_object_child_append(parent, o);
		/* if we set the attributes "from/to" we should know the prop name
		 * before setting them
		 */
		/* FIXME remove "name" */
		value = ecore_hash_get(n->attributes, "name");
		if (!value)
		{
			return NULL;
		}
		object_attribute_set(o, EKEKO_PROPERTY_STRING, "name", value);
	}
	else if (!strcmp(tag, "key"))
	{
		o = (Ekeko_Object *)eon_animation_key_new(parent);
	}
	if (!o)
		return NULL;
	/* attributes */
	ecore_hash_for_each_node(n->attributes, attributes_set, o);
	/* TODO if is renderable show() it */
	return o;
}

void parse(EXML *exml, Ekeko_Object *parent)
{
	EXML_Node *n;
	char *tag;

	if (!parent)
		return;
	n = exml_get(exml);
	if (!n)
		return;
#if !PARSER_DEBUG
	printf("[PARSER] parsing tag %s with parent %s\n", n->tag, ekeko_object_type_name_get(parent));
#endif
	parent = tag_create(n->tag, exml, parent);
	tag = exml_down(exml);
	while (tag)
	{
		/* children */
		parse(exml, parent);
		/* siblings */
		tag = exml_next_nomove(exml);
	}
	exml_goto_node(exml, n);
}

static Eina_Bool file_load(Eon_Canvas *canvas, const char *file)
{
	EXML *exml;
	EXML_Node *n;
	Eina_Iterator *it;
	Parser_Callback *pc;
	char *tag;

	exml = exml_new();
	if (!exml_file_read(exml, file))
	{
		printf("no file\n");
		return EINA_FALSE;
	}
	n = exml_get(exml);
	if (!n || strcmp(n->tag, "eon"))
	{
		printf("no eon file\n");
		return EINA_FALSE;
	}
	doc = eon_canvas_document_get(canvas);
	ecore_hash_for_each_node(n->attributes, attributes_set, canvas);
	/* parse the file */
	tag = exml_down(exml);
	while (tag)
	{
		/* children */
		parse(exml, (Ekeko_Object *)canvas);
		/* siblings */
		tag = exml_next(exml);
	}
	/* call the after callbacks */
	it = eina_list_iterator_new(parsed_cb);
	while (eina_iterator_next(it, &pc))
	{
		pc->cb(pc->value, pc->attr, pc->data);
	}
	eina_iterator_free(it);
	exml_destroy(exml);
	return EINA_TRUE;
}


Eina_Bool tree_get(Eon_External *e, const char *file)
{
	return EINA_TRUE;
}

Eina_Bool subtree_get(Eon_External *e, const char *file)
{
	EXML *exml;
	EXML_Node *n;
	Eina_Iterator *it;
	Parser_Callback *pc;
	char *tag;

	exml = exml_new();
	if (!exml_file_read(exml, file))
	{
		printf("no file\n");
		return EINA_FALSE;
	}
	n = exml_get(exml);
	if (!n || strcmp(n->tag, "eon"))
	{
		printf("no eon file\n");
		return EINA_FALSE;
	}
	/*
	doc = eon_canvas_document_get(canvas);*/
	/* parse the file */
	tag = exml_down(exml);
	while (tag)
	{
		/* children */
		parse(exml, (Ekeko_Object *)e);
		/* siblings */
		tag = exml_next(exml);
	}
	/* call the after callbacks */
	it = eina_list_iterator_new(parsed_cb);
	while (eina_iterator_next(it, &pc))
	{
		pc->cb(pc->value, pc->attr, pc->data);
	}
	eina_iterator_free(it);
	exml_destroy(exml);
	return EINA_TRUE;
}

Eon_Parser p = {
		.file_load = file_load,
		.tree_get = tree_get,
		.subtree_get = subtree_get,
};

void parser_exml_init(void)
{
	printf("exml init\n");
	eon_parser_register("exml", &p);
}
