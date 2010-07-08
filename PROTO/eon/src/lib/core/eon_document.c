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
#define PRIVATE(o) ((Eon_Document_Private *)((Eon_Document *)(o))->prv)
#define DBG(...) EINA_LOG_DOM_DBG(_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_dom, __VA_ARGS__)

static int _dom = -1;
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

static Ekeko_Type * _document_type_get(void);

/* Called whenever an object changes it's id */
static void _id_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Document *d = (Eon_Document *)o;
	Eon_Document_Private *prv = PRIVATE(o);
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;

	eina_hash_add(prv->ids, em->curr->value.string_value, o);
}
/* Called whenever the CPU is idle. Here we process the main layout
 * object
 */
static int _idler_cb(void *data)
{
	Eon_Document *d = data;
	Eon_Document_Private *prv = PRIVATE(d);

	if (prv->layout) eon_layout_process(prv->layout);
	
	return 1;
}
/* Timer callback to tick the etch animation system */
static Eina_Bool _animation_cb(void *data)
{
	Eon_Document *d = data;
	Eon_Document_Private *prv = PRIVATE(d);

	etch_timer_tick(prv->etch);
	return EINA_TRUE;
}

/* Called whenever a child is appended to the document directly */
static void _child_appended(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Document *d = (Eon_Document *)o;
	Eon_Document_Private *prv = PRIVATE(d);

	if (!prv->layout && ekeko_type_instance_is_of(em->related, EON_TYPE_LAYOUT))
		prv->layout = (Eon_Layout *)em->related;
	else if (!prv->style && ekeko_type_instance_is_of(em->related, EON_TYPE_STYLE))
		prv->style = (Eon_Style *)em->related;
}

static void _event_object_new(Eon_Document_Object_New *ev, char *name, Ekeko_Object *o)
{
	Ekeko_Event *ee = (Ekeko_Event *)ev;

	ev->name = name;
	ekeko_event_init(ee, EON_DOCUMENT_OBJECT_NEW, o, EINA_FALSE);
}

/*----------------------------------------------------------------------------*
 *                           Base Type functions                              *
 *----------------------------------------------------------------------------*/
static void _ctor(Ekeko_Object *o)
{
	Eon_Document *d;
	Eon_Document_Private *prv = PRIVATE(d);

	d = (Eon_Document *)o;
	/* FIXME do we need a single idler or better one idler per document? */
	if (!_idler)
	{
		/* this idler will process every child */
		_idler = ecore_idle_enterer_add(_idler_cb, d);
	}
	d->prv = prv = ekeko_type_instance_private_get(_document_type_get(), o);
	/* setup the animation system */
	prv->etch = etch_new();
	etch_timer_fps_set(prv->etch, 30);
	prv->anim_cb = ecore_timer_add(1.0f/30.0f, _animation_cb, d);
	/* the id system */
	prv->ids = eina_hash_string_superfast_new(NULL);
	/* the event listeners */
	ekeko_event_listener_add(o, EKEKO_EVENT_OBJECT_APPEND, _child_appended, EINA_FALSE, NULL);
}

static void _dtor(Ekeko_Object *o)
{

}

static Eina_Bool _appendable(Ekeko_Object *parent, Ekeko_Object *child)
{
	Eon_Document *d = (Eon_Document *)parent;
	Eon_Document_Private *prv = PRIVATE(d);

	/* we only allow style and canvas children */
	if ((!ekeko_type_instance_is_of(child, EON_TYPE_LAYOUT)) &&
			(!ekeko_type_instance_is_of(child, EON_TYPE_STYLE)))
		return EINA_FALSE;
	/* we should use the childs list instead as we might have more than one canvas */
	if (prv->layout && ekeko_type_instance_is_of(child, EON_TYPE_LAYOUT))
		return EINA_FALSE;
	if (prv->style && ekeko_type_instance_is_of(child, EON_TYPE_STYLE))
		return EINA_FALSE;

	return EINA_TRUE;
}

static Ekeko_Type * _document_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		_dom = eina_log_domain_register("eon:document", NULL);
		type = ekeko_type_new(EON_TYPE_DOCUMENT, sizeof(Eon_Document),
				sizeof(Eon_Document_Private), ekeko_object_type_get(),
				_ctor, _dtor, _appendable);
	}

	return type;
}

