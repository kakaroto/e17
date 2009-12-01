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

#define EON_PAINT_DEBUG 0
/* TODO
 * add hints: min-w, max-w, min-h, max-h
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Paint_Private *)((Eon_Paint *)(d))->prv)

#define DBG(...) EINA_LOG_DOM_DBG(_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_dom, __VA_ARGS__)

static int _dom = -1;

struct _Eon_Paint_Private
{
	Eon_Layout *layout;
	Eina_Bool renderable;
	int zindex;
	/* transformed geometry */
	/* untransformed geometry */
	struct 
	{
		Eina_Rectangle curr;
		Eina_Rectangle prev;
		Eina_Bool changed;
	} boundings, geometry;
	/* number of properties (local and global)
	 * that have changed
	 */
	int changed;
	/* the properties */
	Eon_Color color;
	Enesim_Rop rop;
	Enesim_Matrix matrix;
	Enesim_Matrix inverse;
	Eon_Paint_Coordspace coordspace;
	Eon_Paint_Matrixspace matrixspace;
	Eon_Style *style;
	struct {
		Eina_Bool curr;
		Eina_Bool prev;
		Eina_Bool changed;
	} visibility;
};

static Eon_Paint * _prev_renderable_get(Ekeko_Object *o)
{
	Ekeko_Object *last;

	/* check if the object is a canvas, if so, dont go down */
	if (ekeko_type_instance_is_of(o, EON_TYPE_LAYOUT))
		return o;
	last = ekeko_object_child_last_get(o);
	if (!last)
	{
		if (ekeko_type_instance_is_of(o, EON_TYPE_PAINT))
			return o;
		else
			return NULL;
	}
	else
	{
		do
		{
			Ekeko_Object *g;

			g = _prev_renderable_get(last);
			if (g)
				return g;
		} while (last = ekeko_object_prev(last));
		/* any of the childs is a renderable, check the object itself */
		if (ekeko_type_instance_is_of(o, EON_TYPE_PAINT))
			return o;
		else
			return NULL;
	}
}

static Eon_Paint * _prev_renderable_left(Ekeko_Object *r)
{
	while (r = ekeko_object_prev(r))
	{
		Ekeko_Object *g;

		g = _prev_renderable_get(r);
		if (g)
			return (Eon_Paint *)g;
	}
	return NULL;
}

static Eon_Paint * _prev_renderable_up(Ekeko_Object *o)
{
	Ekeko_Object *parent;

	while (parent = ekeko_object_parent_get(o))
	{
		if (ekeko_type_instance_is_of(parent, EON_TYPE_PAINT))
			return (Eon_Paint *)parent;
		else
		{
			Eon_Paint *l;

			l = _prev_renderable_left(parent);
			if (l) return l;
		}
	}
}
/* FIXME fix this, we dont want to always trigger the layout change */
static void _paint_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	//eon_paint_change((Eon_Paint *)o);
}

static void _matrix_change(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Paint_Private *prv = PRIVATE(o);
	Enesim_Matrix *m;

	m = em->curr->value.pointer_value;
	enesim_matrix_inverse(m, &prv->inverse);
}

static void _parent_set_cb(Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Paint_Private *prv;
	Ekeko_Object *p = (Ekeko_Object *)em->related;
	Eon_Paint *last;
	Eon_Layout *l;

	prv = PRIVATE(o);
	/* set the layout this paint object belongs to */
	if (ekeko_type_instance_is_of(p, EON_TYPE_LAYOUT))
		prv->layout = p;
	else if (ekeko_type_instance_is_of(p, EON_TYPE_PAINT))
		prv->layout = eon_paint_layout_get(p);
	else
		return;

	last = _prev_renderable_left(o);
	/* no sibling with a renderable below or itself */
	if (!last)
		last = _prev_renderable_up(o);
	/* no parent with a renderable below */
	if (!last || last == p)
	{
		/* first element */
		/* Set the zindex */
	}
	else
	{
		int z;

		z = eon_paint_zindex_get(last);
		eon_paint_zindex_set(o, z + 1);
	}
	/* TODO propagate the change of zindex locally in case the object is not a canvas */
	/* TODO propagate the change of zindex to the next sibling */
}

