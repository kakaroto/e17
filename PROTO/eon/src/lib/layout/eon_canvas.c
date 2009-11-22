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
#define BOUNDING_DEBUG 0
#define PRIVATE(d) ((Eon_Canvas_Private *)((Eon_Canvas *)(d))->prv)

static Ekeko_Type *_type;

struct _Eon_Canvas_Private
{
	Eina_Bool root;
};
#if 0
/* in case the subcanvas has another canvas as parent it will blt to the
 * parent canvas
 */
static void _subcanvas_render(Eon_Paint *r, Eina_Rectangle *rect)
{
	Eina_Rectangle sgeom, srect;
	Eon_Canvas *c;
	Eon_Canvas_Private *sprv, *cprv;
	Eon_Engine *eng;

	sprv = PRIVATE(r);
	c = (Eon_Canvas *)ekeko_renderable_canvas_get(r);
	cprv = PRIVATE(c);

	eng = eon_document_engine_get(eon_object_document_get((Eon_Object *)r));
#if BOUNDING_DEBUG
	{
#if 0
		func->context->color_set(ctx, 0xffaaaaaa);
		func->context->rop_set(ctx, ENESIM_FILL);
		func->canvas->lock(cprv->s);
		func->shape->rect(cprv->s, ctx, rect->x, rect->y, rect->w, rect->h);
		func->canvas->unlock(cprv->s);
		func->context->delete(ctx);
#endif
	}
#endif
	{
		Enesim_Quad q;

		/* get the largest rectangle that fits on the matrix */
		enesim_matrix_rect_transform(&sprv->inverse, &sgeom, &q);
		enesim_quad_rectangle_to(&q, &srect);
	}
	srect.x = srect.y = -1;
	srect.w = 1 + sprv->w.final;
	srect.h = 1 + sprv->h.final;
	/* blt there */
#ifdef EON_DEBUG
	printf("[Eon_Canvas] Subcanvas render %d %d %d %d (%d %d %d %d)\n", srect.x, srect.y, srect.w, srect.h, rect->x, rect->y, rect->w, rect->h);
#endif
	eon_engine_canvas_blit(eng, sprv->engine_data, rect, cprv->engine_data, &srect);
}

static inline Eina_Bool _subcanvas_flush(Ekeko_Canvas *c, Eina_Rectangle *r)
{
	Eina_Rectangle rscaled;
	Eina_Rectangle cgeom;
	Ekeko_Canvas *dc; /* the canvas this subcanvas has */

	/* this canvas doesnt have a parent canvas? */
	dc = ekeko_renderable_canvas_get((Ekeko_Renderable *)c);
	if (!dc)
		return EINA_TRUE;
	ekeko_renderable_geometry_get((Ekeko_Renderable *)c, &cgeom);
	/* transform the rectangle relative to the upper canvas */
	eina_rectangle_rescale_out(&cgeom, r, &rscaled);
	printf("[Eon_Canvas] subcanvas adding a new damage %d %d %d %d (%d %d %d %d)\n",
			rscaled.x, rscaled.y, rscaled.w, rscaled.h,
			r->x, r->y, r->w, r->h);
	printf("[Eon_Canvas] subcanvas = %p, canvas = %p\n", c, dc);
	ekeko_canvas_damage_add(dc, &rscaled);
	return EINA_FALSE;
}

static Eina_Bool _flush(Ekeko_Canvas *c, Eina_Rectangle *r)
{
	Eon_Canvas_Private *prv;

	prv = PRIVATE(c);
	/* if root flip */
	if (prv->root)
	{
		Eon_Engine *eng;

#ifdef EON_DEBUG
		printf("[Eon_Canvas] flipping root surface\n");
#endif
		eng = eon_document_engine_get(prv->doc);
		return eon_engine_canvas_flush(eng, prv->engine_data, r);
	}
	/* otherwise blt */
	else
	{
		return _subcanvas_flush(c, r);
	}
}

static void _geometry_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Canvas_Private *prv = PRIVATE(o);
	Eon_Engine *eng;
	void *doc_data;
	int w, h;

	/* check if the change is the rectangle */
	if (em->state != EVENT_MUTATION_STATE_POST)
		return;

	if (!prv->doc)
	{
		printf("EEEEEEEEEEEEERRRRRRRRRRRRRRROOOOOOOOOOR\n");
		exit(1);
	}

	/* TODO add the x and y too */
	/* TODO check that the w and h have changed */
	w = prv->w.final;
	h = prv->h.final;
	eng = eon_document_engine_get(prv->doc);
	doc_data = eon_document_engine_data_get(prv->doc);
	if (prv->engine_data)
		eon_engine_canvas_delete(eng, prv->engine_data);
	prv->engine_data = eon_engine_canvas_create(eng, doc_data, (Eon_Canvas *)o, prv->root, w, h);
}
#endif
static Eina_Bool _flush(Eon_Layout *l, Eina_Rectangle *r)
{

}

