#include <Ewl.h>

void            __ewl_seeker_realize(Ewl_Widget * w, void *ev_data,
				     void *user_data);
void            __ewl_seeker_configure(Ewl_Widget * w, void *ev_data,
				       void *user_data);
void            __ewl_seeker_theme_update(Ewl_Widget * w, void *ev_data,
					  void *user_data);
void            __ewl_seeker_dragbar_mouse_down(Ewl_Widget * w, void *ev_data,
						void *user_data);
void            __ewl_seeker_dragbar_mouse_up(Ewl_Widget * w, void *ev_data,
					      void *user_data);
void            __ewl_seeker_dragbar_mouse_move(Ewl_Widget * w, void *ev_data,
						void *user_data);
void            __ewl_seeker_mouse_down(Ewl_Widget * w, void *ev_data,
					void *user_data);
void            __ewl_seeker_focus_in(Ewl_Widget * w, void *ev_data,
				      void *user_data);
void            __ewl_seeker_focus_out(Ewl_Widget * w, void *ev_data,
				       void *user_data);
void            __ewl_seeker_mouse_move(Ewl_Widget * w, void *ev_data,
					void *user_data);
void            __ewl_seeker_position_to_value(Ewl_Seeker * s, int in_x,
					       int in_y);
void            __ewl_seeker_value_to_position(Ewl_Seeker * s);
void            __ewl_seeker_appearance_changed(Ewl_Widget * w, void *ev_data,
						void *user_data);


/**
 * ewl_seeker_new - allocate and initialize a new seeker with orientation
 *
 * @o: the orientation for the new seeker
 *
 * Returns NULL on failure, or a pointer to the new seeker on success.
 */
Ewl_Widget     *ewl_seeker_new(Ewl_Orientation o)
{
	Ewl_Seeker     *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = NEW(Ewl_Seeker, 1);
	if (!s)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ZERO(s, Ewl_Seeker, 1);

	ewl_seeker_init(s, o);

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}


/**
 * ewl_seeker_init - initialize the seeker to some sane starting values
 * @s: the seeker to be initialized
 * @orientation: the orientation of the seeker
 *
 * Returns no value. Initializes the seeker @s to the orientation @orientation
 * to default values and callbacks.
 */