static void _ctor(Ekeko_Object *o)
{
	Eon_Paint *p;
	Eon_Paint_Private *prv;

	p = (Eon_Paint *)o;
	p->prv = prv = ekeko_type_instance_private_get(eon_paint_type_get(), o);
	/* default values */
	prv->rop = ENESIM_BLEND;
	prv->color = 0xffffffff;
	enesim_matrix_identity(&prv->matrix);
	enesim_matrix_inverse(&prv->matrix, &prv->inverse);
	ekeko_event_listener_add(o, EKEKO_EVENT_PROP_MODIFY, _paint_change, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, EKEKO_EVENT_PARENT_SET, _parent_set_cb, EINA_FALSE, NULL);
	ekeko_event_listener_add(o, EON_PAINT_MATRIX_CHANGED, _matrix_change, EINA_FALSE, NULL);
}

static void _dtor(void *paint)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool eon_paint_appendable(Ekeko_Object *p, Ekeko_Object *child)
{
	if (!ekeko_type_instance_is_of(child, EON_TYPE_ANIMATION))
		return EINA_FALSE;
	else
		return EINA_TRUE;
}

/* Every paint object is a subclass of a renderable, just return the one there
 * which might be null in case this paint is attached to a style container
 */
Eon_Layout * eon_paint_layout_get(Eon_Paint *p)
{
	Eon_Paint_Private *prv = PRIVATE(p);

	return prv->layout;
}

Eon_Layout * eon_paint_layout_topmost_get(Eon_Paint *p)
{
	Eon_Paint_Private *prv;
	Eon_Layout *l, *last;

 	prv = PRIVATE(p);
	l = last = prv->layout;
	while (l)
	{
		Eon_Paint *pl = (Eon_Paint *)l;

		last = l;
		prv = PRIVATE(pl);
		l = prv->layout;
	}
	return last;
}

void eon_paint_unchange(Eon_Paint *p)
{
	Eon_Paint_Private *prv;

 	prv = PRIVATE(p);

	prv->changed--;
	if (!prv->layout)
		return;
	if (!prv->changed)
		eon_layout_unchange(prv->layout);
}

void eon_paint_change(Eon_Paint *p)
{
	Eon_Paint_Private *prv;
	Eon_Layout *l;

 	prv = PRIVATE(p);
	prv->changed++;
	if (!prv->layout)
		return;
	if (prv->changed == 1)
		eon_layout_change(prv->layout);

}

void eon_paint_style_inverse_matrix_get(Eon_Paint *p, Eon_Paint *rel,
		Enesim_Matrix *m)
{
	Eon_Paint_Private *prv = PRIVATE(p);
	Eon_Paint_Private *rprv;

	switch (prv->matrixspace)
	{
		case EON_MATRIXSPACE_OBJECT:
		rprv = PRIVATE(rel);
		*m = rprv->inverse;
		break;

		case EON_MATRIXSPACE_USER:
		*m = prv->inverse;
		break;

		case EON_MATRIXSPACE_COMPOSE:
		rprv = PRIVATE(rel);
		enesim_matrix_compose(m, &rprv->inverse, &prv->inverse);
		break;
	}
}

void eon_paint_inverse_matrix_get(Eon_Paint *p, Enesim_Matrix *m)
{
	Eon_Paint_Private *prv;

	prv = PRIVATE(p);
	*m = prv->inverse;
}

/* Sets the geometry for a renderable, if the shape has a transformation
 * matrix, it is aplied on the following way:
 * - multiply w,h * matrix
 * - translate the result to x, y
 *
 */
