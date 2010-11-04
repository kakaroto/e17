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
/* TODO
 * a layout should have a background paint
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(o) ((Eon_Layout_Private *)((Eon_Layout *)(o))->prv)

#define DBG(...) EINA_LOG_DOM_DBG(_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_dom, __VA_ARGS__)

static int _dom = -1;

struct _Eon_Layout_Private
{
	int relative;
	Eina_Tiler *tiler;
	Eina_List *renderables;
	Eina_List *obscures;
	Eina_List *paints;
	Eina_Inlist *inputs;
	Eon_Paint *focused;
	/* number of renderable object that have changed */
	int changed;
	/* Enesim_Surface *surface */
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

	DBG("%p renderable append %s:%p at %d %d %d %d - %d %d %d %d (%d)",
			c, ekeko_object_type_name_get(r), r,
			cgeom->x, cgeom->y, cgeom->w, cgeom->h,
			rgeom->x, rgeom->y, rgeom->w, rgeom->h,
			rvisible);

	intersect = eina_rectangles_intersect(rgeom, cgeom);
	/* not visible */
	if (!rvisible)
	{
		if (eon_paint_renderable_get(r))
		{
			DBG("%p removing renderable %p", c, r);
			prv->renderables = eina_list_remove(prv->renderables, r);
			eon_paint_renderable_set(r, EINA_FALSE);
		}
	}
	/* visible and not intersect */
	else if (!intersect)
	{
		if (eon_paint_renderable_get(r))
		{
			DBG("%p removing renderable %p", c, r);
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

		DBG("%p adding renderable %p", c, r);
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

#if 0
static void _child_geometry_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Layout *c = (Eon_Layout *)data;
	Eon_Paint *r = (Eon_Paint *)o;
	Eina_Rectangle cgeom;
	Eina_Bool rvisible;

	printf("geometry changed!\n");
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
#endif

static void _tiler_update(Eon_Layout *l, Eina_Rectangle *area)
{
	Eina_Tiler *tiler;
	Eon_Layout_Private *prv;

	prv = PRIVATE(l);
	tiler = prv->tiler;
	if (tiler)
	{
		eina_tiler_free(tiler);
	}
	prv->tiler = eina_tiler_new(area->w, area->h);
	if (tiler)
	{
		eina_tiler_rect_add(prv->tiler, area);
	}
}

static void _child_append_cb(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Layout_Private *prv;
	Eon_Paint *p;

	/* TODO
	 * + check if object is the same as the event.rel or
	 * check if the event.target is not a layout and it is different
	 * than this
	 * + check that the child is actually an instance of a renderable type
	 * if so append it to the renderables
	 * + What happens if the child is of type renderable
	 * *and* not a layout and has renderable objects?
	 */
	if (!ekeko_type_instance_is_of(em->related, EON_TYPE_PAINT))
		return;

	/*
	 * TODO if the appended child is a layout, register every UI event to this
	 * object, so when they arrive insert those events into the new layout
	 */
	if (ekeko_type_instance_is_of(em->related, EON_TYPE_LAYOUT))
	{
		DBG("Child is a layout too, registering UI events");
		ekeko_event_listener_add(e->target, EON_EVENT_UI_MOUSE_IN,
				_sublayout_in, EINA_FALSE, NULL);
		ekeko_event_listener_add(e->target, EON_EVENT_UI_MOUSE_OUT,
				_sublayout_out, EINA_FALSE, NULL);
		ekeko_event_listener_add(e->target, EON_EVENT_UI_MOUSE_MOVE,
				_sublayout_move, EINA_FALSE, NULL);
	}
	DBG("%s child of type renderable %s", ekeko_object_type_name_get(o),
			ekeko_object_type_name_get(e->target));
	DBG("%s related %s", ekeko_object_type_name_get(o),
			ekeko_object_type_name_get(em->related));

	prv = PRIVATE(o);
	p = (Eon_Paint *)em->related;
	if (eon_paint_changed(p))
	{
		prv->changed++;
	}
#if 0
	prv->paints = eina_list_append(prv->paints, em->related);
	ekeko_event_listener_add(e->target, EON_PAINT_VISIBILITY_CHANGED,
			_child_visibility_change, EINA_FALSE, o);
	ekeko_event_listener_add(e->target, EON_PAINT_GEOMETRY_CHANGED,
			_child_geometry_change, EINA_FALSE, o);
	ekeko_event_listener_add(e->target, EKEKO_EVENT_OBJECT_REMOVE,
			_child_removed, EINA_FALSE, o);
#endif
}

static void _document_size_changed(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Document_Size_Change *sz = (Eon_Document_Size_Change *)e;
	Eon_Paint_Square *ps = (Eon_Paint_Square *)data;
	Eon_Coord x, y, w, h;

	//eon_paint_square_coords_get(ps, &x, &y, &w, &h);
	if (x.type == EON_COORD_RELATIVE)
	{
		eon_coord_relative_calculate(&x, 0, sz->geom.w, &x.final);
	}
	if (y.type == EON_COORD_RELATIVE)
	{
		eon_coord_relative_calculate(&y, 0, sz->geom.h, &y.final);
	}
	if (w.type == EON_COORD_RELATIVE)
	{
		eon_coord_length_relative_calculate(&w, sz->geom.w, &w.final);
	}
	if (h.type == EON_COORD_RELATIVE)
	{
		eon_coord_length_relative_calculate(&h, sz->geom.h, &h.final);
	}
	//eon_paint_square_geometry_set(ps, x.final, y.final, w.final, h.final);
}

static void _layout_x_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Coord *prev, *curr;
	Eon_Document *doc;
	Ekeko_Object *parent;
	Eon_Layout_Private *prv;
	int w;

	parent = ekeko_object_parent_get(o);
	if (!parent || !ekeko_type_instance_is_of(parent, EON_TYPE_DOCUMENT))
		return;

	prv = PRIVATE(o);
	doc = (Eon_Document *)parent;
	prev = em->prev->value.pointer_value;
	curr = em->curr->value.pointer_value;

	eon_document_size_get(doc, &w, NULL);
	eon_coord_change2(o, curr, prev, &prv->relative,
			0, w, parent, EON_DOCUMENT_SIZE_CHANGED,
			_document_size_changed);
}

static void _layout_y_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Coord *prev, *curr;
	Eon_Document *doc;
	Ekeko_Object *parent;
	Eon_Layout_Private *prv;
	int h;

	parent = ekeko_object_parent_get(o);
	if (!parent || !ekeko_type_instance_is_of(parent, EON_TYPE_DOCUMENT))
		return;

	prv = PRIVATE(o);
	doc = (Eon_Document *)parent;
	prev = em->prev->value.pointer_value;
	curr = em->curr->value.pointer_value;

	eon_document_size_get(doc, NULL, &h);
	eon_coord_change2(o, curr, prev, &prv->relative,
			0, h, parent, EON_DOCUMENT_SIZE_CHANGED,
			_document_size_changed);
}

