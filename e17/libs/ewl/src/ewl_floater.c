#include <Ewl.h>
#include "ewl_floater.h"


void __ewl_floater_configure(Ewl_Widget * parent, void * ev_data, void * user_data);
void __ewl_floater_realize(Ewl_Widget * parent, void * ev_data, void * user_data);
void __ewl_floater_destroy(Ewl_Widget * parent, void * ev_data, void * user_data);


/**
 * ewl_floater_new - allocate a new floater widget
 * @parent: the parent widget to follow if desired
 *
 * Returns NULL on failure, or a pointer to the newly allocated floater
 * widget on success.
 */
Ewl_Widget * 
ewl_floater_new (Ewl_Widget * parent)
{
	Ewl_Widget * f;

	DENTER_FUNCTION(DLEVEL_STABLE);
  
	f = NEW(Ewl_Floater, 1);
	if (!f)
		DRETURN_PTR (NULL, DLEVEL_STABLE);
  
	memset(f, 0, sizeof(Ewl_Floater));

	ewl_floater_init(EWL_FLOATER(f), parent);


	DRETURN_PTR(EWL_WIDGET(f), DLEVEL_STABLE);
}


/**
 * ewl_floater_init - initialize a floater to default values
 * @f: the floater widget
 * @parent: the parent widget, we need this to get the evas and clip_box
 * 
 * Returns no value. Sets the fields and callbacks of the floater @f to their
 * defaults.
 */
void
ewl_floater_init (Ewl_Floater * f, Ewl_Widget * parent)
{
	Ewl_Widget * w;

  
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("f", f);

	w = EWL_WIDGET(f);

	/*
	 * Initialize the inherited box fields, set the fill policy to
	 * normal, and the widget to follow.
	 */
	ewl_box_init(EWL_BOX(w), EWL_ORIENTATION_VERTICAL);
	ewl_object_set_fill_policy (EWL_OBJECT(w), EWL_FILL_POLICY_NORMAL);
	f->follows = parent;

	/*
	 * Setup the basic callbacks for special events.
	 */
	ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, __ewl_floater_configure,
			NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_REALIZE, __ewl_floater_realize,
			NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY, __ewl_floater_destroy,
			NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_floater_set_position - set the start x/y position of the floater
 * @f: Ewl_Floater * - the floater widget
 * @x: int - start x coordinate from the top
 * @y: int - start y coordinate from the top
 *
 * Returns no value
 */
void 
ewl_floater_set_position(Ewl_Floater * f, int x, int y)
{
	DENTER_FUNCTION(DLEVEL_UNSTABLE);

	DCHECK_PARAM_PTR("f", f);

	/*
	 * Set the coordinates of the floater, this will be used for either
	 * absolute or relative positioning.
	 */
	f->x = x;
	f->y = y;

	ewl_widget_configure(EWL_WIDGET(f));

	DLEAVE_FUNCTION(DLEVEL_UNSTABLE);
}

/**
 * ewl_floater_set_relative - set a floater positioned relative to a widget
 * @f: the floater to change positioning relation
 * @w: the widget to position the floater relative
 *
 * Returns no value. Sets the floater @f to be positioned relative to the
 * position of the widget @w.
 */
void
ewl_floater_set_relative(Ewl_Floater *f, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_UNSTABLE);

	DCHECK_PARAM_PTR("f", f);

	/*
	 * Remove the callback attached to the configure event for the
	 * followed widget.
	 */
	if (f->follows)
		ewl_callback_del(f->follows, EWL_CALLBACK_CONFIGURE,
				__ewl_floater_configure);

	/*
	 * Set the widget that the floater follows.
	 */
	f->follows = w;

	/*
	 * Now attach a callback to the configure event for the followed
	 * widget.
	 */
	if (f->follows)
		ewl_callback_append(f->follows, EWL_CALLBACK_CONFIGURE,
				__ewl_floater_configure, NULL);

	ewl_widget_configure(EWL_WIDGET(f));

	DLEAVE_FUNCTION(DLEVEL_UNSTABLE);
}

/*
 * Configure the floater so that the positioning is relative to a followed
 * widget if appropriate.
 */
void
__ewl_floater_configure(Ewl_Widget * w, void * ev_data, void * user_data)
{
	int align, x, y;
	Ewl_Floater * f;

	DENTER_FUNCTION(DLEVEL_STABLE);

	f = EWL_FLOATER(w);

	/*
	 * Determine actual coordinates based on absolute or relative
	 * positioning.
	 */
	if (f->follows) {
		x = REQUEST_X(f->follows) + f->x;
		y = REQUEST_Y(f->follows) + f->y;
	}
	else {
		x = f->x;
		y = f->y;
	}

	/*
	 * Store the alignment in a temporary variable for fast access to
	 * determine positioning.
	 */
	align = ewl_object_get_alignment(EWL_OBJECT(w));

	/*
	 * Determine the horizontal placement of the widget based on alignment
	 */
	if (align & EWL_ALIGNMENT_RIGHT) {
		x -= REQUEST_W(w);
	}
	else if (!(align & EWL_ALIGNMENT_LEFT)) {
		x -= REQUEST_W(w) / 2;
	}

	/*
	 * Determine the vertical placement of the widget based on alignment
	 */
	if (align & EWL_ALIGNMENT_BOTTOM) {
		y -= REQUEST_H(w);
	}
	else if (!(align & EWL_ALIGNMENT_TOP)) {
		y -= REQUEST_H(w) / 2;
	}

	/*
	 * Now request the calculated coordinates for the floater.
	 */
	ewl_object_request_position(EWL_OBJECT(w), x, y);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Grab some necessary information from the parent when realizing this widget
 */
void
__ewl_floater_realize(Ewl_Widget * w, void * ev_data, void * user_data)
{
	w->evas = w->parent->evas;
	w->evas_window = w->parent->evas_window;
	/* evas_set_color(w->evas, w->fx_clip_box, 255, 0, 0, 255); */
}

/*
 * Be sure to remove the floater from the parent windows list of floaters when
 * it gets destroyed.
 */
void
__ewl_floater_destroy(Ewl_Widget * w, void * ev_data, void * user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_window_floater_remove(EWL_WINDOW(w->parent), EWL_FLOATER(w));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
