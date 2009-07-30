/* EKEKO - Object and property system
 * Copyright (C) 2007-2009 Jorge Luis Zapata
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
#include "Ekeko.h"
#include "ekeko_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(canvas) ((Ekeko_Canvas_Private *)((Ekeko_Canvas *)(canvas))->private)

#define TYPE_NAME "Canvas"

struct _Ekeko_Canvas_Private
{
	struct {
		Eina_Bool curr;
		Eina_Bool prev;
		int changed;
	} redraw;
	Eina_Tiler *tiler;
	Eina_List *renderables;
	/* TODO obscures */
	Eina_Inlist *inputs;
};

void _subcanvas_in(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	printf("SUBCANVAS in\n");
	/* TODO feed the mouse in into this canvas */
}

void _subcanvas_out(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	printf("SUBCANVAS out\n");
	/* TODO feed the mouse out into this canvas */
}

void _subcanvas_move(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	printf("SUBCANVAS move\n");
	/* TODO feed the mouse move into this canvas */
}


/*
 * TODO use this instead of the code below
 * this code is as it doesnt maintain the stack hierarchy
 */
static inline void _renderable_append(Ekeko_Canvas *c, Ekeko_Renderable *r,
		Eina_Rectangle *cgeom, Eina_Rectangle *rgeom, Eina_Bool rvisible)
{
	Eina_Bool intersect;
	Ekeko_Canvas_Private *prv;
	prv = PRIVATE(c);

#ifdef EKEKO_DEBUG
	printf("[canvas] %p renderable append %p at %d %d %d %d - %d %d %d %d (%d)\n",
			c, r, cgeom->x, cgeom->y, cgeom->w, cgeom->h,
			rgeom->x, rgeom->y, rgeom->w, rgeom->h,
			rvisible);
#endif

	intersect = eina_rectangles_intersect(rgeom, cgeom);
	/* not visible */
	if (!rvisible)
	{
		if (renderable_appended_get(r))
		{
#ifdef EKEKO_DEBUG
			printf("[canvas] %p removing renderable %p\n", c, r);
#endif
			prv->renderables = eina_list_remove(prv->renderables, r);
			renderable_appended_set(r, EINA_FALSE);
		}
	}
	/* visible and not intersect */
	else if (!intersect)
	{
		if (renderable_appended_get(r))
		{
#ifdef EKEKO_DEBUG
			printf("[canvas] %p removing renderable %p\n", c, r);
#endif
			prv->renderables = eina_list_remove(prv->renderables, r);
			renderable_appended_set(r, EINA_FALSE);
		}
	}
	/* visible and intersect */
	else
	{
		if (!renderable_appended_get(r))
		{
			Ekeko_Renderable *lr = NULL;
			Eina_List *l;
#ifdef EKEKO_DEBUG
			printf("[canvas] %p adding renderable %p\n", c, r);
#endif
			EINA_LIST_FOREACH(prv->renderables, l, lr)
			{
				if (!(ekeko_renderable_zindex_get(r) > ekeko_renderable_zindex_get(lr)))
				{
					prv->renderables = eina_list_prepend_relative(prv->renderables, r, lr);
					renderable_appended_set(r, EINA_TRUE);
					return;
				}
			}
			prv->renderables = eina_list_append_relative(prv->renderables, r, lr);
			renderable_appended_set(r, EINA_TRUE);
		}
	}
}

static void _child_geometry_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Ekeko_Canvas *c = (Ekeko_Canvas *)data;
	Ekeko_Renderable *r = (Ekeko_Renderable *)o;
	Eina_Rectangle cgeom;
	Eina_Bool rvisible;

#ifdef EKEKO_DEBUG
	printf("[canvas renderable %s]\n", ekeko_object_type_name_get(o));
#endif
	if (em->state != EVENT_MUTATION_STATE_POST)
		return;
	ekeko_renderable_geometry_get((Ekeko_Renderable *)c, &cgeom);
	/* reset the canvas coordinates to 0,0 WxH */
	cgeom.x = 0;
	cgeom.y = 0;
	ekeko_renderable_visibility_get(r, &rvisible);
	_renderable_append(c, r, &cgeom, &em->curr->value.rect, rvisible);

}

static void _child_visibility_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Ekeko_Canvas *c = (Ekeko_Canvas *)data;
	Ekeko_Renderable *r = (Ekeko_Renderable *)o;
	Eina_Rectangle rgeom, cgeom;

