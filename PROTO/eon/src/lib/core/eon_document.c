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
 * TODO:
 * + rename this object to just Eon instead of Eon_Document
 * + this isnt needed to be an ekeko object
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define DBG(...) EINA_LOG_DOM_DBG(_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_dom, __VA_ARGS__)

static int _dom = -1;
static Ecore_Idle_Enterer *_idler = NULL;
static Eina_List *_documents = NULL;

struct _Eon_Document
{
	Ekeko_Object parent;

	struct {
		char *name;
		void *data;
		Eon_Engine *backend;
	} engine;
	Eina_Rectangle size;
	/* we should use the childs instead of this */
	Eon_Layout *layout;
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
	Eon_Document *d = (Eon_Document *)o;
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;

	eina_hash_add(d->ids, em->curr->value.string_value, o);
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
	Eon_Document *d = data;

	etch_timer_tick(d->etch);
	return 1;
}

/* Called whenever a child is appended to the document */
static void _child_append_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Document *d = (Eon_Document *)o;

	ekeko_event_listener_add(e->target, EKEKO_OBJECT_ID_CHANGED, _id_change, EINA_FALSE, d);
	/* check that the parent is the document */
	if (em->related != o)
		return;
	/* assign the correct pointers */
	if (!d->layout && ekeko_type_instance_is_of(e->target, EON_TYPE_LAYOUT))
		d->layout = (Eon_Canvas *)e->target;
	if (!d->style && ekeko_type_instance_is_of(e->target, EON_TYPE_STYLE))
		d->style = (Eon_Style *)e->target;
}

static void _event_object_new(Eon_Document_Object_New *ev, char *name, Ekeko_Object *o)
{
	Ekeko_Event *ee = (Ekeko_Event *)ev;

	ev->name = name;
	ekeko_event_init(ee, EON_DOCUMENT_OBJECT_NEW, o, EINA_FALSE);
}

static void _ctor(Ekeko_Object *o)
{
	Eon_Document *d;

	/* FIXME do we need a single idler or better one idler per document? */
	if (!_idler)
	{
		/* this idler will process every child */
		_idler = ecore_idle_enterer_add(_idler_cb, NULL);
	}
	d = (Eon_Document *)o;
	/* setup the animation system */
	d->etch = etch_new();
	etch_timer_fps_set(d->etch, 30);
	d->anim_cb = ecore_timer_add(1.0f/30.0f, _animation_cb, d);
	/* the id system */
	d->ids = eina_hash_string_superfast_new(NULL);
	/* the event listeners */
	ekeko_event_listener_add(o, EKEKO_EVENT_OBJECT_APPEND, _child_append_cb, EINA_TRUE, NULL);
}

static void _dtor(void *canvas)
{

}

static Eina_Bool _appendable(Ekeko_Object *parent, Ekeko_Object *child)
{
	Eon_Document *d = (Eon_Document *)parent;

	/* we only allow style and canvas children */
	if ((!ekeko_type_instance_is_of(child, EON_TYPE_CANVAS)) &&
			(!ekeko_type_instance_is_of(child, EON_TYPE_STYLE)))
		return EINA_FALSE;
	/* we should use the childs list instead as we might have more than one canvas */
	if (d->layout && ekeko_type_instance_is_of(child, EON_TYPE_CANVAS))
		return EINA_FALSE;
	if (d->style && ekeko_type_instance_is_of(child, EON_TYPE_STYLE))
		return EINA_FALSE;

	return EINA_TRUE;
}

static Ekeko_Type * _document_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		_dom = eina_log_domain_register("eon:document", NULL);
		type = ekeko_type_new(EON_TYPE_DOCUMENT, 0,
				sizeof(Eon_Document), ekeko_object_type_get(),
				_ctor, _dtor, _appendable);
		/*EON_DOCUMENT_SIZE = EKEKO_TYPE_PROP_SINGLE_ADD(type, "size",
				EKEKO_PROPERTY_RECTANGLE,
				OFFSET(Eon_Document, size));*/
	}

	return type;
}

static Eon_Document * _document_new(const char *engine, int w, int h, const char *options)
{
	Eon_Document *d;
	Ekeko_Value v;

	d = ekeko_type_instance_new(_document_type_get());
	if (!d)
		return NULL;

	/* the gfx engine */
	d->engine.backend = eon_engine_get(engine);
	d->engine.data = eon_engine_document_create(d->engine.backend, d, options);
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
	return d->engine.backend;
}

void * eon_document_engine_data_get(Eon_Document *d)
{
	return d->engine.data;
}

Etch * eon_document_etch_get(Eon_Document *d)
{
	return d->etch;
}

void eon_document_script_execute(Eon_Document *d, const char *fname, Ekeko_Object *o)
{
	if (!d->vm.sm || !d->vm.sm->execute)
		return;
	d->vm.sm->execute(d->vm.data, fname, o);
}

void eon_document_script_unload(Eon_Document *d, const char *file)
{

}

void eon_document_script_load(Eon_Document *d, const char *file)
{
	if (!d->vm.sm)
		return;
	d->vm.sm->load(d->vm.data, file);
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_DOCUMENT_SIZE;

/**
 * Creates a new document
 */
EAPI Eon_Document * eon_document_new(const char *engine, int w, int h, const char *options)
{
	Eon_Document *d;
	Eon_Canvas *c;
	Eon_Style *s;

	d = _document_new(engine, w, h, options);
	/* the main style */
	s = eon_style_new(d);
	ekeko_object_child_append((Ekeko_Object *)d, (Ekeko_Object *)s);

	return d;
}

EAPI Eon_Canvas * eon_document_layout_get(Eon_Document *d)
{
	return d->layout;
}

/* FIXME remove this */
EAPI Eon_Style * eon_document_style_get(Eon_Document *d)
{
	return d->style;
}

EAPI void eon_document_size_get(Eon_Document *d, int *w, int *h)
{
	if (w) *w = d->size.w;
	if (h) *h = d->size.h;
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
	return eina_hash_find(d->ids, id);
}

EAPI void eon_document_pause(Eon_Document *d)
{
	if (!d->anim_cb)
		return;

	ecore_timer_del(d->anim_cb);
	d->anim_cb = NULL;
}

EAPI void eon_document_play(Eon_Document *d)
{
	if (d->anim_cb)
		return;

	d->anim_cb = ecore_timer_add(1.0f/30.0f, _animation_cb, d);
}

/**
 * Creates a new object based on it's registered name
 * @param d The document that creates the object
 * @param name The name of the object's type
 * @return The instance of the object or NULL of it wasn't able to create
 * such object
 */
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
		/* check that the object is a subtype of an Eon_Object */
		/* send the event of a new object */
		Eon_Document_Object_New ev;

		_event_object_new(&ev, name, o);
		ekeko_object_event_dispatch(d, (Ekeko_Event *)&ev);
		/* set the document on the newly created object */
		eon_object_document_set(o, d);
	}
	return o;
}

