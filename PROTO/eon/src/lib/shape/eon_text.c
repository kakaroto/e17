/*
 * eon_text.c
 *
 *  Created on: 28-jun-2009
 *      Author: jl
 */

#include "Eon.h"
#include "eon_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Text_Private *)((Eon_Text *)(d))->private)
struct _Eon_Text_Private
{
	char *text;
	void *engine_data;
};

static void _update_geometry(Eon_Text *p)
{
	Eon_Text_Private *prv = PRIVATE(p);
	Eina_Rectangle geom;

	eina_rectangle_coords_from(&geom, prv->x.final - prv->radius, prv ->y.final - prv->radius, prv->radius << 1, prv->radius << 1);
	ekeko_renderable_geometry_set((Ekeko_Renderable *)p, &geom);
}

static void _render(Eon_Shape *s, Eon_Engine *eng, void *engine_data, void *canvas_data, Eina_Rectangle *clip)
{
	Eon_Text *t;
	Eon_Text_Private *prv;

	t = (Eon_Text *)s;

#ifdef EON_DEBUG
	printf("[Eon_Text] Rendering text %p into canvas %p\n", t, c);
#endif
	eon_engine_text_render(eng, engine_data, canvas_data, clip);
}

static void _ctor(void *instance)
{
	Eon_Text *t;
	Eon_Text_Private *prv;

	t = (Eon_Text *) instance;
	t->private = prv = ekeko_type_instance_private_get(eon_polygon_type_get(), instance);
	t->parent.render = _render;
	t->parent.create = eon_engine_text_create;
	/* events */
}

static void _dtor(void *polygon)
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
EAPI Ekeko_Type *eon_text_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_CIRCLE, sizeof(Eon_Text),
				sizeof(Eon_Text_Private), eon_shape_type_get(),
				_ctor, _dtor, _appendable);
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