static Eon_Document * _document_new(const char *engine, int w, int h, const char *options)
{
	Eon_Document *d;
	Eon_Document_Private *prv;
	Ekeko_Value v;

	d = ekeko_type_instance_new(_document_type_get());
	if (!d)
		return NULL;

	prv = PRIVATE(d);
	/* the gfx engine */
	prv->engine.backend = eon_engine_get(engine);
	prv->engine.data = eon_engine_setup(prv->engine.backend, d, w, h, options);
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
	Eon_Document_Private *prv = PRIVATE(d);

	return prv->engine.backend;
}

void * eon_document_engine_data_get(Eon_Document *d)
{
	Eon_Document_Private *prv = PRIVATE(d);

	return prv->engine.data;
}

Etch * eon_document_etch_get(Eon_Document *d)
{
	Eon_Document_Private *prv = PRIVATE(d);

	return prv->etch;
}

void eon_document_script_execute(Eon_Document *d, const char *fname, Ekeko_Object *o)
{
	Eon_Document_Private *prv = PRIVATE(d);

	if (!prv->vm.sm || !prv->vm.sm->execute)
		return;
	prv->vm.sm->execute(prv->vm.data, fname, o);
}

void eon_document_script_unload(Eon_Document *d, const char *file)
{

}

void eon_document_script_load(Eon_Document *d, const char *file)
{
	Eon_Document_Private *prv = PRIVATE(d);

	if (!prv->vm.sm)
		return;
	prv->vm.sm->load(prv->vm.data, file);
}

void eon_document_resize(Eon_Document *d, int w, int h)
{
	Eon_Document_Private *prv = PRIVATE(d);
	Eon_Document_Size_Change ev;

	prv->size.w = ev.geom.w = w;
	prv->size.h = ev.geom.h = h;

	ekeko_event_init(&ev, EON_DOCUMENT_SIZE_CHANGED, (Ekeko_Object *)d, EINA_FALSE);
	ekeko_object_event_dispatch((Ekeko_Object *)d, (Ekeko_Event *)&ev);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Creates a new document
 * @return The newly created document
 */
EAPI Eon_Document * eon_document_new(const char *engine, int w, int h, const char *options)
{
	Eon_Document *d;
	Eon_Canvas *c;
	Eon_Style *s;

	d = _document_new(engine, w, h, options);
#if 0
	/* the main style */
	s = eon_style_new(d);
	ekeko_object_child_append((Ekeko_Object *)d, (Ekeko_Object *)s);
#endif
	return d;
}

EAPI Eon_Layout * eon_document_layout_get(Eon_Document *d)
{
	Eon_Document_Private *prv = PRIVATE(d);

	return prv->layout;
}

/* FIXME remove this */
EAPI Eon_Style * eon_document_style_get(Eon_Document *d)
{
	Eon_Document_Private *prv = PRIVATE(d);

	return prv->style;
}
/**
 * Gets the size of the document
 * @param d The document to get the size from
 * @param w The variable to store the width
 * @param h The variable to store the height
 */
EAPI void eon_document_size_get(Eon_Document *d, int *w, int *h)
{
	Eon_Document_Private *prv = PRIVATE(d);

	if (w) *w = prv->size.w;
	if (h) *h = prv->size.h;
}

EAPI Ekeko_Object * eon_document_object_get_by_id(Eon_Document *d, const char *id)
{
	Eon_Document_Private *prv = PRIVATE(d);

	return eina_hash_find(prv->ids, id);
}
/**
 * All animations of a document will be paused
 * @param d The document to pause
 */
EAPI void eon_document_pause(Eon_Document *d)
{
	Eon_Document_Private *prv = PRIVATE(d);

	if (!prv->anim_cb)

		return;

	ecore_timer_del(prv->anim_cb);
	prv->anim_cb = NULL;
}
/**
 * All animations of a document will play
 * @param d The document to play
 */
EAPI void eon_document_play(Eon_Document *d)
{
	Eon_Document_Private *prv = PRIVATE(d);

	if (prv->anim_cb)
		return;

	prv->anim_cb = ecore_timer_add(1.0f/30.0f, _animation_cb, d);
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