static void _layout_w_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Coord *prev, *curr;
	Eon_Document *doc;
	Ekeko_Object *parent;
	Eon_Layout_Private *prv;
	int w;

	parent = ekeko_object_parent_get(o);
	if (!parent || !ekeko_type_instance_is_of(parent, EON_TYPE_DOCUMENT))
		return;

	prv = PRIVATE(o);
	doc = (Eon_Document *)parent;
	prev = em->prev->value.pointer_value;
	curr = em->curr->value.pointer_value;

	eon_document_size_get(doc, &w, NULL);
	eon_coord_length_change2(o, curr, prev, &prv->relative,
			w, parent, EON_DOCUMENT_SIZE_CHANGED,
			_document_size_changed);
}

static void _layout_h_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Coord *prev, *curr;
	Eon_Document *doc;
	Ekeko_Object *parent;
	Eon_Layout_Private *prv;
	int h;

	parent = ekeko_object_parent_get(o);
	if (!parent || !ekeko_type_instance_is_of(parent, EON_TYPE_DOCUMENT))
		return;

	prv = PRIVATE(o);
	doc = (Eon_Document *)parent;
	prev = em->prev->value.pointer_value;
	curr = em->curr->value.pointer_value;

	eon_document_size_get(doc, NULL, &h);
	eon_coord_length_change2(o, curr, prev, &prv->relative,
			h, parent, EON_DOCUMENT_SIZE_CHANGED,
			_document_size_changed);
}