void eon_paint_geometry_set(Eon_Paint *p, int x, int y, int w, int h)
{
	Eon_Paint_Private *prv;
	Eon_Paint_Geometry_Change gch;
	Enesim_Matrix_Type mtype;
	Eina_Rectangle rect;

	prv = PRIVATE(p);

	DBG("Setting paint geometry %s:%p %d %d %d %d", ekeko_object_type_name_get(p), p, x, y, w, h);
	eina_rectangle_coords_from(&rect, x, y, w, h);
	mtype = enesim_matrix_type_get(&prv->matrix);
	if (mtype == ENESIM_MATRIX_IDENTITY)
	{
		prv->boundings.curr = prv->geometry.curr = rect;
	}
	else
	{
		Eina_Rectangle r;
		Enesim_Quad q;
		float x1, y1, x2, y2, x3, y3, x4, y4;

		eina_rectangle_coords_from(&r, 0, 0, rect.w, rect.h);
		/* get the largest rectangle that fits on the matrix */
		enesim_matrix_rect_transform(&prv->matrix, &r, &q);
		enesim_quad_coords_get(&q, &x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4);
		enesim_quad_rectangle_to(&q, &r);
		/* when scaling the area should scale too */
		r.x += rect.x - prv->matrix.xx;
		r.y += rect.y - prv->matrix.yy;
		r.w += prv->matrix.xx;
		r.h += prv->matrix.yy;

		prv->geometry.curr = rect;
		prv->boundings.curr = r;
	}
	/* check that the geometry have changed */
	if (eina_rectangle_equals(&prv->geometry.curr, &prv->geometry.prev)
			&& prv->geometry.changed)
	{
		prv->geometry.changed = EINA_FALSE;
		eon_paint_unchange(p);
	}
	else if (!eina_rectangle_equals(&prv->geometry.curr, &prv->geometry.prev)
			&& !prv->geometry.changed)
	{
		prv->geometry.changed = EINA_TRUE;
		eon_paint_change(p);
	}
	/* send the geometry update event */
	gch.geom = prv->geometry.curr;
	ekeko_event_init((Ekeko_Event *)&gch, EON_PAINT_GEOMETRY_CHANGED,
			(Ekeko_Object *)p, EINA_FALSE);
	ekeko_object_event_dispatch((Ekeko_Object *)p, (Ekeko_Event *)&gch);
	/* check that the boundings have changed */
	if (eina_rectangle_equals(&prv->boundings.curr, &prv->geometry.prev)
			&& prv->boundings.changed)
	{
		prv->boundings.changed = EINA_FALSE;
		eon_paint_unchange(p);
	}
	else if (!eina_rectangle_equals(&prv->boundings.curr, &prv->geometry.prev)
			&& !prv->boundings.changed)
	{
		prv->boundings.changed = EINA_TRUE;
		eon_paint_change(p);
	}
	/* send the boundings update event */
	gch.geom = prv->boundings.curr;
	ekeko_event_init((Ekeko_Event *)&gch, EON_PAINT_BOUNDINGS_CHANGED,
			(Ekeko_Object *)p, EINA_FALSE);
	ekeko_object_event_dispatch((Ekeko_Object *)p, (Ekeko_Event *)&gch);
}

Eina_Bool eon_paint_is_inside(Eon_Paint *p, int x, int y)
{
	Eon_Paint_Private *prv = PRIVATE(p);
	Enesim_Matrix_Type mtype;

	if (!p->is_inside)
		return EINA_TRUE;

	/* handle the transformation */
	mtype = enesim_matrix_type_get(&prv->inverse);
	if (mtype != ENESIM_MATRIX_IDENTITY)
	{
		float fx, fy;

		x -= prv->geometry.curr.x;
		y -= prv->geometry.curr.y;
		enesim_matrix_point_transform(&prv->inverse, x, y, &fx, &fy);
		x = fx + prv->geometry.curr.x;
		y = fy + prv->geometry.curr.y;

	}
	/* call the shape's is_inside */
	return p->is_inside(p, x, y);
}

