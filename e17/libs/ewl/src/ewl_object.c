
#include <Ewl.h>

/**
 * ewl_object_init - initialize the fields of an object
 * @o: the object to initialize
 *
 * Returns no value. Sets all of the fields of the object @o to their default
 * values.
 */
void
ewl_object_init(Ewl_Object * o)
{
	DCHECK_PARAM_PTR("o", o);

	MINIMUM_W(o) = 1;
	MINIMUM_H(o) = 1;
	MAXIMUM_W(o) = 1 << 30;
	MAXIMUM_H(o) = 1 << 30;

	CURRENT_W(o) = REQUEST_W(o) = 100;
	CURRENT_H(o) = REQUEST_H(o) = 100;

	o->fill_policy = EWL_FILL_POLICY_NORMAL;
	o->alignment = EWL_ALIGNMENT_LEFT | EWL_ALIGNMENT_TOP;
}

/**
 * ewl_object_set_current_geometry - set the current size and posiiton of object
 * @o: the object to change
 * @x: the new x coordinate of the object
 * @y: the new y coordinate of the object
 * @w: the new width of the object
 * @h: the new height of the object
 *
 * Returns no value. Sets the geometry of @o to the size and position
 * specified.
 */
void
ewl_object_set_current_geometry(Ewl_Object * o, int x, int y, int w, int h)
{
	DCHECK_PARAM_PTR("o", o);

	CURRENT_X(o) = x;
	CURRENT_Y(o) = y;
	CURRENT_W(o) = w;
	CURRENT_H(o) = h;

	if (CURRENT_W(o) < MINIMUM_W(o))
		CURRENT_W(o) = MINIMUM_W(o);
	else if (w > MAXIMUM_W(o))
		CURRENT_W(o) = MAXIMUM_W(o);

	if (CURRENT_H(o) < MINIMUM_H(o))
		CURRENT_H(o) = MINIMUM_H(o);
	else if (w > MAXIMUM_H(o))
		CURRENT_H(o) = MAXIMUM_H(o);
}

/**
 * ewl_object_get_current_geometry - retrieve the size and position of object
 * @o: the object to retrieve size and position
 * @x: a pointer to the integer to store the x coordinate
 * @y: a pointer to the integer to store the y coordinate
 * @w: a pointer to the integer to store the width
 * @h: a pointer to the integer to store the height
 *
 * Returns no value. Examines @o and stores it's size and position into the
 * integers pointed to by the parameters @x, @y, @w, and @h.
 */
void
ewl_object_get_current_geometry(Ewl_Object * o,
				int *x, int *y, int *w, int *h)
{
	DCHECK_PARAM_PTR("o", o);

	if (x)
		*x = o->current.x;
	if (y)
		*y = o->current.y;
	if (w)
		*w = o->current.w;
	if (h)
		*h = o->current.h;
}

/**
 * ewl_object_set_current_size - set the current size of the object
 * @o: the object to change size
 * @w: the new width of the object
 * @h: the new height of the object
 *
 * Returns no value. The dimensions of the object @o are set to the values of
 * the parameters @w, and @h unless these values are greater than the objects
 * maximum value or smaller than the objects minimum value. If they are
 * outside these bounds, the size is not altered.
 */
void
ewl_object_set_current_size(Ewl_Object * o, int w, int h)
{
	DCHECK_PARAM_PTR("o", o);

	if (w >= MINIMUM_W(o) && w <= MAXIMUM_W(o))
		o->current.w = w;

	if (h >= MINIMUM_H(o) && h <= MAXIMUM_H(o))
		o->current.h = h;
}

/**
 * ewl_object_get_current_size - retrieve the current size of an object
 * @o: the object to retrieve size information
 * @w: a pointer to the integer to store the width of the object
 * @h: a pointer to the integer to store the height of the object
 *
 * Returns no value. Stores the width and height of object @o into @w and @h
 * respectively.
 */
void
ewl_object_get_current_size(Ewl_Object * o, int *w, int *h)
{
	DCHECK_PARAM_PTR("o", o);

	if (w)
		*w = o->current.w;
	if (h)
		*h = o->current.h;
}