static void _parent_set(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Paint_Square *ps = (Eon_Paint_Square *)o;
	Eon_Layout_Private *prv;
	Eon_Document *doc;
	Eon_Coord cx, cy, cw, ch;
	Eina_Rectangle geometry;
	int w, h;

	if (!ekeko_type_instance_is_of(em->related, EON_TYPE_DOCUMENT))
		return;
	/* in case we had some coord relative set the needed callbacks
	 * and update the final geometry
	 */
	prv = PRIVATE(o);
	doc = (Eon_Document *)em->related;
	eon_document_size_get(doc, &w, &h);
	//eon_paint_square_coords_get(ps, &cx, &cy, &cw, &ch);
	if (cx.type == EON_COORD_RELATIVE)
	{
		eon_coord_relative_calculate(&cx, 0, w, &cx.final);
		prv->relative++;
	}
	if (cy.type == EON_COORD_RELATIVE)
	{
		eon_coord_relative_calculate(&cy, 0, h, &cy.final);
		prv->relative++;
	}
	if (cw.type == EON_COORD_RELATIVE)
	{
		eon_coord_length_relative_calculate(&cw, w, &cw.final);
		prv->relative++;
	}
	if (ch.type == EON_COORD_RELATIVE)
	{
		eon_coord_length_relative_calculate(&ch, h, &ch.final);
		prv->relative++;
	}
	if (prv->relative)
	{
		ekeko_event_listener_add(em->related,
				EON_DOCUMENT_SIZE_CHANGED,
				_document_size_changed,
				EINA_FALSE, o);
		//eon_paint_square_geometry_set(ps, cx.final, cy.final,
		//		cw.final, ch.final);
	}
}

static void _parent_unset(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Paint_Square *ps = (Eon_Paint_Square *)o;
	Eon_Coord cx, cy, cw, ch;
	Eon_Layout_Private *prv;

	if (!ekeko_type_instance_is_of(em->related, EON_TYPE_DOCUMENT))
		return;

	prv = PRIVATE(o);
	//eon_paint_square_coords_get(ps, &cx, &cy, &cw, &ch);
	/* remove any callback relative to the size of the document */
	if (prv->relative)
	{
		ekeko_event_listener_remove(em->related,
				EON_DOCUMENT_SIZE_CHANGED,
				_document_size_changed,
				EINA_FALSE, o);
		prv->relative = 0;
	}
}
/*----------------------------------------------------------------------------*
 *                                  Events                                    *
 *----------------------------------------------------------------------------*/
static void _property_get(Ekeko_Object *o, Ekeko_Event *e, void *data)
{

}
static void _property_set(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	printf("property set\n");
}
/*----------------------------------------------------------------------------*
 *                           Base Type functions                              *
 *----------------------------------------------------------------------------*/
static void _ctor(Ekeko_Object *o)
{
	Eon_Layout *layout;
	Eon_Layout_Private *prv;

	layout = (Eon_Layout *)o;
	layout->prv = prv = ekeko_type_instance_private_get(eon_layout_type_get(), o);
	prv->renderables = NULL;
	prv->tiler = eina_tiler_new(0, 0);
	/* events */
	ekeko_event_listener_add(o, EKEKO_EVENT_VALUE_GET,
			_property_get, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, EKEKO_EVENT_VALUE_SET,
			_property_set, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, EKEKO_EVENT_OBJECT_APPEND,
			_child_append_cb, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, EKEKO_EVENT_PARENT_SET,
			_parent_set, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, EKEKO_EVENT_PARENT_UNSET,
			_parent_unset, EINA_FALSE, NULL);
}