Eina_Bool eon_paint_renderable_get(Eon_Paint *p)
{
	Eon_Paint_Private *prv = PRIVATE(p);

	return prv->renderable;
}

void eon_paint_renderable_set(Eon_Paint *p, Eina_Bool renderable)
{
	Eon_Paint_Private *prv = PRIVATE(p);

	prv->renderable = renderable;
}

int eon_paint_zindex_get(Eon_Paint *r)
{
	Eon_Paint_Private *prv;

	prv = PRIVATE(r);
	return prv->zindex;
}

void eon_paint_zindex_set(Eon_Paint *r, int zindex)
{
	Eon_Paint_Private *prv;

	prv = PRIVATE(r);
	prv->zindex = zindex;
}

Eina_Bool eon_paint_geometry_changed(Eon_Paint *p, Eina_Rectangle *curr,
		Eina_Rectangle *prev)
{
	Eon_Paint_Private *prv = PRIVATE(p);

	if (curr) *curr = prv->geometry.curr;
	if (prev) *prev = prv->geometry.prev;
	return prv->geometry.changed;
}

Eina_Bool eon_paint_boundings_changed(Eon_Paint *p, Eina_Rectangle *curr,
		Eina_Rectangle *prev)
{
	Eon_Paint_Private *prv = PRIVATE(p);

	if (curr) *curr = prv->boundings.curr;
	if (prev) *prev = prv->boundings.prev;
	return prv->boundings.changed;
}

Eina_Bool eon_paint_visibility_changed(Eon_Paint *p, Eina_Bool *curr,
		Eina_Bool *prev)
{
	Eon_Paint_Private *prv = PRIVATE(p);

	if (curr) *curr = prv->visibility.curr;
	if (prev) *prev = prv->visibility.prev;
	return prv->visibility.changed;
}

void eon_paint_process(Eon_Paint *p)
{
	Eon_Paint_Private *prv = PRIVATE(p);

	if (prv->boundings.changed)
	{
		prv->boundings.changed = EINA_FALSE;
		prv->boundings.prev = prv->boundings.curr;
		eon_paint_unchange(p);
		if (prv->layout)
		{
			eon_layout_damage_add(prv->layout, &prv->boundings.prev);
			eon_layout_damage_add(prv->layout, &prv->boundings.curr);
		}
	}
	if (prv->geometry.changed)
	{
		prv->geometry.changed = EINA_FALSE;
		prv->geometry.prev = prv->geometry.curr;
		eon_paint_unchange(p);
	}
	if (prv->visibility.changed)
	{
		prv->visibility.changed = EINA_FALSE;
		prv->visibility.prev = prv->visibility.curr;
		eon_paint_unchange(p);
	}
	return;
}

