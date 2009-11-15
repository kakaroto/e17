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
#define PRIVATE(o) ((Eon_Layout_Private *)((Eon_Layout *)(o))->prv)

#define DBG(...) EINA_LOG_DOM_DBG(_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_dom, __VA_ARGS__)

#define TYPE_NAME "layout"

static int _dom = -1;

struct _Eon_Layout_Private
{
	struct {
		Eina_Bool curr;
		Eina_Bool prev;
		int changed;
	} redraw;
	Eina_Tiler *tiler;
	Eina_List *renderables;
	Eina_List *obscures;
	Eina_Inlist *inputs;
	Eon_Paint *focused;
};

void _sublayout_in(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	printf("SUBCANVAS in\n");
	/* TODO feed the mouse in into this layout */
}

void _sublayout_out(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	printf("SUBCANVAS out\n");
	/* TODO feed the mouse out into this layout */
}

void _sublayout_move(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	printf("SUBCANVAS move\n");
	/* TODO feed the mouse move into this layout */
}

/*
 * TODO use this instead of the code below
 * this code is as it doesnt maintain the stack hierarchy
 */
static inline void _renderable_append(Eon_Layout *c, Eon_Paint *r,
		Eina_Rectangle *cgeom, Eina_Rectangle *rgeom, Eina_Bool rvisible)
{
	Eina_Bool intersect;
	Eon_Layout_Private *prv;
	prv = PRIVATE(c);

	DBG("%p renderable append %p at %d %d %d %d - %d %d %d %d (%d)\n",
			c, r, cgeom->x, cgeom->y, cgeom->w, cgeom->h,
			rgeom->x, rgeom->y, rgeom->w, rgeom->h,
			rvisible);

	intersect = eina_rectangles_intersect(rgeom, cgeom);
	/* not visible */
	if (!rvisible)
	{
		if (eon_paint_renderable_get(r))
		{
			DBG("%p removing renderable %p\n", c, r);
			prv->renderables = eina_list_remove(prv->renderables, r);
			eon_paint_renderable_set(r, EINA_FALSE);
		}
	}
	/* visible and not intersect */
	else if (!intersect)
	{
		if (eon_paint_renderable_get(r))
		{
			DBG("%p removing renderable %p\n", c, r);
			prv->renderables = eina_list_remove(prv->renderables, r);
			eon_paint_renderable_set(r, EINA_FALSE);
		}
	}
	/* visible and intersect */
	else
	{
		Eon_Paint *lr = NULL;
		Eina_List *l;

		if (eon_paint_renderable_get(r))
			return;

		DBG("%p adding renderable %p\n", c, r);
		EINA_LIST_FOREACH(prv->renderables, l, lr)
		{
			if (!(eon_paint_zindex_get(r) >
					eon_paint_zindex_get(lr)))
			{
				prv->renderables = eina_list_prepend_relative(prv->renderables, r, lr);
				eon_paint_renderable_set(r, EINA_TRUE);
				return;
			}
		}
		prv->renderables = eina_list_append_relative(prv->renderables, r, lr);
		eon_paint_renderable_set(r, EINA_TRUE);
	}
}

static void _child_removed(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Layout *c = (Eon_Layout *)data;
	Eon_Paint *r = (Eon_Paint *)o;
	Eon_Layout_Private *prv;
	Eina_Rectangle geom;
	prv = PRIVATE(c);


	DBG("%s removed\n", ekeko_object_type_name_get(o));
	if (prv->focused == r)
	{
		prv->focused = NULL;
	}

	prv->renderables = eina_list_remove(prv->renderables, r);
	if (!prv->tiler) return;

	eon_paint_boundings_get(r, &geom);
	eina_tiler_rect_add(prv->tiler, &geom);
}

static void _child_geometry_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Layout *c = (Eon_Layout *)data;
	Eon_Paint *r = (Eon_Paint *)o;
	Eina_Rectangle cgeom;
	Eina_Bool rvisible;

	if (em->state != EVENT_MUTATION_STATE_POST)
		return;
	eon_paint_boundings_get((Eon_Paint *)c, &cgeom);
	/* reset the layout coordinates to 0,0 WxH */
	cgeom.x = 0;
	cgeom.y = 0;
	rvisible = eon_paint_visibility_get(r);
	_renderable_append(c, r, &cgeom, &em->curr->value.rect, rvisible);

}