static void _subcanvas_render(Eon_Paint *p, Eina_Rectangle *rect)
{

}

static Eina_Bool _subcanvas_is_inside(Eon_Canvas *c, int x, int y)
{
	return EINA_TRUE;
}

static void _child_appended(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Canvas *c = (Eon_Canvas *)o;
	Eon_Canvas_Private *prv = PRIVATE(c);
	Eon_Coord x, y, w, h, zero;

	/* whenever a child is appended the relative coordinates should
	 * be calculated from this geometry
	 */
	if (ekeko_type_instance_is_of(e->target, EON_TYPE_PAINT_SQUARE))
	{
		Eon_Paint_Square *ps = EON_PAINT_SQUARE(e->target);

		eon_paint_square_x_get(ps, &x);
		eon_paint_square_y_get(ps, &y);
		eon_paint_square_w_get(ps, &w);
		eon_paint_square_h_get(ps, &h);
	}
	else if (ekeko_type_instance_is_of(e->target, EON_TYPE_SHAPE_SQUARE))
	{
		Eon_Shape_Square *ss = EON_PAINT_SQUARE(e->target);

		eon_shape_square_x_get(ss, &x);
		eon_shape_square_y_get(ss, &y);
		eon_shape_square_w_get(ss, &w);
		eon_shape_square_h_get(ss, &h);
	}
	else return;

}

static void _child_append_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Canvas *c;
	Eon_Canvas_Private *prv;
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;

	/* FIXME the canvas can only have one parent, either a document
	 * or a canvas
	 */
	c = (Eon_Canvas *)o;
	prv = PRIVATE(c);

	/* TODO whenever we attach to any parent, update the final oordinates
	 * in case the user has already set them up before attaching
	 */
	/* in case the parent is a document retrieve the engine
	 * in case the parent is a canvas retrieve the engine from the relative
	 * document
	 */
	/* parent is a document */
	if (!ekeko_type_instance_is_of(em->related, EON_TYPE_CANVAS))
	{
		prv->root = EINA_TRUE;
	}
}

static Eina_Bool _appendable(void *instance, void *child)
{
	if ((!ekeko_type_instance_is_of(child, EON_TYPE_CANVAS)) &&
			(!ekeko_type_instance_is_of(child, EON_TYPE_PAINT)) &&
			(!ekeko_type_instance_is_of(child, EON_TYPE_EXTERNAL)) &&
			(!ekeko_type_instance_is_of(child, EON_TYPE_ANIMATION)))
		return EINA_FALSE;
	return EINA_TRUE;
}

static void _ctor(Ekeko_Object *o)
{
	Eon_Canvas *c;
	Eon_Canvas_Private *prv;

	c = (Eon_Canvas *)o;
	c->prv = prv = ekeko_type_instance_private_get(_type, o);
	c->base.flush = _flush;
	c->base.base.parent.render = _subcanvas_render;
	c->base.base.parent.is_inside = _subcanvas_is_inside;
	//ekeko_event_listener_add(o, EON_PAINT_GEOMETRY_CHANGED, _geometry_change, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, EKEKO_EVENT_OBJECT_APPEND, _child_append_cb, EINA_FALSE, NULL);
}

static void _dtor(void *canvas)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_canvas_init(void)
{
	_type = ekeko_type_new(EON_TYPE_CANVAS, sizeof(Eon_Canvas),
			sizeof(Eon_Canvas_Private), eon_layout_type_get(),
			_ctor, _dtor, _appendable);

	eon_type_register(_type, EON_TYPE_CANVAS);
}

void eon_canvas_shutdown(void)
{
	eon_type_unregister(_type);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Creates a new canvas layout
 * @param d The document this canvas will be created on
 * @return The newly created canvas
 */
EAPI Eon_Canvas * eon_canvas_new(Eon_Document *d)
{
	Eon_Canvas *c;

	c = eon_document_object_new(d, EON_TYPE_CANVAS);

	return c;
}
