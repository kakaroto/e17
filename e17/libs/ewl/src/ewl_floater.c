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

	ewl_box_init(EWL_BOX(w), EWL_ORIENTATION_VERTICAL);
	ewl_object_set_fill_policy (EWL_OBJECT(w), EWL_FILL_POLICY_NORMAL);

	ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE, __ewl_floater_configure,
			NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_REALIZE, __ewl_floater_realize,
			NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY, __ewl_floater_destroy,
			NULL);

	if (parent) {
		f->follows = parent;
		f->x = CURRENT_X(w) = REQUEST_X(parent);
		f->y = CURRENT_Y(w) =  REQUEST_Y(parent);
	}

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

	if (f->follows) {
		f->x = x + REQUEST_X(f->follows);
		f->y = y + REQUEST_Y(f->follows);
	}
	else {
		f->x = x;
		f->y = y;
	}

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

	f->follows = w;

	ewl_widget_configure(EWL_WIDGET(f));

	DLEAVE_FUNCTION(DLEVEL_UNSTABLE);
}

void
__ewl_floater_configure(Ewl_Widget * w, void * ev_data, void * user_data)
{
	Ewl_Floater * f;

	DENTER_FUNCTION(DLEVEL_STABLE);

	f = EWL_FLOATER(w);

	if (f->follows) {
		REQUEST_X(f) = REQUEST_X(f->follows) + f->x;
		REQUEST_Y(f) = REQUEST_Y(f->follows) + f->y;
	}
	else {
		REQUEST_X(f) = f->x;
		REQUEST_Y(f) = f->y;
	}

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
	evas_set_color(w->evas, w->fx_clip_box, 255, 0, 0, 255);
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