static void _child_visibility_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Layout *c = (Eon_Layout *)data;
	Eon_Paint *r = (Eon_Paint *)o;
	Eina_Rectangle rgeom, cgeom;

	if (em->state != EVENT_MUTATION_STATE_POST)
		return;
	eon_paint_boundings_get((Eon_Paint *)c, &cgeom);
	/* reset the layout coordinates to 0,0 WxH */
	cgeom.x = 0;
	cgeom.y = 0;
	eon_paint_boundings_get(r, &rgeom);
	_renderable_append(c, r, &cgeom, &rgeom, em->curr->value.bool_value);
}

static void _geometry_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Layout_Private *prv;
	Eina_Tiler *tiler;

	if (em->state != EVENT_MUTATION_STATE_POST)
		return;

	prv = PRIVATE(((Eon_Layout *)o));
	tiler = prv->tiler;
	if (tiler)
	{
		eina_tiler_free(tiler);
	}
#ifdef EKEKO_DEBUG
	printf("[layout %s] Changing geometry\n", ekeko_object_type_name_get(o));
#endif
	prv->tiler = eina_tiler_new(em->curr->value.rect.w, em->curr->value.rect.h);
	/* TODO In case it already has a tiler, mark everything again */
	if (tiler)
	{
		eina_tiler_rect_add(prv->tiler, &em->curr->value.rect);
	}
#ifdef EKEKO_DEBUG
	printf("[layout %s] tiler is %p\n", ekeko_object_type_name_get(o), prv->tiler);
#endif
}

/* Called whenever the process has finished on this element */
static void _process_cb(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Layout *c;
	Eon_Layout_Private *prv;
	Eina_Iterator *it;
	Eina_Rectangle rect;
	Eon_Document *d;
	Eon_Engine *eng;
	Eon_Surface *surface;

	c = (Eon_Layout *)o;
#ifdef EKEKO_DEBUG
	printf("[layout %s] Processing layout %p\n", ekeko_object_type_name_get(o), o);
#endif
	prv = PRIVATE(c);
	if (!prv->tiler)
		return;

	d = eon_object_document_get((Eon_Object *)c);
	eng = eon_document_engine_get(d);
	surface = eon_object_engine_data_get((Eon_Object *)c);

	/* TODO remove the obscures */
	/* get the tiler render areas */
	it = eina_tiler_iterator_new(prv->tiler);
	while (eina_iterator_next(it, (void **)&rect))
	{
		Eina_Iterator *rit;
		Eon_Paint *r;

		DBG("%p Redraw rectangle %d %d %d %d\n", o, rect.x, rect.y, rect.w, rect.h);
		/* iterate over the list of renderables */
		rit = eina_list_iterator_new(prv->renderables);
		while (eina_iterator_next(rit, (void **)&r))
		{
			Eina_Rectangle geom;

			eon_paint_boundings_get(r, &geom);
			DBG("%p Rendering renderable %p (%d %d %d %d)\n", o, r, geom.x, geom.y, geom.w, geom.h);
			/* intersect the geometry and the damage area */
			if (!eina_rectangle_intersection(&geom, &rect))
				continue;
#if BOUNDING_DEBUG
			eon_engine_debug_rect(eng, surface, 0xffaaaaaa, rect.x, rect.y, rect.w, rect.h);
#endif
			/* call the draw function on the renderable */
			r->render(r, eng, eon_object_engine_data_get((Eon_Object *)r), surface, &geom);
		}
		eina_iterator_free(rit);
	}
	eina_iterator_free(it);
	/* iterate over the redraw rectangles and flush */
	it = eina_tiler_iterator_new(prv->tiler);
	while (eina_iterator_next(it, (void **)&rect))
	{
		if (c->flush(c, &rect))
			break;
	}
	eina_iterator_free(it);
	/* clear the tiler */
	eina_tiler_clear(prv->tiler);
}

static void _redraw_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Layout_Private *prv;

	if (em->state != EVENT_MUTATION_STATE_POST)
		return;
	/* toggle property */
	em->curr->value.bool_value = EINA_FALSE;
}

