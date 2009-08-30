/*
 * eon_text.c
 *
 *  Created on: 28-jun-2009
 *      Author: jl
 */

/* Should the text be a child of square? i.e with w and h properties? */

#include "Eon.h"
#include "eon_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Text_Private *)((Eon_Text *)(d))->private)
struct _Eon_Text_Private
{
	Eon_Coord x, y;
	char *str;
};

static void _render(Eon_Shape *s, Eon_Engine *eng, void *engine_data, void *canvas_data, Eina_Rectangle *clip)
{
	Eon_Text *t;
	Eon_Text_Private *prv;

	t = (Eon_Text *)s;

#ifdef EON_DEBUG
	printf("[Eon_Text] Rendering text %p into canvas %p\n", t, c);
#endif
	//eon_engine_text_render(eng, engine_data, canvas_data, clip);
}

static void _ctor(void *instance)
{
	Eon_Text *t;
	Eon_Text_Private *prv;

	t = (Eon_Text *) instance;
	t->private = prv = ekeko_type_instance_private_get(eon_text_type_get(), instance);
	t->parent.render = _render;
	//t->parent.create = eon_engine_text_create;
	/* events */
}

static void _dtor(void *instance)
{

}

static Eina_Bool _appendable(void *instance, void *child)
{
	if (!ekeko_type_instance_is_of(child, EON_TYPE_ANIMATION))
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_TEXT_X;
Ekeko_Property_Id EON_TEXT_Y;
Ekeko_Property_Id EON_TEXT_STR;

EAPI Ekeko_Type *eon_text_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_TEXT, sizeof(Eon_Text),
				sizeof(Eon_Text_Private), eon_shape_type_get(),
				_ctor, _dtor, _appendable);
		EON_TEXT_X = EKEKO_TYPE_PROP_SINGLE_ADD(type, "x", EON_PROPERTY_COORD, OFFSET(Eon_Text_Private, x));
		EON_TEXT_Y = EKEKO_TYPE_PROP_SINGLE_ADD(type, "y", EON_PROPERTY_COORD, OFFSET(Eon_Text_Private, y));
		EON_TEXT_STR = EKEKO_TYPE_PROP_SINGLE_ADD(type, "str", EKEKO_PROPERTY_STRING, OFFSET(Eon_Text_Private, str));
	}

	return type;
}

EAPI Eon_Text * eon_text_new(Eon_Canvas *c)
{
	Eon_Text *t;

	t = ekeko_type_instance_new(eon_text_type_get());
	ekeko_object_child_append((Ekeko_Object *)c, (Ekeko_Object *)t);

	return t;
}

EAPI void eon_text_string_set(Eon_Text *t, const char *str)
{
	Ekeko_Value v;

	ekeko_value_str_from(&v, str);
	ekeko_object_property_value_set((Ekeko_Object *)t, "str", &v);
}

EAPI void eon_text_x_rel_set(Eon_Text *t, int x)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, x, EON_COORD_RELATIVE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)t, "x", &v);
}

EAPI void eon_text_x_set(Eon_Text *t, int x)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, x, EON_COORD_ABSOLUTE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)t, "x", &v);
}

EAPI void eon_text_y_set(Eon_Text *t, int y)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, y, EON_COORD_ABSOLUTE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)t, "y", &v);
}

EAPI void eon_text_y_rel_set(Eon_Text *t, int y)
{
	Eon_Coord coord;
	Ekeko_Value v;

	eon_coord_set(&coord, y, EON_COORD_RELATIVE);
	eon_value_coord_from(&v, &coord);
	ekeko_object_property_value_set((Ekeko_Object *)t, "y", &v);
}