static void _dtor(Ekeko_Object *o)
{
	Eon_Layout_Private *prv;

	prv = PRIVATE(o);
	eina_tiler_free(prv->tiler);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool eon_layout_appendable(Ekeko_Object *parent, Ekeko_Object *child)
{
	if (!ekeko_type_instance_is_of(child, EON_TYPE_PAINT))
		return EINA_FALSE;
	return EINA_TRUE;
}

void eon_layout_process(Eon_Layout *l)
{
	Eon_Layout_Private *prv = PRIVATE(l);
	Eon_Engine *e;
	Eina_Rectangle geom;
        Eina_Rectangle *rect;
	Eon_Document *doc;
	Ekeko_Object *ch;
	Eina_Iterator *it;
	void *dd, *ld;

	doc = eon_object_document_get((Eon_Object *)l);
	dd = eon_document_engine_data_get(doc);
	e = eon_document_engine_get(doc);

	ld = eon_object_engine_data_get((Eon_Object *)l);
	if (!eon_paint_changed((Eon_Paint *)l))
		goto childs;
	if (eon_paint_geometry_changed((Eon_Paint *)l, &geom, NULL))
	{
		/* first create the new tiler */
		_tiler_update(l, &geom);
		/* create the surface that holds this layout */
#if 0
		if (ld) eon_engine_layout_delete(e, ld);
#endif

#if 0
		ld = eon_engine_layout_create(e, l, dd, geom.w, geom.h);
#endif
		eon_object_engine_data_set((Eon_Object *)l, ld);
		goto renderables;
	}
childs:
	if (!prv->changed) goto tiles;
	/* iterate and process the paint objects while there's
	 * no more changed ones
	 */
renderables:
	ch = ekeko_object_child_first_get((Ekeko_Object *)l);
	do
	{
		Eina_Rectangle prect, crect;
		Eina_Bool pvis, cvis;
		Eon_Paint *p;
		int pchanged;

		if (!ekeko_type_instance_is_of(ch, EON_TYPE_PAINT))
			continue;

		p = (Eon_Paint *)ch;
		/* check that the paint object has changed the geometry
		 * and the visibility, if so, append or remove
		 * from the list of renderables
		 */
		eon_paint_boundings_changed(p, &crect, NULL);
		eon_paint_visibility_changed(p, &cvis, NULL);
		_renderable_append(l, p, &geom, &crect, cvis);
		if (!(pchanged = eon_paint_changed(p)))
			continue;
		if (p->process) p->process(p);
	} while (ch = ekeko_object_next(ch));
tiles:
	/* get the tiler render areas */
	it = eina_tiler_iterator_new(prv->tiler);
	while (eina_iterator_next(it, (void **)rect))
	{
		Eina_Iterator *rit;
		Eon_Paint *r;

		DBG("%p redraw rectangle %d %d %d %d", l, rect->x, rect->y, rect->w, rect->h);
		/* iterate over the list of renderables */
		rit = eina_list_iterator_new(prv->renderables);
		while (eina_iterator_next(rit, (void **)&r))
		{
			Eina_Rectangle geom;
			void *pdata;

			eon_paint_boundings_get(r, &geom);
			/* intersect the geometry and the damage area */
			if (!eina_rectangle_intersection(&geom, rect))
				continue;
			DBG("%p rendering renderable %s:%p (%d %d %d %d)", l, ekeko_object_type_name_get(r), r, geom.x, geom.y, geom.w, geom.h);
#if 0
#if BOUNDING_DEBUG
			eon_engine_debug_rect(e, surface, 0xffaaaaaa, rect->x, rect->y, rect->w, rect->h);
#endif
#endif
			/* FIXME */
			pdata = eon_object_engine_data_get((Eon_Object *)r);
			if (!pdata)
			{
				pdata = r->create(e, r);
				eon_object_engine_data_set((Eon_Object *)r, pdata);
			}
			/* call the draw function on the renderable */
			r->render(r, e, pdata, ld, &geom);
		}
		eina_iterator_free(rit);
	}
	eina_iterator_free(it);

	/* iterate over the redraw rectangles and flush */
	it = eina_tiler_iterator_new(prv->tiler);
	while (eina_iterator_next(it, (void **)rect))
	{
		DBG("%p flushing %d %d %d %d", l, rect->x, rect->y, rect->w, rect->h);
#if 0
		if (eon_engine_layout_flush(e, ld, rect))
			break;
#endif
#if 0
		if (l->flush(l, rect))
			break;
#endif
	}
	eina_iterator_free(it);
	/* clear the tiler */
	eina_tiler_clear(prv->tiler);
	if (!eon_paint_changed((Eon_Paint *)l))
		return;
	eon_paint_process((Eon_Paint *)l);
}

void eon_layout_change(Eon_Layout *l)
{
	Eon_Layout_Private *prv = PRIVATE(l);

	prv->changed++;
}

void eon_layout_unchange(Eon_Layout *l)
{
	Eon_Layout_Private *prv = PRIVATE(l);

	prv->changed--;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Gets the type of a layout object
 * @return The type definition
 */
Ekeko_Type * eon_layout_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		_dom = eina_log_domain_register("eon_layout", NULL);
		type = ekeko_type_new(EON_TYPE_LAYOUT, sizeof(Eon_Layout),
				sizeof(Eon_Layout_Private),
				eon_paint_type_get(),
				_ctor, _dtor, NULL);
	}

	return type;
}
/**
 * Marks a rectangle on the layout as damaged, this area will be
 * processed again. When the layout process that area it will no longer be
 * a damaged area
 * @param c The layout
 * @param r Rectangle that defines the area damaged
 */