static void _child_append_cb(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Layout_Private *prv;

	/* TODO 
	 * + check if object is the same as the event.rel or
	 * check if the event.target is not a layout and it is different
	 * than this
	 * + check that the child is actually an instance of a renderable type
	 * if so append it to the renderables
	 * + What happens if the child is of type renderable
	 * *and* not a layout and has renderable objects?
	 */

	/* in case the child's layout is not this, skip */
	if (eon_paint_layout_get((Eon_Paint *)e->target) != (Eon_Layout *)o)
	{
		DBG("Skipping this %p renderable as it already has a layout %p and is not this %p\n",
				e->target,
				eon_paint_layout_get((Eon_Paint *)e->target), o);
		return;
	}
	/*
	 * TODO if the appended child is a layout, register every UI event to this
	 * object, so when they arrive insert those events into the new layout
	 */
	if (ekeko_type_instance_is_of(e->target, EON_TYPE_LAYOUT))
	{
		DBG("Child is a layout too, registering UI events\n");
		ekeko_event_listener_add(e->target, EON_EVENT_UI_MOUSE_IN,
				_sublayout_in, EINA_FALSE, NULL);
		ekeko_event_listener_add(e->target, EON_EVENT_UI_MOUSE_OUT,
				_sublayout_out, EINA_FALSE, NULL);
		ekeko_event_listener_add(e->target, EON_EVENT_UI_MOUSE_MOVE,
				_sublayout_move, EINA_FALSE, NULL);
	}
	DBG("%s child of type renderable %s\n", ekeko_object_type_name_get(o),
			ekeko_object_type_name_get(e->target));
	DBG("%s related %s\n", ekeko_object_type_name_get(o),
			ekeko_object_type_name_get(em->related));

	prv = PRIVATE(em->related);
	ekeko_event_listener_add(e->target, EON_PAINT_VISIBILITY_CHANGED,
			_child_visibility_change, EINA_FALSE, o);
	ekeko_event_listener_add(e->target, EON_PAINT_GEOMETRY_CHANGED,
			_child_geometry_change, EINA_FALSE, o);
	/* FIXME what about the DELETE event? */
	ekeko_event_listener_add(e->target, EKEKO_EVENT_OBJECT_REMOVE,
			_child_removed, EINA_FALSE, o);
}

static void _ctor(Ekeko_Object *o)
{
	Eon_Layout *layout;
	Eon_Layout_Private *prv;

	layout = (Eon_Layout *)o;
	layout->prv = prv = ekeko_type_instance_private_get(eon_layout_type_get(), o);
	prv->renderables = NULL;
	prv->tiler = eina_tiler_new(0, 0);
	ekeko_event_listener_add(o, EON_LAYOUT_REDRAW_CHANGED, _redraw_change, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, EON_PAINT_GEOMETRY_CHANGED, _geometry_change, EINA_FALSE, NULL);
	/* TODO add the event listener when the object has finished the process() function */
	ekeko_event_listener_add(o, EKEKO_EVENT_OBJECT_PROCESS, _process_cb, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, EKEKO_EVENT_OBJECT_APPEND, _child_append_cb, EINA_TRUE, NULL);
}

static void _dtor(Ekeko_Object *o)
{
	Eon_Layout_Private *prv;

	prv = PRIVATE(o);	
	eina_tiler_free(prv->tiler);
}

static Eina_Bool _appendable(Ekeko_Object *parent, Ekeko_Object *child)
{
	if (!ekeko_type_instance_is_of(child, EON_TYPE_PAINT))
		return EINA_FALSE;
	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_LAYOUT_REDRAW;

Ekeko_Type * eon_layout_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		_dom = eina_log_domain_register("eon:layout", NULL);
		type = ekeko_type_new(TYPE_NAME, sizeof(Eon_Layout),
				sizeof(Eon_Layout_Private),
				eon_paint_type_get(),
				_ctor, _dtor, _appendable);
		/* the properties */
		EON_LAYOUT_REDRAW = EKEKO_TYPE_PROP_DOUBLE_ADD(type, "redraw",
				EKEKO_PROPERTY_BOOL,
				OFFSET(Eon_Layout_Private, redraw.curr),
				OFFSET(Eon_Layout_Private, redraw.prev),
				OFFSET(Eon_Layout_Private, redraw.changed));
	}

	return type;
}

EAPI void eon_layout_size_set(Eon_Layout *c, int w, int h)
{
	Eina_Rectangle rect;

	rect.x = 0;
	rect.y = 0;
	rect.w = w;
	rect.h = h;
	eon_paint_geometry_set((Eon_Paint *)c, &rect);
}
/**
 * @brief Marks a rectangle on the layout as damaged, this area will be
 * processed again. When the layout process that area it will no longer be
 * a damaged area
 * @param r Rectangle that defines the area damaged
 */
