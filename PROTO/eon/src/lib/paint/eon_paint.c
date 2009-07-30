/*
 * eon_paint.c
 *
 *  Created on: 04-feb-2009
 *      Author: jl
 */
#include "Eon.h"
#include "eon_private.h"

#define EON_PAINT_DEBUG 0
/* TODO
 * + add coordinate space flag, object space or canvas space for relative and
 * absolute coordinates
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Paint_Private *)((Eon_Paint *)(d))->private)
struct _Eon_Paint_Private
{
	/* TODO move x, y, w, h and coordspace to the image object */
	Eon_Coord x, y, w, h;
	Enesim_Matrix matrix;
	Enesim_Matrix inverse;
	Eon_Paint_Coordspace coordspace;
	void *engine_data;
};

static void _child_append_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Paint *p;
	Eon_Paint_Private *prv;
	Eon_Document *d;
	Eon_Engine *eng;
	Eon_Canvas *c;

	c = eon_paint_canvas_get((Eon_Paint *)o);
	if (!c)
		return;
	/* when this shape is appended to a canvas, try to setup the context */
	d = eon_canvas_document_get(c);
	/* FIXME in case the canvas doesnt have a document */
	eng = eon_document_engine_get(d);
	p = (Eon_Paint *)o;
	prv = PRIVATE(p);
	prv->engine_data = p->create(eng, p);
}

static void _matrix_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Paint *i = (Eon_Paint *)o;
	Eon_Paint_Private *prv = PRIVATE(i);
	Enesim_Matrix *m;
	Ekeko_Object *parent;

	m = em->curr->value.pointer_value;
	enesim_matrix_inverse(m, &prv->inverse);
}

static void _ctor(void *instance)
{
	Eon_Paint *p;
	Eon_Paint_Private *prv;

	p = (Eon_Paint*) instance;
	p->private = prv = ekeko_type_instance_private_get(eon_paint_type_get(), instance);
	enesim_matrix_identity(&prv->matrix);
	enesim_matrix_inverse(&prv->matrix, &prv->inverse);
	ekeko_event_listener_add((Ekeko_Object *)p, EON_PAINT_MATRIX_CHANGED, _matrix_change, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)p, EKEKO_EVENT_OBJECT_APPEND, _child_append_cb, EINA_FALSE, NULL);
}

static void _dtor(void *paint)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void * eon_paint_engine_data_get(Eon_Paint *p)
{
	Eon_Paint_Private *prv = PRIVATE(p);
	return prv->engine_data;
}

/*
 * There are paints which might have other paints as childs, so we should
 * handle a recursive canvas finding
 */
Eon_Canvas * eon_paint_canvas_get(Eon_Paint *p)
{
	Ekeko_Object *o;

	o = ekeko_object_parent_get((Ekeko_Object *)p);
	while (!ekeko_type_instance_is_of(o, EON_TYPE_CANVAS))
	{
		o = ekeko_object_parent_get(o);
	}
	return (Eon_Canvas *)o;
}

void eon_paint_create(Eon_Paint *p)
{
	Eon_Paint_Private *prv = PRIVATE(p);
	Eon_Canvas *c;
	Eon_Document *d;
	Eon_Engine *eng;

	c = eon_paint_canvas_get(p);
	d = eon_canvas_document_get(c);
	/* FIXME in case the canvas doesnt have a document */
	eng = eon_document_engine_get(d);
	prv->engine_data = p->create(eng, p);
}

Eina_Bool eon_paint_setup(Eon_Paint *p, Eon_Shape *s)
{
	Eon_Paint_Private *prv = PRIVATE(p);
	Eon_Canvas *c;
	Eon_Document *d;
	Eon_Engine *eng;

	c = eon_paint_canvas_get(p);
	d = eon_canvas_document_get(c);
	/* FIXME in case the canvas doesnt have a document */
	eng = eon_document_engine_get(d);
	return p->setup(eng, prv->engine_data, s);
}

Eina_Bool eon_paint_appendable(Ekeko_Object *p, Ekeko_Object *child)
{
	if (!ekeko_type_instance_is_of(child, EON_TYPE_ANIMATION))
		return EINA_FALSE;
	else
		return EINA_TRUE;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_PAINT_MATRIX;
Ekeko_Property_Id EON_PAINT_X;
Ekeko_Property_Id EON_PAINT_Y;
Ekeko_Property_Id EON_PAINT_W;
Ekeko_Property_Id EON_PAINT_H;
Ekeko_Property_Id EON_PAINT_COORDSPACE;

EAPI Ekeko_Type *eon_paint_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_PAINT, sizeof(Eon_Paint),
				sizeof(Eon_Paint_Private), ekeko_object_type_get(),
				_ctor, _dtor, NULL);
		EON_PAINT_MATRIX = EKEKO_TYPE_PROP_SINGLE_ADD(type, "matrix", EON_PROPERTY_MATRIX, OFFSET(Eon_Paint_Private, matrix));
		EON_PAINT_X = EKEKO_TYPE_PROP_SINGLE_ADD(type, "x", EON_PROPERTY_COORD, OFFSET(Eon_Paint_Private, x));
		EON_PAINT_Y = EKEKO_TYPE_PROP_SINGLE_ADD(type, "y", EON_PROPERTY_COORD, OFFSET(Eon_Paint_Private, y));
		EON_PAINT_W = EKEKO_TYPE_PROP_SINGLE_ADD(type, "w", EON_PROPERTY_COORD, OFFSET(Eon_Paint_Private, w));
		EON_PAINT_H = EKEKO_TYPE_PROP_SINGLE_ADD(type, "h", EON_PROPERTY_COORD, OFFSET(Eon_Paint_Private, h));
		EON_PAINT_COORDSPACE = EKEKO_TYPE_PROP_SINGLE_ADD(type, "coordspace", EKEKO_PROPERTY_INT, OFFSET(Eon_Paint_Private, coordspace));
	}

	return type;
}

EAPI Eon_Paint * eon_paint_new(Eon_Canvas *c)
{
	Eon_Paint *r;

	r = ekeko_type_instance_new(eon_paint_type_get());
	ekeko_object_child_append((Ekeko_Object *)c, (Ekeko_Object *)r);

	return r;
}

EAPI void eon_paint_matrix_set(Eon_Paint *i, Enesim_Matrix *m)
{
	Ekeko_Value v;

	eon_value_matrix_from(&v, m);
	ekeko_object_property_value_set((Ekeko_Object *)i, "matrix", &v);
}

EAPI void eon_paint_coords_get(Eon_Paint *p, Eon_Coord *x, Eon_Coord *y, Eon_Coord *w, Eon_Coord *h)
{
	Eon_Paint_Private *prv = PRIVATE(p);

	*x = prv->x;
	*y = prv->y;
	*w = prv->w;
	*h = prv->h;
}

EAPI Eon_Paint_Coordspace eon_paint_coordspace_get(Eon_Paint *p)
{
	Eon_Paint_Private *prv = PRIVATE(p);

	return prv->coordspace;
}
