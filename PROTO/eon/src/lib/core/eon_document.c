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
/*
 * TODO rename this object to just Eon instead of Eon_Document
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Document_Private *)((Eon_Document *)(d))->private)

static Ecore_Idle_Enterer *_idler = NULL;
static Eina_List *_documents = NULL;

struct _Eon_Document_Private
{
	struct {
		char *name;
		void *data;
		Eon_Engine *backend;
	} engine;
	Eina_Rectangle size;
	/* we should use the childs instead of this */
	Eon_Canvas *canvas;
	Eon_Style *style;
	Eina_Hash *ids;
	Etch *etch;
	Ecore_Timer *anim_cb;
	/* each document might have a VM, only one VM type per document */
	struct {
		Eon_Script_Module *sm;
		void *data;
	} vm;
};

/* Called whenever an object changes it's id */
static void _id_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Document_Private *prv = PRIVATE(data);

	eina_hash_add(prv->ids, em->curr->value.string_value, o);
}

static int _idler_cb(void *data)
{
	Eina_Iterator *it;
	Eon_Document *doc;

	it = eina_list_iterator_new(_documents);
	while (eina_iterator_next(it, (void **)&doc))
	{
		ekeko_object_process((Ekeko_Object *)doc);
	}
	return 1;
}
static int _animation_cb(void *data)
{
	Eon_Document *doc = data;
	Eon_Document_Private *prv = PRIVATE(doc);

	etch_timer_tick(prv->etch);
	return 1;
}

/* Called whenever a child is appended to the document */
static void _child_append_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Document *d = (Eon_Document *)o;
	Eon_Document_Private *prv = PRIVATE(d);

	ekeko_event_listener_add(e->target, EKEKO_OBJECT_ID_CHANGED, _id_change, EINA_FALSE, d);
	/* check that the parent is the document */
	if (em->related != o)
		return;
	/* assign the correct pointers */
	if (!prv->canvas && ekeko_type_instance_is_of(e->target, EON_TYPE_CANVAS))
		prv->canvas = (Eon_Canvas *)e->target;
	if (!prv->style && ekeko_type_instance_is_of(e->target, EON_TYPE_STYLE))
		prv->style = (Eon_Style *)e->target;
}

static void _object_new(Eon_Document_Object_New *ev, char *name, Ekeko_Object *o)
{
	Ekeko_Event *ee = (Ekeko_Event *)ev;

	ev->name = name;
	ekeko_event_init(ee, EON_DOCUMENT_OBJECT_NEW, o, EINA_FALSE);
}

static void _ctor(void *instance)
{
	Eon_Document *dc;
	Eon_Document_Private *prv;

	/* FIXME do we need a single idler or better one idler per document? */
	if (!_idler)
	{
		/* this idler will process every child */
		_idler = ecore_idle_enterer_add(_idler_cb, NULL);
	}
	dc = (Eon_Document*) instance;
	dc->private = prv = ekeko_type_instance_private_get(eon_document_type_get(), instance);
	/* setup the animation system */
	prv->etch = etch_new();
	etch_timer_fps_set(prv->etch, 30);
	prv->anim_cb = ecore_timer_add(1.0f/30.0f, _animation_cb, dc);
	/* the id system */
	prv->ids = eina_hash_string_superfast_new(NULL);
	/* the event listeners */
	ekeko_event_listener_add((Ekeko_Object *)dc, EKEKO_EVENT_OBJECT_APPEND, _child_append_cb, EINA_TRUE, NULL);
}

static void _dtor(void *canvas)
{

}

static Eina_Bool _appendable(void *parent, void *child)
{
	Eon_Document_Private *prv = PRIVATE(parent);

	/* we only allow style and canvas children */
	if ((!ekeko_type_instance_is_of(child, EON_TYPE_CANVAS)) &&
			(!ekeko_type_instance_is_of(child, EON_TYPE_STYLE)))
		return EINA_FALSE;
	/* we should use the childs list instead as we might have more than one canvas */
	if (prv->canvas && ekeko_type_instance_is_of(child, EON_TYPE_CANVAS))
		return EINA_FALSE;
	if (prv->style && ekeko_type_instance_is_of(child, EON_TYPE_STYLE))
		return EINA_FALSE;

	return EINA_TRUE;
}