int eon_paint_changed(Eon_Paint *p)
{
	Eon_Paint_Private *prv = PRIVATE(p);

	return prv->changed;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_PAINT_COLOR;
Ekeko_Property_Id EON_PAINT_ROP;
Ekeko_Property_Id EON_PAINT_MATRIX;
Ekeko_Property_Id EON_PAINT_COORDSPACE;
Ekeko_Property_Id EON_PAINT_MATRIXSPACE;
Ekeko_Property_Id EON_PAINT_STYLE;
Ekeko_Property_Id EON_PAINT_VISIBILITY;

/**
 * Gets the type of a paint object
 * @return The type definition
 */
EAPI Ekeko_Type *eon_paint_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		_dom = eina_log_domain_register("eon_paint", NULL);

		type = ekeko_type_new(EON_TYPE_PAINT, sizeof(Eon_Paint),
				sizeof(Eon_Paint_Private),
				eon_object_type_get(),
				_ctor, _dtor, NULL);
		EON_PAINT_COLOR = EKEKO_TYPE_PROP_SINGLE_ADD(type, "color",
				 EON_PROPERTY_COLOR,
				OFFSET(Eon_Paint_Private, color));
		EON_PAINT_ROP = EKEKO_TYPE_PROP_SINGLE_ADD(type, "rop",
				EKEKO_PROPERTY_INT,
				OFFSET(Eon_Paint_Private, rop));
		EON_PAINT_MATRIX = EKEKO_TYPE_PROP_SINGLE_ADD(type, "matrix",
				EON_PROPERTY_MATRIX,
				OFFSET(Eon_Paint_Private, matrix));
		EON_PAINT_COORDSPACE = EKEKO_TYPE_PROP_SINGLE_ADD(type,
				"coordspace", EKEKO_PROPERTY_INT,
				OFFSET(Eon_Paint_Private, coordspace));
		EON_PAINT_MATRIXSPACE = EKEKO_TYPE_PROP_SINGLE_ADD(type,
				"matrixspace", EKEKO_PROPERTY_INT,
				OFFSET(Eon_Paint_Private, matrixspace));
		EON_PAINT_STYLE = EKEKO_TYPE_PROP_SINGLE_ADD(type, "style",
				EKEKO_PROPERTY_OBJECT,
				OFFSET(Eon_Paint_Private, style));
		EON_PAINT_VISIBILITY = EKEKO_TYPE_PROP_SINGLE_ADD(type,
				"visibility", EKEKO_PROPERTY_BOOL,
				OFFSET(Eon_Paint_Private, visibility.curr));
	}

	return type;
}

/**
 * Sets the transformation matrix on a paint
 * @param p The paint to set the transformation on
 * @param m The transformation matrix
 */
EAPI void eon_paint_matrix_set(Eon_Paint *p, Enesim_Matrix *m)
{
	Ekeko_Value v;

	eon_value_matrix_from(&v, m);
	ekeko_object_property_value_set((Ekeko_Object *)p, "matrix", &v);
}
/**
 * Gets the transformation matrix on a paint
 * @param p The paint object
 * @param m The transformation matrix to write the values to
 */
EAPI void eon_paint_matrix_get(Eon_Paint *p, Enesim_Matrix *m)
{
	Eon_Paint_Private *prv = PRIVATE(p);
	*m = prv->matrix;
}

EAPI void eon_paint_coordspace_set(Eon_Paint *p, Eon_Paint_Coordspace cs)
{
	Ekeko_Value v;

	ekeko_value_int_from(&v, cs);
	ekeko_object_property_value_set((Ekeko_Object *)p, "coordspace", &v);
}

EAPI Eon_Paint_Coordspace eon_paint_coordspace_get(Eon_Paint *p)
{
	Eon_Paint_Private *prv = PRIVATE(p);

	return prv->coordspace;
}

EAPI void eon_paint_matrixspace_set(Eon_Paint *p, Eon_Paint_Matrixspace cs)
{
	Ekeko_Value v;

	ekeko_value_int_from(&v, cs);
	ekeko_object_property_value_set((Ekeko_Object *)p, "matrixspace", &v);
}

EAPI Eon_Paint_Matrixspace eon_paint_matrixspace_get(Eon_Paint *p)
{
	Eon_Paint_Private *prv = PRIVATE(p);

	return prv->matrixspace;
}
/**
 * Sets the multiplication color of a paint
 * @param p The paint to set the color on
 * @param color The multiplcation color
 */
EAPI void eon_paint_color_set(Eon_Paint *p, Eon_Color color)
{
	Ekeko_Value v;

	eon_value_color_from(&v, color);
	ekeko_object_property_value_set((Ekeko_Object *)p, "color", &v);
}
/**
 * Gets the multiplicaiton color of a paint
 * @param p The paint to get the color from
 * @return The multiplication color
 */