#ifdef EKEKO_DEBUG
	printf("[canvas renderable %s]\n", ekeko_object_type_name_get(o));
#endif
	if (em->state != EVENT_MUTATION_STATE_POST)
		return;
	ekeko_renderable_geometry_get((Ekeko_Renderable *)c, &cgeom);
	/* reset the canvas coordinates to 0,0 WxH */
	cgeom.x = 0;
	cgeom.y = 0;
	ekeko_renderable_geometry_get(r, &rgeom);
	_renderable_append(c, r, &cgeom, &rgeom, em->curr->value.bool_value);
}

static void _geometry_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Ekeko_Canvas_Private *prv;
	Eina_Tiler *tiler;

	if (em->state != EVENT_MUTATION_STATE_POST)
		return;

	prv = PRIVATE(((Ekeko_Canvas *)o));
	tiler = prv->tiler;
	if (tiler)
	{
		eina_tiler_del(tiler);
	}
#ifdef EKEKO_DEBUG
	printf("[canvas %s] Changing geometry\n", ekeko_object_type_name_get(o));
#endif
	prv->tiler = eina_tiler_new(em->curr->value.rect.w, em->curr->value.rect.h);
	/* TODO In case it already has a tiler, mark everything again */
	if (tiler)
	{
		eina_tiler_rect_add(prv->tiler, &em->curr->value.rect);
	}
#ifdef EKEKO_DEBUG
	printf("[canvas %s] tiler is %p\n", ekeko_object_type_name_get(o), prv->tiler);
#endif
}

/* Called whenever the process has finished on this element */
static void _process_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Canvas *c;
	Ekeko_Canvas_Private *prv;
	Eina_Iterator *it;
	Eina_Rectangle rect;

	c = (Ekeko_Canvas *)o;
#ifdef EKEKO_DEBUG
	printf("[canvas %s] Processing canvas %p\n", ekeko_object_type_name_get(o), o);
#endif
	prv = PRIVATE(c);
	if (!prv->tiler)
		return;
	/* TODO remove the obscures */
	/* get the tiler render areas */
	it = eina_tiler_iterator_new(prv->tiler);
	while (eina_iterator_next(it, (void **)&rect))
	{
		Eina_Iterator *rit;
		Ekeko_Renderable *r;

#ifdef EKEKO_DEBUG
		printf("[canvas] %p Redraw rectangle %d %d %d %d\n", o, rect.x, rect.y, rect.w, rect.h);
#endif
		/* iterate over the list of renderables */
		/* TODO from top to bottom ?*/
		rit = eina_list_iterator_new(prv->renderables);
		while (eina_iterator_next(rit, (void **)&r))
		{
			Eina_Rectangle geom;

			ekeko_renderable_geometry_get(r, &geom);
#ifdef EKEKO_DEBUG
			printf("[canvas] %p Rendering renderable %p (%d %d %d %d)\n", o, r, geom.x, geom.y, geom.w, geom.h);
#endif
			/* intersect the geometry and the damage area */
			if (!eina_rectangle_intersection(&geom, &rect))
				continue;
			/* call the draw function on the renderable */
			r->render(r, &geom);
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

static void _redraw_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Ekeko_Canvas_Private *prv;

	if (em->state != EVENT_MUTATION_STATE_POST)
		return;
	/* toggle property */
	em->curr->value.bool_value = EINA_FALSE;
}

static void _child_append_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Ekeko_Canvas_Private *prv;

	/* TODO check if object is the same as the event.rel or
	 * check if the event.target is not a canvas and it is different
	 * than this
	 */
	if (!ekeko_type_instance_is_of(e->target, "Renderable"))
	{
#ifdef EKEKO_DEBUG
		printf("[canvas %s] child is not of type renderable\n", ekeko_object_type_name_get(o));
#endif
		return;
	}
	/* TODO check that the child is actually an instance of a renderable type
	 * if so append it to the renderables
	 */
	/*
	 * TODO What happens if the child is of type renderable
	 * *and* not a canvas and has renderable objects?
	 */

	/* in case the child's canvas is not this, skip */
	if (ekeko_renderable_canvas_get((Ekeko_Renderable *)e->target) != (Ekeko_Canvas *)o)
	{
#ifdef EKEKO_DEBUG
		printf("[canvas] Skipping this %p renderable as it already has a canvas %p and is not this %p\n", e->target, ekeko_renderable_canvas_get((Ekeko_Renderable *)e->target), o);
#endif
		return;
	}
	/*
	 * TODO if the appended child is a canvas, register every UI event to this
	 * object, so when they arrive insert those events into the new canvas
	 */
	if (ekeko_type_instance_is_of(e->target, "Canvas"))
	{
#ifdef EKEKO_DEBUG
		printf("[canvas] Child is a canvas too, registering UI events\n");
#endif
		ekeko_event_listener_add((Ekeko_Object *)e->target, EKEKO_EVENT_UI_MOUSE_IN, _subcanvas_in, EINA_FALSE, NULL);
		ekeko_event_listener_add((Ekeko_Object *)e->target, EKEKO_EVENT_UI_MOUSE_OUT, _subcanvas_out, EINA_FALSE, NULL);
		ekeko_event_listener_add((Ekeko_Object *)e->target, EKEKO_EVENT_UI_MOUSE_MOVE, _subcanvas_move, EINA_FALSE, NULL);
	}
#ifdef EKEKO_DEBUG
	printf("[canvas %s] child of type renderable %s\n", ekeko_object_type_name_get(o), ekeko_object_type_name_get(e->target));
	printf("[canvas %s] related %s\n", ekeko_object_type_name_get(o), ekeko_object_type_name_get(em->related));
#endif

	prv = PRIVATE(em->related);
#ifdef EKEKO_DEBUG
	printf("[canvas %s] %p tiler = %p, canvas = %p\n", ekeko_object_type_name_get(o), em->related, prv->tiler, ekeko_renderable_canvas_get((Ekeko_Renderable *)o));
#endif
	ekeko_event_listener_add((Ekeko_Object *)e->target, EKEKO_RENDERABLE_VISIBILITY_CHANGED, _child_visibility_change, EINA_FALSE, o);
	ekeko_event_listener_add((Ekeko_Object *)e->target, EKEKO_RENDERABLE_GEOMETRY_CHANGED, _child_geometry_change, EINA_FALSE, o);
}