void ewl_seeker_init(Ewl_Seeker * s, Ewl_Orientation orientation)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	w = EWL_WIDGET(s);

	/*
	 * Initialize the widget fields and set appropriate orientation and
	 * type
	 */
	if (orientation == EWL_ORIENTATION_HORIZONTAL) {
		ewl_container_init(EWL_CONTAINER(w), "hseeker",
				NULL, NULL, NULL);
		ewl_object_set_preferred_h(EWL_OBJECT(w), 16);

		ewl_object_set_fill_policy(EWL_OBJECT(w),
				EWL_FILL_POLICY_HFILL |
				EWL_FILL_POLICY_HSHRINK);
	}
	else {
		ewl_container_init(EWL_CONTAINER(w), "vseeker", NULL, NULL,
				   NULL);
		ewl_object_set_preferred_w(EWL_OBJECT(w), 16);

		ewl_object_set_fill_policy(EWL_OBJECT(w),
				EWL_FILL_POLICY_VFILL |
				EWL_FILL_POLICY_VSHRINK);
	}

	/*
	 * Create and add the dragbar portion of the seeker
	 */
	s->dragbar = ewl_button_new(NULL);
	ewl_container_append_child(EWL_CONTAINER(s), s->dragbar);

	/*
	 * Set the starting orientation, range and values
	 */
	s->orientation = orientation;
	s->range = 100.0;
	s->value = 0.0;
	s->step = 5.0;

	/*
	 * Add necessary configuration callbacks
	 */
	ewl_callback_append(w, EWL_CALLBACK_REALIZE, __ewl_seeker_realize,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, __ewl_seeker_configure,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_THEME_UPDATE,
			    __ewl_seeker_theme_update, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN, __ewl_seeker_mouse_down,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_THEME_UPDATE,
			    __ewl_seeker_appearance_changed, NULL);

	/*
	 * Append a callback for catching mouse movements on the dragbar and
	 * add the dragbar to the seeker
	 */
	ewl_callback_append(s->dragbar, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_seeker_dragbar_mouse_down, NULL);
	ewl_callback_append(s->dragbar, EWL_CALLBACK_MOUSE_UP,
			    __ewl_seeker_dragbar_mouse_up, NULL);
	ewl_callback_append(s->dragbar, EWL_CALLBACK_MOUSE_MOVE,
			    __ewl_seeker_dragbar_mouse_move, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_seeker_set_value - set the value of pointer of the seekers locator
 *
 * @s: the seeker whose value will be changed
 * @v: the new value of the locator, it will be checked against the valid range
 *
 * Returns no value.
 */
void ewl_seeker_set_value(Ewl_Seeker * s, double v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	if (v < 0)
		v = 0;

	if (v > s->range)
		v = s->range;

	s->value = v;

	ewl_widget_configure(EWL_WIDGET(s));
	ewl_callback_call(EWL_WIDGET(s), EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_seeker_get_value - retrieve the current value of the seekers locator
 *
 * @s: the seekers to retrieve the value
 *
 * Returns 0 on failure, the value of the seekers locator on success.
 */
double ewl_seeker_get_value(Ewl_Seeker * s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, -1);

	DRETURN_FLOAT(s->value, DLEVEL_STABLE);
}


/**
 * ewl_seeker_set_range - specify the range of values represented by the seeker
 *
 * @s: the seeker to change the range
 * @r: the largest bound on the range of the seekers value
 *
 * Returns no value.
 */
void ewl_seeker_set_range(Ewl_Seeker * s, double r)
{
	int             new_val;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	new_val = r * (s->value / s->range);

	s->range = r;
	s->value = new_val;

	ewl_widget_configure(EWL_WIDGET(s));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_seeker_get_range - retrieve the range of values represented by the seeker
 *
 * @s: the seeker to return the range of values
 *
 * Returns 0 on failure, or the upper bound on the seeker on success.
 */
double ewl_seeker_get_range(Ewl_Seeker * s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, -1);

	DRETURN_FLOAT(s->range, DLEVEL_STABLE);
}


/**
 * ewl_seeker_set_step - set the steps between increments
 * @s: the seeker to change step
 * @step: the new step value for the seeker
 *
 * Returns no value. Changes the amount that each increment or decrement
 * changes the value of the seeker @s.
 */
void ewl_seeker_set_step(Ewl_Seeker * s, double step)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	s->step = step;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_seeker_get_step - retrieve the step size of the seeker
 * @s: the seeker to retrieve step size
 *
 * Returns the step size of the seeker @s.
 */
double ewl_seeker_get_step(Ewl_Seeker * s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, -1);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
	DRETURN_FLOAT(s->step, DLEVEL_STABLE);

}


/**
 * ewl_seeker_increase - increase the value of a seeker by it's step size
 * @s: the seeker to increase
 *
 * Returns no value. Increases the value of the seeker @s by one increment of
 * it's step size.
 */
void ewl_seeker_increase(Ewl_Seeker * s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	s->value += s->step;

	if (s->value > s->range)
		s->value = s->range;
	else if (s->value < 0.0)
		s->value = 0.0;

	ewl_widget_configure(EWL_WIDGET(s));

	ewl_callback_call(EWL_WIDGET(s), EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_seeker_decrease - decrease the value of a seeker by it's step size
 * @s: the seeker to decrease
 *
 * Returns no value. Decreases the value of the seeker @s by one increment of
 * it's step size.
 */
void ewl_seeker_decrease(Ewl_Seeker * s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	s->value -= s->step;

	if (s->value > s->range)
		s->value = s->range;
	else if (s->value < 0.0)
		s->value = 0.0;

	ewl_widget_configure(EWL_WIDGET(s));

	ewl_callback_call(EWL_WIDGET(s), EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/*
 * Draw the representation of the seeker
 */
void __ewl_seeker_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ewl_widget_realize(EWL_SEEKER(w)->dragbar);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/*
 * On a configure event we need to adjust the seeker to fit into it's new
 * coords and position as well as move the dragbar to the correct size and
 * position.
 */
void __ewl_seeker_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Seeker     *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SEEKER(w);
	if (!s->dragbar)
		DRETURN(DLEVEL_STABLE);

	__ewl_seeker_value_to_position(s);
	ewl_widget_configure(EWL_WIDGET(s->dragbar));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void
__ewl_seeker_dragbar_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ecore_Event_Mouse_Down *ev;
	Ewl_Seeker     *s;
	int             xx, yy, ww, hh;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev_data", ev_data);

	ev = ev_data;

	s = EWL_SEEKER(w->parent);

	ewl_object_get_current_geometry(EWL_OBJECT(w), &xx, &yy, &ww, &hh);

	if (s->orientation == EWL_ORIENTATION_HORIZONTAL)
		s->dragstart = ev->x - xx;
	else
		s->dragstart = ev->y - yy;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void
__ewl_seeker_dragbar_mouse_up(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	EWL_SEEKER(w->parent)->dragstart = 0;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/*
 * Move the cursor to the correct position
 */
void
__ewl_seeker_dragbar_mouse_move(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ecore_Event_Mouse_Move *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev_data", ev_data);

	/*
	 * If the pointer is not pressed we don't care about mouse movements.
	 */
	if (!(w->state & EWL_STATE_PRESSED))
		DRETURN(DLEVEL_STABLE);

	if (EWL_SEEKER(w->parent)->dragstart < 1)
		DRETURN(DLEVEL_STABLE);

	ev = ev_data;

	__ewl_seeker_position_to_value(EWL_SEEKER(w->parent), ev->x, ev->y);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void __ewl_seeker_position_to_value(Ewl_Seeker * s, int in_x, int in_y)
{
	Ewl_Widget     *w;
	double          val = 0.0, old_val;
	int             xx, yy, ww, hh;
	int             req_x, req_y;

	w = s->dragbar;

	ewl_object_get_current_geometry(EWL_OBJECT(s), &xx, &yy, &ww, &hh);

	/*
	 * Adjust the width and height to fit within the insets of the bit
	 */
	xx += INSET_LEFT(w);
	yy += INSET_TOP(w);
	ww -= INSET_LEFT(w) + INSET_RIGHT(w);
	hh -= INSET_TOP(w) + INSET_BOTTOM(w);

	/*
	 * The direction of the dragbar move depends on the orientation of the
	 * parent seeker.
	 */
	if (s->orientation == EWL_ORIENTATION_HORIZONTAL) {
		if (CURRENT_W(w) >= ww)
			goto end;
		/*
		 * Use the current x value along with the starting drag
		 * position to determine the new position
		 */
		req_x = in_x - s->dragstart;
		/*
		 * Check the boundaries to make sure we're not off the end of
		 * the bar
		 */
		if (req_x < xx)
			req_x = xx;

		if (req_x + CURRENT_W(w) > xx + ww)
			req_x = xx + ww - CURRENT_W(w);

		/*
		 * Calculate the new value for the seeker
		 */
		val = 1.0 -
		    (((double) (req_x - xx) / (double) (ww - CURRENT_W(w))));

	} else {
		if (CURRENT_H(w) >= hh)
			goto end;
		/*
		 * Use the current y value along with the starting drag
		 * position to determine the new position
		 */
		req_y = in_y - s->dragstart;

		/*
		 * Check the boundaries to make sure we're not off the end of
		 * the bar
		 */
		if (req_y < yy)
			req_y = yy;

		if (req_y + CURRENT_H(w) > yy + hh)
			req_y = yy + hh - CURRENT_H(w);

		/*
		 * Calculate the new value for the seeker
		 */
		val = (double) (req_y - yy) / (double) (hh - CURRENT_H(w));
	}

      end:
	/*
	 * Calculate the new value and determine if the VALUE_CHANGED callback
	 * needs to be triggered
	 */
	old_val = s->value;
	s->value = s->range - val * s->range;

	if (s->value > s->range)
		s->value = s->range;

	__ewl_seeker_value_to_position(s);

	if (old_val != s->value)
		ewl_callback_call(EWL_WIDGET(s), EWL_CALLBACK_VALUE_CHANGED);
}


/*
 * Map the seeker's value to a position.
 */
void __ewl_seeker_value_to_position(Ewl_Seeker * s)
{
	Ewl_Widget     *w;
	int             xx, yy, ww, hh;
	int             req_x, req_y;

	w = s->dragbar;

	ewl_object_get_current_geometry(EWL_OBJECT(s), &xx, &yy, &ww, &hh);

	/*
	 * The direction of the dragbar move depends on the orientation of the
	 * parent seeker.
	 */
	if (s->orientation == EWL_ORIENTATION_HORIZONTAL) {

		/*
		 * Use the current value along with the range to determine the
		 * new position
		 */
		req_x = xx + ww - (int) (floor((double) (ww - CURRENT_W(w)) *
					       ((s->range -
						 s->value) / s->range))) -
		    CURRENT_W(w);

		req_y = yy;
	} else {

		/*
		 * Use the current value along with the range to determine the
		 * new position
		 */
		req_y = yy + hh - (int) (floor((double)
					       (hh - CURRENT_H(w)) *
					       (s->value / s->range))) -
		    CURRENT_H(w);

		req_x = xx;
	}

	ewl_object_request_position(EWL_OBJECT(w), req_x, req_y);
}


void __ewl_seeker_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Seeker     *s;
	Ecore_Event_Mouse_Down *ev;
	int             xx, yy, ww, hh;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev_data", ev_data);

	ev = ev_data;

	s = EWL_SEEKER(w);

	ewl_object_get_current_geometry(EWL_OBJECT(s->dragbar),
					&xx, &yy, &ww, &hh);

	switch (s->orientation) {
	case EWL_ORIENTATION_HORIZONTAL:
		if (ev->x < xx)
			s->value -= s->step;
		else if (ev->x > xx + ww)
			s->value += s->step;
		break;
	case EWL_ORIENTATION_VERTICAL:
		if (ev->y < yy)
			s->value += s->step;
		else if (ev->y > yy + hh)
			s->value -= s->step;
		break;
	default:
		break;
	}

	if (s->value < 0.0)
		s->value = 0.0;
	else if (s->value > s->range)
		s->value = s->range;

	/*
	 * Determine the new position of the dragbar and configure it.
	 */
	__ewl_seeker_value_to_position(s);
	ewl_widget_configure(s->dragbar);

	/*
	 * Now signal that the value of the seeker has changed.
	 */
	ewl_callback_call(EWL_WIDGET(s), EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/*
 * Update the theme for the dragbar.
 */
void __ewl_seeker_theme_update(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Seeker     *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SEEKER(w);

	ewl_widget_theme_update(s->dragbar);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void
__ewl_seeker_appearance_changed(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Seeker     *s;
	char            appearance[PATH_LEN];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SEEKER(w);

	snprintf(appearance, PATH_LEN, "%s/dragbar", w->appearance);

	ewl_widget_set_appearance(s->dragbar, appearance);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