/**
 * ewl_object_request_geometry - request a new geometry be applied to an object
 * @o: the object to request a new size
 * @x: the x coordinate to request be applied to the object
 * @y: the y coordinate to request be applied to the object
 * @w: the width to request be applied to the object
 * @h: the height to request be applied to the object
 *
 * Returns no value. The given geometry is requested to be applied to the
 * object @o at a later time. This is the usual method for requesting a new
 * geometry for an object.
 */
void
ewl_object_request_geometry(Ewl_Object * o, int x, int y, int w, int h)
{
	DCHECK_PARAM_PTR("o", o);

	o->request.x = x;
	o->request.y = y;
	o->request.w = w;
	o->request.h = h;
}


/**
 * ewl_object_request_size - request a new size be applied to an object
 * @o: the object to request a new size
 * @w: the width to request be applied to the object
 * @h: the height to request be applied to the object
 *
 * Returns no value. The given size is requested to be applied to the
 * object @o at a later time. This is the usual method for requesting a new
 * size for an object.
 */
void
ewl_object_request_size(Ewl_Object * o, int w, int h)
{
	DCHECK_PARAM_PTR("o", o);

	o->request.w = w;
	o->request.h = h;
}


/**
 * ewl_object_request_size - request a new position be applied to an object
 * @o: the object to request a new size
 * @x: the x coordinate to request be applied to the object
 * @y: the y coordinate to request be applied to the object
 *
 * Returns no value. The given position is requested to be applied to the
 * object @o at a later time. This is the usual method for requesting a new
 * position for an object.
 */
void
ewl_object_request_position(Ewl_Object * o, int x, int y)
{
	DCHECK_PARAM_PTR("o", o);

	o->request.x = x;
	o->request.y = y;
}

/**
 * ewl_object_request_x - request a new x position for an object
 * @o: the object to request a new x position
 * @x: the new x coordinate to be applied to the object
 *
 * Returns no value. The given x coordinate is stored to be applied to the
 * object @o at a later time.
 */
inline void
ewl_object_request_x(Ewl_Object * o, int x)
{
	DCHECK_PARAM_PTR("o", o);

	o->request.x = x;
}


/**
 * ewl_object_request_y - request a new y position for an object
 * @o: the object to request a new y position
 * @y: the new y coordinate to be applied to the object
 *
 * Returns no value. The given y coordinate is stored to be applied to the
 * object @o at a later time.
 */
inline void
ewl_object_request_y(Ewl_Object * o, int y)
{
	DCHECK_PARAM_PTR("o", o);

	o->request.y = y;
}


/**
 * ewl_object_request_w - request a new width for an object
 * @o: the object to request a new width
 * @w: the new width to be applied to the object
 *
 * Returns no value. The given width is stored to be applied to the
 * object @o at a later time.
 */
inline void
ewl_object_request_w(Ewl_Object * o, int w)
{
	DCHECK_PARAM_PTR("o", o);

	o->request.w = w;
}


/**
 * ewl_object_request_w - request a new width for an object
 * @o: the object to request a new height
 * @h: the new height to be applied to the object
 *
 * Returns no value. The given height is stored to be applied to the
 * object @o at a later time.
 */
inline void
ewl_object_request_h(Ewl_Object * o, int h)
{
	DCHECK_PARAM_PTR("o", o);

	o->request.h = h;
}

/**
 * ewl_object_requested_geometry - retrieve an objects requested geometry
 * @o: the object to query for requested geometry
 * @x: a pointer to the integer to store the requested x coordinate
 * @y: a pointer to the integer to store the requested y coordinate
 * @w: a pointer to the integer to store the requested width
 * @h: a pointer to the integer to store the requested height
 *
 * Returns no value. The requested geometry of the object @o is stored in the
 * the integers pointed to by @x, @y, @w, and @h.
 */
void
ewl_object_requested_geometry(Ewl_Object * o, int *x, int *y, int *w, int *h)
{
	DCHECK_PARAM_PTR("o", o);

	if (x)
		*x = o->request.x;
	if (y)
		*y = o->request.y;
	if (w)
		*w = o->request.w;
	if (h)
		*h = o->request.h;
}