static void _ctor(void *instance)
{
	Ekeko_Canvas *canvas;
	Ekeko_Canvas_Private *prv;

	canvas = (Ekeko_Canvas*) instance;
	canvas->private = prv = ekeko_type_instance_private_get(ekeko_canvas_type_get(), instance);
	prv->renderables = NULL;
	prv->tiler = eina_tiler_new(0, 0);
	/* register to an event where some child is appended to this parent */
	ekeko_event_listener_add((Ekeko_Object *)canvas, EKEKO_EVENT_OBJECT_APPEND, _child_append_cb, EINA_TRUE, NULL);
	/* register the event where the size is changed */
	ekeko_event_listener_add((Ekeko_Object *)canvas, EKEKO_RENDERABLE_GEOMETRY_CHANGED, _geometry_change, EINA_FALSE, NULL);
	/* TODO add the event listener when the object has finished the process() function */
	ekeko_event_listener_add((Ekeko_Object *)canvas, EKEKO_EVENT_OBJECT_PROCESS, _process_cb, EINA_FALSE, NULL);
	ekeko_event_listener_add((Ekeko_Object *)canvas, EKEKO_CANVAS_REDRAW_CHANGED, _redraw_change, EINA_FALSE, NULL);
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Canvas] ctor %p %p, tiler = %p, canvas = %p\n", canvas, canvas->private, prv->tiler, ekeko_renderable_canvas_get((Ekeko_Renderable *)canvas));
#endif
}

static void _dtor(void *canvas)
{
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Canvas] dtor %p\n", canvas);
#endif
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EKEKO_CANVAS_REDRAW;

Ekeko_Type *ekeko_canvas_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(TYPE_NAME, sizeof(Ekeko_Canvas),
				sizeof(Ekeko_Canvas_Private), ekeko_renderable_type_get(),
				_ctor, _dtor, NULL);
		/* the properties */
		EKEKO_CANVAS_REDRAW = EKEKO_TYPE_PROP_DOUBLE_ADD(type, "redraw", EKEKO_PROPERTY_BOOL,
				OFFSET(Ekeko_Canvas_Private, redraw.curr), OFFSET(Ekeko_Canvas_Private, redraw.prev),
				OFFSET(Ekeko_Canvas_Private, redraw.changed));
	}

	return type;
}

