
#include <Ewl.h>

/**
 * ewl_object_init - initialize the fields of an object
 * @o: the object to initialize
 *
 * Returns no value. Sets all of the fields of the object @o to their default
 * values.
 */
void ewl_object_init(Ewl_Object * o)
{
	DCHECK_PARAM_PTR("o", o);

	/*
	 * Set the default minimum sizes.
	 */
	ewl_object_set_minimum_size(o, EWL_OBJECT_MIN_SIZE,
			EWL_OBJECT_MIN_SIZE);

	/*
	 * Set the default maximum sizes.
	 */
	ewl_object_set_maximum_size(o, EWL_OBJECT_MAX_SIZE,
			EWL_OBJECT_MAX_SIZE);

	/*
	 * Set the default fill policy and alignment for the object.
	 */
	o->fill_policy = EWL_FILL_POLICY_NORMAL;
	o->alignment = EWL_ALIGNMENT_LEFT | EWL_ALIGNMENT_TOP;
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
ewl_object_get_current_geometry(Ewl_Object * o, int *x, int *y, int *w, int *h)
{
	DCHECK_PARAM_PTR("o", o);

	if (x)
		*x = ewl_object_get_current_x(o);
	if (y)
		*y = ewl_object_get_current_y(o);
	if (w)
		*w = ewl_object_get_current_w(o);
	if (h)
		*h = ewl_object_get_current_h(o);
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
void ewl_object_get_current_size(Ewl_Object * o, int *w, int *h)
{
	DCHECK_PARAM_PTR("o", o);

	if (w)
		*w = ewl_object_get_current_w(o);
	if (h)
		*h = ewl_object_get_current_h(o);
}

/**
 * ewl_object_get_current_x - get the current x position of the object
 * @o: the object to retrieve the current x position
 *
 * Returns the current x position of the object @o.
 */
int ewl_object_get_current_x(Ewl_Object * o)
{
	DCHECK_PARAM_PTR_RET("o", o, 0);

	DRETURN_INT(CURRENT_X(o) - PADDING_LEFT(o) - INSET_LEFT(o),
			DLEVEL_STABLE);
}

/**
 * ewl_object_get_current_y - get the current y position of the object
 * @o: the object to retrieve the current y position
 *
 * Returns the current y position of the object @o.
 */
int ewl_object_get_current_y(Ewl_Object * o)
{
	DCHECK_PARAM_PTR_RET("o", o, 0);

	DRETURN_INT(CURRENT_Y(o) - PADDING_TOP(o) - INSET_TOP(o),
			DLEVEL_STABLE);
}

/**
 * ewl_object_get_current_w - get the current width of the object
 * @o: the object to retrieve the current width
 *
 * Returns the current width of the object @o.
 */
int ewl_object_get_current_w(Ewl_Object * o)
{
	DCHECK_PARAM_PTR_RET("o", o, 0);

	DRETURN_INT(CURRENT_W(o) + PADDING_HORIZONTAL(o) + INSET_HORIZONTAL(o),
			DLEVEL_STABLE);
}

/**
 * ewl_object_get_current_h - get the current height of the object
 * @o: the object to retrieve the current height
 *
 * Returns the current height of the object @o.
 */
int ewl_object_get_current_h(Ewl_Object * o)
{
	DCHECK_PARAM_PTR_RET("o", o, 0);

	DRETURN_INT(CURRENT_H(o) + PADDING_VERTICAL(o) + INSET_VERTICAL(o),
			DLEVEL_STABLE);
}

/**
 * ewl_object_set_preferred_size - set the preferred size of the object
 * @o: the object to change size
 * @w: the new width of the object
 * @h: the new height of the object
 *
 * Returns no value. The dimensions of the object @o are set to the values of
 * the parameters @w, and @h unless these values are greater than the objects
 * maximum value or smaller than the objects minimum value. If they are
 * outside these bounds, the size is not altered.
 */
void ewl_object_set_preferred_size(Ewl_Object * o, int w, int h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("o", o);

	ewl_object_set_preferred_w(o, w);
	ewl_object_set_preferred_h(o, h);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_object_set_preferred_w - set the preferred width of the object
 * @o: the object to change preferred width
 * @w: the value to use as the preferred width
 *
 * Returns no value. Sets the preferred of @o width to @w or as close as
 * possible according to the bounds.
 */
void ewl_object_set_preferred_w(Ewl_Object * o, int w)
{
	int             old_size;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("o", o);

	if (w == PREFERRED_W(o))
		DRETURN(DLEVEL_STABLE);

	/*
	 * Store the previous size.
	 */
	old_size = PREFERRED_W(o);

	/*
	 * Set the current size to the new preferred size.
	 */
	PREFERRED_W(o) = w;

	/*
	 * Now update the widgets parent of the change in size.
	 */
	ewl_container_resize_child(EWL_WIDGET(o),
				   PREFERRED_W(o) - old_size,
				   EWL_ORIENTATION_HORIZONTAL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_object_set_preferred_h - set the preferred height of the object
 * @o: the object to change preferred height 
 * @h: the value to use as the preferred height
 *
 * Returns no value. Sets the preferred of @o height to @w or as close as
 * possible according to the bounds.
 */
void ewl_object_set_preferred_h(Ewl_Object * o, int h)
{
	int             old_size;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("o", o);

	if (h == PREFERRED_H(o))
		DRETURN(DLEVEL_STABLE);

	/*
	 * Store the previous size
	 */
	old_size = PREFERRED_H(o);

	/*
	 * Set the current size to the new preferred size
	 */
	PREFERRED_H(o) = h;

	/*
	 * Notify the parent widgets of the change in size.
	 */
	ewl_container_resize_child(EWL_WIDGET(o),
				   PREFERRED_H(o) - old_size,
				   EWL_ORIENTATION_VERTICAL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_object_get_preferred_size - retrieve the preferred size of an object
 * @o: the object to retrieve size information
 * @w: a pointer to the integer to store the width of the object
 * @h: a pointer to the integer to store the height of the object
 *
 * Returns no value. Stores the width and height of object @o into @w and @h
 * respectively.
 */
void ewl_object_get_preferred_size(Ewl_Object * o, int *w, int *h)
{
	DCHECK_PARAM_PTR("o", o);

	if (w)
		*w = ewl_object_get_preferred_w(o);
	if (h)
		*h = ewl_object_get_preferred_h(o);
}

/**
 * ewl_object_get_preferred_w - get the preferred width of the object
 * @o: the object to retrieve the preferred width
 *
 * Returns the preferred width of the object @o.
 */
int ewl_object_get_preferred_w(Ewl_Object * o)
{
	DCHECK_PARAM_PTR_RET("o", o, 0);

	DRETURN_INT(PREFERRED_W(o) + INSET_HORIZONTAL(o) +
			PADDING_HORIZONTAL(o), DLEVEL_STABLE);
}

/**
 * ewl_object_get_preferred_h - get the preferred height of the object
 * @o: the object to retrieve the preferred height
 *
 * Returns the preferred height of the object @o.
 */
int ewl_object_get_preferred_h(Ewl_Object * o)
{
	DCHECK_PARAM_PTR_RET("o", o, 0);

	DRETURN_INT(PREFERRED_H(o) + INSET_VERTICAL(o) + PADDING_VERTICAL(o),
			DLEVEL_STABLE);
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
 * object @o. This is the usual method for requesting a new geometry for an
 * object.
 */
void ewl_object_request_geometry(Ewl_Object * o, int x, int y, int w, int h)
{
	/*
	 * Pass the parameters on to the appropriate object request functions.
	 */
	ewl_object_request_position(o, x, y);
	ewl_object_request_size(o, w, h);
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
void ewl_object_request_size(Ewl_Object * o, int w, int h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_object_request_w(o, w);
	ewl_object_request_h(o, h);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_object_request_position - request a new position be applied to an object
 * @o: the object to request a new size
 * @x: the x coordinate to request be applied to the object
 * @y: the y coordinate to request be applied to the object
 *
 * Returns no value. The given position is requested to be applied to the
 * object @o at a later time. This is the usual method for requesting a new
 * position for an object.
 */
void ewl_object_request_position(Ewl_Object * o, int x, int y)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_object_request_x(o, x);
	ewl_object_request_y(o, y);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_object_request_x - request a new x position for an object
 * @o: the object to request a new x position
 * @x: the new x coordinate to be applied to the object
 *
 * Returns no value. The given x coordinate is stored to be applied to the
 * object @o at a later time.
 */
inline void ewl_object_request_x(Ewl_Object * o, int x)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("o", o);

	CURRENT_X(o) = x + PADDING_LEFT(o) + INSET_LEFT(o);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_object_request_y - request a new y position for an object
 * @o: the object to request a new y position
 * @y: the new y coordinate to be applied to the object
 *
 * Returns no value. The given y coordinate is stored to be applied to the
 * object @o at a later time.
 */
inline void ewl_object_request_y(Ewl_Object * o, int y)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("o", o);

	CURRENT_Y(o) = y + PADDING_TOP(o) + INSET_TOP(o);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_object_request_w - request a new width for an object
 * @o: the object to request a new width
 * @w: the new width to be applied to the object
 *
 * Returns no value. The given width is stored to be applied to the
 * object @o at a later time.
 */
void ewl_object_request_w(Ewl_Object * o, int w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("o", o);

	w -= PADDING_HORIZONTAL(o) + INSET_HORIZONTAL(o);

	/*
	 * Bound the width by the preferred size first.
	 */
	if ((w < o->preferred.w && !(o->fill_policy & EWL_FILL_POLICY_HSHRINK))
	    || (w > o->preferred.w &&
		!(o->fill_policy & EWL_FILL_POLICY_HFILL)))
		w = o->preferred.w;

	/*
	 * Now bound it by the min's and max's to achieve the desired size.
	 */
	if (w < MINIMUM_W(o))
		CURRENT_W(o) = MINIMUM_W(o);
	else if (w > MAXIMUM_W(o))
		CURRENT_W(o) = MAXIMUM_W(o);
	else
		CURRENT_W(o) = w;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_object_request_h - request a new width for an object
 * @o: the object to request a new height
 * @h: the new height to be applied to the object
 *
 * Returns no value. The given height is stored to be applied to the
 * object @o at a later time.
 */
void ewl_object_request_h(Ewl_Object * o, int h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("o", o);

	h -= PADDING_VERTICAL(o) + INSET_VERTICAL(o);

	/*
	 * Bound the width by the preferred size first.
	 */
	if ((h < o->preferred.h && !(o->fill_policy & EWL_FILL_POLICY_VSHRINK))
	    || (h > o->preferred.h &&
		!(o->fill_policy & EWL_FILL_POLICY_VFILL)))
		h = o->preferred.h;

	/*
	 * Now bound it by the min's and max's to achieve the desired size.
	 */
	if (h < MINIMUM_H(o))
		CURRENT_H(o) = MINIMUM_H(o);
	else if (h > MAXIMUM_H(o))
		CURRENT_H(o) = MAXIMUM_H(o);
	else
		CURRENT_H(o) = h;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
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
void ewl_object_set_minimum_size(Ewl_Object * o, int w, int h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("o", o);

	ewl_object_set_minimum_w(o, w);
	ewl_object_set_minimum_h(o, h);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_object_set_minimum_w - set the minimum width of an object
 * @o: the object to change the minimum width
 * @w: the new minimum width
 *
 * Returns no value. Sets the minimum width of the object @o to @w. If the
 * current width or maximum width are less than the new minimum, they are set to
 * the new minimum width.
 */
inline void ewl_object_set_minimum_w(Ewl_Object * o, int w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("o", o);

	MINIMUM_W(o) = w + PADDING_HORIZONTAL(o) + INSET_HORIZONTAL(o);

	if (MAXIMUM_W(o) < w)
		MAXIMUM_W(o) = w;

	if (CURRENT_W(o) < w)
		CURRENT_W(o) = w;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_object_set_minimum_h - set the minimum height of an object
 * @o: the object to change the minimum height
 * @h: the new minimum height
 *
 * Returns no value. Sets the minimum height of the object @o to @h. If the
 * current height or maximum height are less than the new minimum, they are set
 * to the new minimum height.
 */
inline void ewl_object_set_minimum_h(Ewl_Object * o, int h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("o", o);

	MINIMUM_H(o) = h;

	if (MAXIMUM_H(o) < h)
		MAXIMUM_H(o) = h;

	if (CURRENT_H(o) < h)
		CURRENT_H(o) = h;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_object_get_minimum_w - get the minimum width of an object
 * @o: the object to get the minimum width
 *
 * Returns the minimum width of the object @o.
 */
inline int ewl_object_get_minimum_w(Ewl_Object * o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("o", o, 0);

	DRETURN_INT(MINIMUM_W(o) + PADDING_HORIZONTAL(o) + INSET_HORIZONTAL(o),
			DLEVEL_STABLE);
}

/**
 * ewl_object_get_minimum_h - get the minimum height of an object
 * @o: the object to get the minimum height
 *
 * Returns the minimum height of the object @o.
 */
inline int ewl_object_get_minimum_h(Ewl_Object * o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("o", o, 0);

	DRETURN_INT(MINIMUM_H(o) + PADDING_VERTICAL(o) + INSET_VERTICAL(o),
			DLEVEL_STABLE);
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
void ewl_object_get_minimum_size(Ewl_Object * o, int *w, int *h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("o", o);

	if (w)
		*w = ewl_object_get_minimum_w(o);
	if (h)
		*h = ewl_object_get_minimum_h(o);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
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
void ewl_object_set_maximum_size(Ewl_Object * o, int w, int h)
{
	DCHECK_PARAM_PTR("o", o);
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_object_set_maximum_w(o, w);
	ewl_object_set_maximum_h(o, h);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_object_set_maximum_w - set the minimum width of an object
 * @o: the object to change the maximum width
 * @w: the new maximum width
 *
 * Returns no value. Sets the maximum width of the object @o to @w. If the
 * current width or minimum width are less than the new maximum, they are set to
 * the new maximum width.
 */
inline void ewl_object_set_maximum_w(Ewl_Object * o, int w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("o", o);

	MAXIMUM_W(o) = w;

	if (MAXIMUM_W(o) < 0)
		MAXIMUM_W(o) = EWL_OBJECT_MAX_SIZE;

	if (CURRENT_W(o) > w)
		CURRENT_W(o) = w;

	if (MINIMUM_W(o) > w)
		MINIMUM_W(o) = w;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_object_set_maximum_h - set the minimum height of an object
 * @o: the object to change the maximum height
 * @h: the new maximum height
 *
 * Returns no value. Sets the maximum height of the object @o to @h. If the
 * current height or minimum width are less than the new maximum, they are set
 * to the new maximum height.
 */
inline void ewl_object_set_maximum_h(Ewl_Object * o, int h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("o", o);

	o->maximum.h = h;

	if (MAXIMUM_H(o) < 0)
		MAXIMUM_H(o) = EWL_OBJECT_MAX_SIZE;

	if (MINIMUM_H(o) > h)
		o->minimum.h = h;

	if (CURRENT_H(o) > h)
		CURRENT_H(o) = h;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_object_get_maximum_w - get the maximum width of an object
 * @o: the object to get the maximum width
 *
 * Returns the maximum width of the object @o.
 */
inline int ewl_object_get_maximum_w(Ewl_Object * o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("o", o, 0);

	DRETURN_INT(MAXIMUM_W(o) + PADDING_HORIZONTAL(o) + INSET_HORIZONTAL(o),
			DLEVEL_STABLE);
		
}


/**
 * ewl_object_get_maximum_h - get the maximum height of an object
 * @o: the object to get the maximum height
 *
 * Returns the maximum height of the object @o.
 */
inline int ewl_object_get_maximum_h(Ewl_Object * o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("o", o, 0);

	DRETURN_INT(MAXIMUM_H(o) + PADDING_VERTICAL(o) + INSET_VERTICAL(o),
			DLEVEL_STABLE);
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
void ewl_object_get_maximum_size(Ewl_Object * o, int *w, int *h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("o", o);

	if (w)
		*w = ewl_object_get_maximum_w(o);
	if (h)
		*h = ewl_object_get_maximum_h(o);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
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
void ewl_object_set_padding(Ewl_Object * o, int l, int r, int t, int b)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("o", o);

	o->pad.l = l;
	o->pad.r = r;
	o->pad.t = t;
	o->pad.b = b;

	ewl_object_set_minimum_size(o, MINIMUM_W(o), MINIMUM_H(o));
	ewl_object_set_maximum_size(o, MAXIMUM_W(o), MAXIMUM_H(o));
	ewl_object_set_preferred_size(o, PREFERRED_W(o), PREFERRED_H(o));

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
void ewl_object_get_padding(Ewl_Object * o, int *l, int *r, int *t, int *b)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("o", o);

	if (l)
		*l = o->pad.l;
	if (r)
		*r = o->pad.r;
	if (t)
		*t = o->pad.t;
	if (b)
		*b = o->pad.b;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_object_top_padding - get the top padding for the object
 * @o: the ewl object to retrieve the top padding
 *
 * Returns the top padding for the object @o.
 */
int ewl_object_top_padding(Ewl_Object * o)
{
	DCHECK_PARAM_PTR_RET("o", o, 0);

	DRETURN_INT(PADDING_TOP(o), DLEVEL_STABLE);
}

/**
 * ewl_object_bottom_padding - get the bottom padding for the object
 * @o: the ewl object to retrieve the bottom padding
 *
 * Returns the bottom padding for the object @o.
 */
int ewl_object_bottom_padding(Ewl_Object * o)
{
	DCHECK_PARAM_PTR_RET("o", o, 0);

	DRETURN_INT(PADDING_BOTTOM(o), DLEVEL_STABLE);
}

/**
 * ewl_object_left_padding - get the left padding for the object
 * @o: the ewl object to retrieve the left padding
 *
 * Returns the left padding for the object @o.
 */
int ewl_object_left_padding(Ewl_Object * o)
{
	DCHECK_PARAM_PTR_RET("o", o, 0);

	DRETURN_INT(PADDING_LEFT(o), DLEVEL_STABLE);
}

/**
 * ewl_object_right_padding - get the right padding for the object
 * @o: the ewl object to retrieve the right padding
 *
 * Returns the right padding for the object @o.
 */
int ewl_object_right_padding(Ewl_Object * o)
{
	DCHECK_PARAM_PTR_RET("o", o, 0);

	DRETURN_INT(PADDING_RIGHT(o), DLEVEL_STABLE);
}


/**
 * ewl_object_set_insets - set the insets around an objects edges
 * @o: the object to change the insets
 * @l: the new insets along the left side of the object
 * @r: the new insets along the right side of the object
 * @t: the new insets along the top side of the object
 * @b: the new insets along the bottom side of the object
 *
 * Returns no value. Stores the values of @l, @r, @t and @b into the object to
 * be used for distancing it's edges from other widgets when laying out.
 */
void ewl_object_set_insets(Ewl_Object * o, int l, int r, int t, int b)
{
	int dh, dv;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("o", o);

	dh = (l - o->insets.l) + (r - o->insets.r);
	dv = (t - o->insets.t) + (b - o->insets.t);

	o->insets.l = l;
	o->insets.r = r;
	o->insets.t = t;
	o->insets.b = b;

	/*
	 * Now update the widgets parent of the change in size.
	 */
	if (dh)
		ewl_container_resize_child(EWL_WIDGET(o), dh,
				EWL_ORIENTATION_HORIZONTAL);
	if (dv)
		ewl_container_resize_child(EWL_WIDGET(o), dv,
				EWL_ORIENTATION_VERTICAL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_object_get_insets - retrieve the edge insets of an object
 * @o: the object to retrieve insets
 * @l: a pointer to an integer that will receive the insets of the left side
 * @r: a pointer to an integer that will receive the insets of the right side
 * @t: a pointer to an integer that will receive the insets of the top side
 * @b: a pointer to an integer that will receive the insets of the bottom side
 *
 * Returns no value. Stores the edge insets of the object @o into the integer
 * pointer parameters passed in.
 */
void ewl_object_get_insets(Ewl_Object * o, int *l, int *r, int *t, int *b)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("o", o);

	if (l)
		*l = o->insets.l;
	if (r)
		*r = o->insets.r;
	if (t)
		*t = o->insets.t;
	if (b)
		*b = o->insets.b;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_object_top_insets - get the top insets for the object
 * @o: the ewl object to retrieve the top insets
 *
 * Returns the top insets for the object @o.
 */
int ewl_object_top_insets(Ewl_Object * o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("o", o, 0);

	DRETURN_INT(INSET_TOP(o), DLEVEL_STABLE);
}

/**
 * ewl_object_bottom_insets - get the bottom insets for the object
 * @o: the ewl object to retrieve the bottom insets
 *
 * Returns the bottom insets for the object @o.
 */
int ewl_object_bottom_insets(Ewl_Object * o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("o", o, 0);

	DRETURN_INT(INSET_BOTTOM(o), DLEVEL_STABLE);
}

/**
 * ewl_object_left_insets - get the left insets for the object
 * @o: the ewl object to retrieve the left insets
 *
 * Returns the left insets for the object @o.
 */
int ewl_object_left_insets(Ewl_Object * o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("o", o, 0);

	DRETURN_INT(INSET_LEFT(o), DLEVEL_STABLE);
}

/**
 * ewl_object_right_insets - get the right insets for the object
 * @o: the ewl object to retrieve the right insets
 *
 * Returns the right insets for the object @o.
 */
int ewl_object_right_insets(Ewl_Object * o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("o", o, 0);

	DRETURN_INT(INSET_RIGHT(o), DLEVEL_STABLE);
}

/**
 * ewl_object_set_alignment - change the alignment of the specified object
 * @o: the object to change alignment
 * @align: the new alignment for the object
 *
 * Returns no value. Stores the new alignment value into the object for use
 * when laying out the object.
 */
inline void ewl_object_set_alignment(Ewl_Object * o, Ewl_Alignment align)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
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
inline void ewl_object_set_fill_policy(Ewl_Object * o, Ewl_Fill_Policy fill)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("o", o);

	o->fill_policy = fill;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_object_get_alignment - retrieve the value of the objects alignment
 * @o: the object to retrieve the alignment value
 *
 * Returns the value stored in the objects alignment attribute.
 */
inline          Ewl_Alignment ewl_object_get_alignment(Ewl_Object * o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("o", o, EWL_ALIGNMENT_LEFT);

	DRETURN_INT(o->alignment, DLEVEL_STABLE);
}


/**
 * ewl_object_get_fill_policy - retrieve the value of the objects fill policy
 * @o: the object to retrieve the fill policy value
 *
 * Returns the value stored in the objects fill policy attribute.
 */
inline          Ewl_Fill_Policy ewl_object_get_fill_policy(Ewl_Object * o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("o", o, EWL_FILL_POLICY_NORMAL);

	DRETURN_INT(o->fill_policy, DLEVEL_STABLE);
}