/**
 * ewl_object_requested_size - retrieve an objects requested size
 * @o: the object to query for requested size
 * @w: a pointer to the integer to store the requested width
 * @h: a pointer to the integer to store the requested height
 *
 * Returns no value. The requested size of the object @o is stored in the
 * the integers pointed to by @w and @h.
 */
void
ewl_object_requested_size(Ewl_Object * o, int *w, int *h)
{
	DCHECK_PARAM_PTR("o", o);

	if (w)
		*w = REQUEST_W(o);
	if (h)
		*h = REQUEST_H(o);
}

/**
 * ewl_object_set_minimum_size - set the minimum size of an object
 * @o: the object to change the minimum size
 * @w: the new minimum width
 * @h: the new minimum height
 *
 * Returns no value. Sets the minimum size of the object @o to @w x @h. If the
 * current size or maximum size are less than the new minimum, they are set to
 * the new minimum size.
 */
void
ewl_object_set_minimum_size(Ewl_Object * o, int w, int h)
{
	DCHECK_PARAM_PTR("o", o);
	DENTER_FUNCTION(DLEVEL_STABLE);

	o->minimum.w = w;
	o->minimum.h = h;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_object_set_minimum_width - set the minimum width of an object
 * @o: the object to change the minimum width
 * @w: the new minimum width
 *
 * Returns no value. Sets the minimum width of the object @o to @w. If the
 * current width or maximum width are less than the new minimum, they are set to
 * the new minimum width.
 */
inline void
ewl_object_set_minimum_width(Ewl_Object * o, int w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("o", o);

	o->minimum.w = w;
	if (CURRENT_W(o) < w)
		CURRENT_W(o) = w;

	if (MAXIMUM_W(o) < w)
		MAXIMUM_W(o) = w;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_object_set_minimum_height - set the minimum height of an object
 * @o: the object to change the minimum height
 * @h: the new minimum height
 *
 * Returns no value. Sets the minimum height of the object @o to @h. If the
 * current height or maximum height are less than the new minimum, they are set
 * to the new minimum height.
 */
inline void
ewl_object_set_minimum_height(Ewl_Object * o, int h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("o", o);

	o->minimum.h = h;
	if (CURRENT_H(o) < h)
		CURRENT_H(o) = h;

	if (MAXIMUM_H(o) < h)
		MAXIMUM_H(o) = h;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_object_get_minimum_size - retrieve the minimum dimensions of an object
 * @o: the object to retrieve the minimum dimensions
 * @w: a pointer to an integer to store the minimum width
 * @h: a pointer to an integer to store the minimum height
 *
 * Returns no value. Stores the minimum height and width of object @o into the
 * integers pointed to by  @w and @h respectively.
 */
void
ewl_object_get_minimum_size(Ewl_Object * o, int *w, int *h)
{
	DCHECK_PARAM_PTR("o", o);

	if (w)
		*w = o->minimum.w;
	if (h)
		*h = o->minimum.h;
}


/**
 * ewl_object_set_maximum_size - set the maximum size of an object
 * @o: the object to change the maximum size
 * @w: the new maximum width
 * @h: the new maximum height
 *
 * Returns no value. Sets the maximum size of the object @o to @w x @h. If the
 * current size or minimum size are less than the new maximum, they are set to
 * the new maximum size.
 */
void
ewl_object_set_maximum_size(Ewl_Object * o, int w, int h)
{
	DCHECK_PARAM_PTR("o", o);
	DENTER_FUNCTION(DLEVEL_STABLE);

	o->maximum.w = w;
	if (w < MINIMUM_W(o))
		o->minimum.w = w;

	if (CURRENT_W(o) > w)
		CURRENT_W(o) = w;

	o->maximum.h = h;
	if (h < MINIMUM_H(o))
		o->minimum.h = h;

	if (CURRENT_H(o) > h)
		CURRENT_H(o) = h;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_object_set_maximum_width - set the minimum width of an object
 * @o: the object to change the maximum width
 * @w: the new maximum width
 *
 * Returns no value. Sets the maximum width of the object @o to @w. If the
 * current width or minimum width are less than the new maximum, they are set to
 * the new maximum width.
 */
inline void
ewl_object_set_maximum_width(Ewl_Object * o, int w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("o", o);

	o->maximum.w = w;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_object_set_maximum_height - set the minimum height of an object
 * @o: the object to change the maximum height
 * @h: the new maximum height
 *
 * Returns no value. Sets the maximum height of the object @o to @h. If the
 * current height or minimum width are less than the new maximum, they are set
 * to the new maximum height.
 */
inline void
ewl_object_set_maximum_height(Ewl_Object * o, int h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("o", o);

	o->maximum.h = h;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_object_get_maximum_size - retrieve the minimum dimensions of an object
 * @o: the object to retrieve the maximum dimensions
 * @w: a pointer to an integer to store the maximum width
 * @h: a pointer to an integer to store the maximum height
 *
 * Returns no value. Stores the maximum height and width of object @o into the
 * integers pointed to by  @w and @h respectively.
 */
void
ewl_object_get_maximum_size(Ewl_Object * o, int *w, int *h)
{
	DCHECK_PARAM_PTR("o", o);

	if (w)
		*w = o->maximum.w;
	if (h)
		*h = o->maximum.h;
}

/**
 * ewl_object_set_padding - set the padding around an objects edges
 * @o: the object to change the padding
 * @l: the new padding along the left side of the object
 * @r: the new padding along the right side of the object
 * @t: the new padding along the top side of the object
 * @b: the new padding along the bottom side of the object
 *
 * Returns no value. Stores the values of @l, @r, @t and @b into the object to
 * be used for distancing it's edges from other widgets when laying out.
 */
void
ewl_object_set_padding(Ewl_Object * o, int l, int r, int t, int b)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("o", o);

	o->padd.l = l;
	o->padd.r = r;
	o->padd.t = t;
	o->padd.b = b;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_object_get_padding - retrieve the edge padding of an object
 * @o: the object to retrieve padding
 * @l: a pointer to an integer that will receive the padding of the left side
 * @r: a pointer to an integer that will receive the padding of the right side
 * @t: a pointer to an integer that will receive the padding of the top side
 * @b: a pointer to an integer that will receive the padding of the bottom side
 *
 * Returns no value. Stores the edge padding of the object @o into the integer
 * pointer parameters passed in.
 */
void
ewl_object_get_padding(Ewl_Object * o, int *l, int *r, int *t, int *b)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("o", o);

	if (l)
		*l = o->padd.l;
	if (r)
		*r = o->padd.r;
	if (t)
		*t = o->padd.t;
	if (b)
		*b = o->padd.b;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_object_set_alignment - change the alignment of the specified object
 * @o: the object to change alignment
 * @align: the new alignment for the object
 *
 * Returns no value. Stores the new alignment value into the object for use
 * when laying out the object.
 */
inline void
ewl_object_set_alignment(Ewl_Object * o, Ewl_Alignment align)
{
	DCHECK_PARAM_PTR("o", o);

	o->alignment = align;
}


/**
 * ewl_object_set_fill_policy - change the fill policy of the specified object
 * @o: the object to change fill policy
 * @fill: the new fill policy for the object
 *
 * Returns no value. Stores the new fill policy value into the object for use
 * when laying out the object.
 */
inline void
ewl_object_set_fill_policy(Ewl_Object * o, Ewl_Fill_Policy fill)
{
	DCHECK_PARAM_PTR("o", o);

	o->fill_policy = fill;
}

/**
 * ewl_object_get_alignment - retrieve the value of the objects alignment
 * @o: the object to retrieve the alignment value
 *
 * Returns the value stored in the objects alignment attribute.
 */
inline Ewl_Alignment
ewl_object_get_alignment(Ewl_Object * o)
{
	DCHECK_PARAM_PTR_RET("o", o, EWL_ALIGNMENT_LEFT);

	return o->alignment;

}


/**
 * ewl_object_get_fill_policy - retrieve the value of the objects fill policy
 * @o: the object to retrieve the fill policy value
 *
 * Returns the value stored in the objects fill policy attribute.
 */
inline Ewl_Fill_Policy
ewl_object_get_fill_policy(Ewl_Object * o)
{
	DCHECK_PARAM_PTR_RET("o", o, EWL_FILL_POLICY_NORMAL);

	return o->fill_policy;
}