EAPI void eon_layout_damage_add(Eon_Layout *c, Eina_Rectangle *r)
{
	Eon_Layout_Private *prv;

	prv = PRIVATE(c);
	if (prv->tiler)
	{
		Ekeko_Value v;
#ifdef EKEKO_DEBUG
		printf("[Eon_Layout] %s %p adding damage rectangle %d %d %d %d\n", ekeko_object_type_name_get(c), c, r->x, r->y, r->w, r->h);
#endif
		/* if we only add a damage the process_cb wont be called, we need
		 * to inform somehow that the layout needs to be processed again
		 */
		eina_tiler_rect_add(prv->tiler, r);
		/* as other objects might call this function during
		 * ekeko_object_process() how to handle that situation?
		 */
		ekeko_value_bool_from(&v, EINA_TRUE);
		ekeko_object_property_value_set((Ekeko_Object *)c, "redraw", &v);
	}
}
/**
 * @brief Marks a rectangle area on the layout that will never be processed.
 * The area is kept on the layout until it is cleared with
 * layout_obscure_del()
 * @param r Rectangle that defines the obscure area
 */
EAPI void eon_layout_obscure_add(Eon_Layout *c, Eina_Rectangle *r)
{
	Eon_Layout_Private *prv;

	prv = PRIVATE(c);
	//_obscures_add(c, r);
}

EAPI Eon_Paint * eon_layout_renderable_get_at_coord(Eon_Layout *c,
		unsigned int x, unsigned int y)
{
	Eon_Layout_Private *prv;
	Eina_List *l;
	Eina_Rectangle igeom;

	prv = PRIVATE(c);
	if (!prv->renderables)
		return NULL;
	eina_rectangle_coords_from(&igeom, x, y, 1, 1);
	/* iterate from top most and find the renderable that matches the coords */
	for (l = eina_list_last(prv->renderables); l; l = eina_list_prev(l))
	{
		Eon_Paint *r;
		Eina_Rectangle rgeom;

		r = eina_list_data_get(l);
		eon_paint_boundings_get(r, &rgeom);
		if (!eina_rectangles_intersect(&igeom, &rgeom))
			continue;
		/* specific intersection */
		if (eon_paint_is_inside(r, x, y))
		{
#ifdef EKEKO_DEBUG
			printf("[Eon_Layout] renderable found %p\n", r);
#endif
			return r;
		}
#if RECURSIVE
		if (recursive)
		{
			if (ekeko_type_instance_is_of(r, "Canvas"))
			{
				Eon_Paint *subr;
				Eina_Rectangle rscaled;

				/* transform the coordinates */
				eina_rectangle_rescale_in(&rgeom, &igeom, &rscaled);
				subr = eon_layout_renderable_get_at_coord((Eon_Layout *)r,
						rscaled.x, rscaled.y, recursive);
				if (subr)
				{
#ifdef EKEKO_DEBUG
					printf("[Eon_Layout] Recursive, renderable found %p\n", subr);
#endif
					return subr;
				}
				else
				{
#ifdef EKEKO_DEBUG
					printf("[Eon_Layout] Recursive, no sublayout renderable found, so return %p\n", r);
#endif
					return r;
				}
			}
			else
			{
#ifdef EKEKO_DEBUG
				printf("[Eon_Layout] No recursive renderable found %p\n", r);
#endif
				return r;
			}
		}
		else
		{
#ifdef EKEKO_DEBUG
			printf("[Eon_Layout] renderable found %p\n", r);
#endif
			return r;
		}
#endif
	}
#ifdef EKEKO_DEBUG
	printf("[Eon_Layout] no renderable found\n");
#endif
	return NULL;
}

/* TODO add a focus in event */
EAPI void eon_layout_focus_set(Eon_Layout *l, Eon_Paint *p)
{
	Eon_Layout_Private *prv;

	prv = PRIVATE(l);
	prv->focused = p;
}

/* TODO add a focus out event */
EAPI Eon_Paint * eon_layout_focus_get(Eon_Layout *l)
{
	Eon_Layout_Private *prv;

	prv = PRIVATE(l);
	return prv->focused;
}

EAPI Eon_Input * eon_layout_input_new(Eon_Layout *l)
{
	Ekeko_Input *i;

	i = eon_input_new(l);
	return i;
}