EAPI Eon_Color eon_paint_color_get(Eon_Paint *p)
{
	Eon_Paint_Private *prv;

	prv = PRIVATE(p);
	return prv->color;
}
/**
 * Sets the raster operation for the paint objet.
 * The raster operation defines how the paint object should blend
 * against the lower paint objects
 * @param p The paint object to set the raster operation
 * @param rop The raster operation
 */
EAPI void eon_paint_rop_set(Eon_Paint *p, Enesim_Rop rop)
{
	Ekeko_Value v;

	ekeko_value_int_from(&v, rop);
	ekeko_object_property_value_set((Ekeko_Object *)p, "rop", &v);
}
/**
 * Get the raster operation of a paint object
 * @param p The paint object to get the raster operation from
 * @return The raster operation
 */
EAPI Enesim_Rop eon_paint_rop_get(Eon_Paint *p)
{
	Eon_Paint_Private *prv;

	prv = PRIVATE(p);
	return prv->rop;
}

EAPI void eon_paint_style_set(Eon_Paint *p, Eon_Style *s)
{
	Ekeko_Value v;

	ekeko_value_object_from(&v, s);
	ekeko_object_property_value_set((Ekeko_Object *)p, "style", &v);
}

EAPI Eon_Style * eon_paint_style_get(Eon_Paint *p)
{
	Eon_Paint_Private *prv;

	prv = PRIVATE(p);
	return prv->style;
}
/**
 * Gets the visibility of the paint object
 * @param p The paint object
 * @return EINA_TRUE when the object is visible or
 * EINA_FALSE if the object is hidden
 */
EAPI Eina_Bool eon_paint_visibility_get(Eon_Paint *p)
{
	Eon_Paint_Private *prv;

	prv = PRIVATE(p);
	return prv->visibility.curr;
}
/**
 * Shows a paint object
 * @param p The paint object
 */
EAPI void eon_paint_show(Eon_Paint *p)
{
	Eon_Paint_Private *prv;
	Ekeko_Value value;

	prv = PRIVATE(p);
	if (prv->visibility.curr)
		return;
	ekeko_value_bool_from(&value, EINA_TRUE);
	ekeko_object_property_value_set((Ekeko_Object *)p, "visibility", &value);
}
/**
 * Hides a paint object
 * @param p The paint object
 */
EAPI void eon_paint_hide(Eon_Paint *p)
{
	Eon_Paint_Private *prv;
	Ekeko_Value value;

	prv = PRIVATE(p);
	if (!prv->visibility.curr)
		return;
	ekeko_value_bool_from(&value, EINA_FALSE);
	ekeko_object_property_value_set((Ekeko_Object *)p, "visibility", &value);
}
/**
 * Sets the visibility of a paint
 * @param p The paint object
 * @param visible EINA_TRUE shows the object, EINA_FALSE hides it
 */
EAPI void eon_paint_visibility_set(Eon_Paint *p, Eina_Bool visible)
{
	Eon_Paint_Private *prv;
	Ekeko_Value value;

	prv = PRIVATE(p);
	if (prv->visibility.curr == visible)
		return;
	ekeko_value_bool_from(&value, visible);
	ekeko_object_property_value_set((Ekeko_Object *)p, "visibility", &value);
}
/**
 * Gets the bounding box of the transformed paint
 * @param p The paint object
 * @param bound The bounding box
 */
EAPI void eon_paint_boundings_get(Eon_Paint *p, Eina_Rectangle *bounds)
{
	Eon_Paint_Private *prv;

	prv = PRIVATE(p);
	*bounds = prv->boundings.curr;
}
/**
 * Gets the bounding box of the untransformed paint
 * @param p The paint object
 * @param rect The geometry of the object
 */
EAPI void eon_paint_geometry_get(Eon_Paint *p, Eina_Rectangle *rect)
{
	Eon_Paint_Private *prv;

	prv = PRIVATE(p);
	*rect = prv->geometry.curr;
}