EAPI void eon_layout_damage_add(Eon_Layout *l, Eina_Rectangle *r)
{
	Eon_Layout_Private *prv;
	Ekeko_Value v;

	prv = PRIVATE(l);
	if (!prv->tiler)
		return;

	DBG("%p adding damage rectangle %d %d %d %d", l,
			r->x, r->y, r->w, r->h);
	eina_tiler_rect_add(prv->tiler, r);
	/* if we only add a damage the process_cb wont be called, we need
	 * to inform somehow that the layout needs to be processed again
	 */
#if 0
	/* as other objects might call this function during
	 * ekeko_object_process() how to handle that situation?
	 */
	ekeko_value_bool_from(&v, EINA_TRUE);
	ekeko_object_property_value_set((Ekeko_Object *)l, "redraw", &v);
#endif
}
/**
 * Marks a rectangle area on the layout that will never be processed.
 * The area is kept on the layout until it is cleared with
 * layout_obscure_del()
 * @param c The layout
 * @param r Rectangle that defines the obscure area
 */
EAPI void eon_layout_obscure_add(Eon_Layout *c, Eina_Rectangle *r)
{
	Eon_Layout_Private *prv;

	prv = PRIVATE(c);
	//_obscures_add(c, r);
}
/**
 * Gets the paint object that is at coordinates x, y. The paint
 * object is visible (renderable)
 * @param l The layout to search for the renderable
 * @param x The horiziontal coordinate to search for the object
 * @param y The vertical coordinate to search for the object
 * @return The renderable object if there's one at the coordinates
 * or NULL otherwise
 */
EAPI Eon_Paint * eon_layout_renderable_get_at_coord(Eon_Layout *l,
		unsigned int x, unsigned int y)
{
	Eon_Layout_Private *prv;
	Eina_List *el;
	Eina_Rectangle igeom;

	prv = PRIVATE(l);
	if (!prv->renderables)
		return NULL;
	eina_rectangle_coords_from(&igeom, x, y, 1, 1);
	/* iterate from top most and find the renderable that matches the coords */
	for (el = eina_list_last(prv->renderables); el; el = eina_list_prev(el))
	{
		Eon_Paint *r;
		Eina_Rectangle rgeom;

		r = eina_list_data_get(el);
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
			if (ekeko_type_instance_is_of(r, EON_TYPE_LAYOUT))
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
	Eon_Input *i;

	i = eon_input_new(l);
	return i;
}