static Eon_Document * _document_new(const char *engine, int w, int h, const char *options)
{
	Eon_Document *d;
	Eon_Document_Private *prv;
	Ekeko_Value v;

	d = ekeko_type_instance_new(eon_document_type_get());
	if (!d)
		return NULL;

	prv = PRIVATE(d);
	/* the gfx engine */
	prv->engine.backend = eon_engine_get(engine);
	prv->engine.data = eon_engine_document_create(prv->engine.backend, d, options);
	eon_document_resize(d, w, h);
#if 0
	/* the script engine */
	prv->vm.sm = eon_script_get("neko");
	/* FIXME only initialize whenever we have a script element */
	prv->vm.data = prv->vm.sm->init();
#endif
	_documents = eina_list_append(_documents, d);

	return d;

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eon_Engine * eon_document_engine_get(Eon_Document *d)
{
	Eon_Document_Private *prv;

	prv = PRIVATE(d);
	return prv->engine.backend;
}

void * eon_document_engine_data_get(Eon_Document *d)
{
	Eon_Document_Private *prv;

	prv = PRIVATE(d);
	return prv->engine.data;
}

Etch * eon_document_etch_get(Eon_Document *d)
{
	Eon_Document_Private *prv;

	prv = PRIVATE(d);
	return prv->etch;
}

void eon_document_script_execute(Eon_Document *d, const char *fname, Ekeko_Object *o)
{
	Eon_Document_Private *prv;

	prv = PRIVATE(d);
	if (!prv->vm.sm || !prv->vm.sm->execute)
		return;
	prv->vm.sm->execute(prv->vm.data, fname, o);
}

void eon_document_script_unload(Eon_Document *d, const char *file)
{

}

void eon_document_script_load(Eon_Document *d, const char *file)
{
	Eon_Document_Private *prv;

	prv = PRIVATE(d);
	if (!prv->vm.sm)
		return;
	prv->vm.sm->load(prv->vm.data, file);
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_DOCUMENT_SIZE;

Ekeko_Type *eon_document_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_DOCUMENT, sizeof(Eon_Document),
				sizeof(Eon_Document_Private), ekeko_object_type_get(), _ctor,
				_dtor, _appendable);
		EON_DOCUMENT_SIZE = EKEKO_TYPE_PROP_SINGLE_ADD(type, "size", EKEKO_PROPERTY_RECTANGLE, OFFSET(Eon_Document_Private, size));
	}

	return type;
}

EAPI Eon_Document * eon_document_new(const char *engine, int w, int h, const char *options)
{
	Eon_Document *d;
	Eon_Canvas *c;
	Eon_Style *s;

	d = _document_new(engine, w, h, options);
	/* the main style */
	s = eon_style_new(d);
	ekeko_object_child_append((Ekeko_Object *)d, (Ekeko_Object *)s);
	/* the main canvas */
	c = eon_canvas_new(d);
	ekeko_object_child_append((Ekeko_Object *)d, (Ekeko_Object *)c);

	return d;
}

EAPI Eon_Document * eon_document_void_new(const char *engine, int w, int h, const char *options)
{
	Eon_Document *d;

	d = _document_new(engine, w, h, options);

	return d;
}

EAPI Eon_Canvas * eon_document_canvas_get(Eon_Document *d)
{
	Eon_Document_Private *prv;

	prv = PRIVATE(d);
	return prv->canvas;
}

EAPI Eon_Style * eon_document_style_get(Eon_Document *d)
{
	Eon_Document_Private *prv;

	prv = PRIVATE(d);
	return prv->style;
}

EAPI void eon_document_size_get(Eon_Document *d, int *w, int *h)
{
	Eon_Document_Private *prv;

	prv = PRIVATE(d);
	if (w) *w = prv->size.w;
	if (h) *h = prv->size.h;
}

/* FIXME this is wrong! the document isnt resized from outside */
EAPI void eon_document_resize(Eon_Document *d, int w, int h)
{
	Ekeko_Value v;

	ekeko_value_rectangle_coords_from(&v, 0, 0, w, h);
	ekeko_object_property_value_set((Ekeko_Object *)d, "size", &v);
}

EAPI Ekeko_Object * eon_document_object_get_by_id(Eon_Document *d, const char *id)
{
	Eon_Document_Private *prv = PRIVATE(d);

	return eina_hash_find(prv->ids, id);
}

EAPI void eon_document_pause(Eon_Document *d)
{
	Eon_Document_Private *prv = PRIVATE(d);
	if (!prv->anim_cb)
		return;

	ecore_timer_del(prv->anim_cb);
	prv->anim_cb = NULL;
}

EAPI void eon_document_play(Eon_Document *d)
{
	Eon_Document_Private *prv = PRIVATE(d);
	if (prv->anim_cb)
		return;

	prv->anim_cb = ecore_timer_add(1.0f/30.0f, _animation_cb, d);
}


EAPI Ekeko_Object * eon_document_object_new(Eon_Document *d, const char *name)
{
	Ekeko_Object *o;
	Ekeko_Type *t;

	/* find the hash of types */
	t = eon_type_get(name);
	if (!t) return NULL;
	o = ekeko_type_instance_new(t);
	if (o)
	{
		/* send the event of a new object */
		Eon_Document_Object_New ev;

		_object_new(&ev, name, o);
		ekeko_object_event_dispatch(d, (Ekeko_Event *)&ev);
	}
	return o;
}