EAPI Ekeko_Canvas * ekeko_canvas_new(void)
{
	Ekeko_Canvas *canvas;

	canvas = ekeko_type_instance_new(ekeko_canvas_type_get());

	return canvas;
}

EAPI void ekeko_canvas_size_set(Ekeko_Canvas *c, int w, int h)
{
	Eina_Rectangle rect;

	rect.x = 0;
	rect.y = 0;
	rect.w = w;
	rect.h = h;
	ekeko_renderable_geometry_set((Ekeko_Renderable *)c, &rect);
}
/**
 * @brief Marks a rectangle on the canvas as damaged, this area will be
 * processed again. When the canvas process that area it will no longer be
 * a damaged area
 * @param r Rectangle that defines the area damaged
 */
EAPI void ekeko_canvas_damage_add(Ekeko_Canvas *c, Eina_Rectangle *r)
{
	Ekeko_Canvas_Private *prv;

	prv = PRIVATE(c);
	if (prv->tiler)
	{
		Ekeko_Value v;
#ifdef EKEKO_DEBUG
		printf("[Ekeko_Canvas] %s %p adding damage rectangle %d %d %d %d\n", ekeko_object_type_name_get(c), c, r->x, r->y, r->w, r->h);
#endif
		/* if we only add a damage the process_cb wont be called, we need
		 * to inform somehow that the canvas needs to be processed again
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
 * @brief Marks a rectangle area on the canvas that will never be processed.
 * The area is kept on the canvas until it is cleared with
 * canvas_obscure_del()
 * @param r Rectangle that defines the obscure area
 */
EAPI void ekeko_canvas_obscure_add(Ekeko_Canvas *c, Eina_Rectangle *r)
{
	Ekeko_Canvas_Private *prv;

	prv = PRIVATE(c);
	//_obscures_add(c, r);
}

EAPI Ekeko_Input * ekeko_canvas_input_new(Ekeko_Canvas *c)
{
	Ekeko_Input *i;

	i = input_new(c);
	return i;
}

EAPI Ekeko_Renderable * ekeko_canvas_renderable_get_at_coord(Ekeko_Canvas *c,
		unsigned int x, unsigned int y)
{
	Ekeko_Canvas_Private *prv;
	Eina_List *l;
	Eina_Rectangle igeom;

	prv = PRIVATE(c);
	if (!prv->renderables)
		return NULL;
	eina_rectangle_coords_from(&igeom, x, y, 1, 1);
	/* iterate from top most and find the renderable that matches the coords */
	for (l = eina_list_last(prv->renderables); l; l = eina_list_prev(l))
	{
		Ekeko_Renderable *r;
		Eina_Rectangle rgeom;

		r = eina_list_data_get(l);
		ekeko_renderable_geometry_get(r, &rgeom);
		if (!eina_rectangles_intersect(&igeom, &rgeom))
			continue;
		/* specific intersection */
		if (ekeko_renderable_intersect(r, x, y))
		{
#ifdef EKEKO_DEBUG
			printf("[Ekeko_Canvas] renderable found %p\n", r);
#endif
			return r;
		}
#if RECURSIVE
		if (recursive)
		{
			if (ekeko_type_instance_is_of(r, "Canvas"))
			{
				Ekeko_Renderable *subr;
				Eina_Rectangle rscaled;

				/* transform the coordinates */
				eina_rectangle_rescale_in(&rgeom, &igeom, &rscaled);
				subr = ekeko_canvas_renderable_get_at_coord((Ekeko_Canvas *)r,
						rscaled.x, rscaled.y, recursive);
				if (subr)
				{
#ifdef EKEKO_DEBUG
					printf("[Ekeko_Canvas] Recursive, renderable found %p\n", subr);
#endif
					return subr;
				}
				else
				{
#ifdef EKEKO_DEBUG
					printf("[Ekeko_Canvas] Recursive, no subcanvas renderable found, so return %p\n", r);
#endif
					return r;
				}
			}
			else
			{
#ifdef EKEKO_DEBUG
				printf("[Ekeko_Canvas] No recursive renderable found %p\n", r);
#endif
				return r;
			}
		}
		else
		{
#ifdef EKEKO_DEBUG
			printf("[Ekeko_Canvas] renderable found %p\n", r);
#endif
			return r;
		}
#endif
	}
#ifdef EKEKO_DEBUG
	printf("[Ekeko_Canvas] no renderable found\n");
#endif
	return NULL;
}
