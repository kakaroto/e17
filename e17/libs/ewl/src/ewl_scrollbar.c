
#include <Ewl.h>


void            __ewl_scrollbar_realize(Ewl_Widget * s, void *ev_data,
					void *user_data);
void            __ewl_scrollbar_decrement(Ewl_Widget * w, void *ev_data,
					  void *user_data);
void            __ewl_scrollbar_increment(Ewl_Widget * w, void *ev_data,
					  void *user_data);
void            __ewl_scrollbar_seeker_configure(Ewl_Widget * w, void *ev_data,
						 void *user_data);
void            __ewl_scrollbar_seeker_value_changed(Ewl_Widget * w,
						     void *ev_data,
						     void *user_data);


/**
 * ewl_scrollbar_new - allocate and initialize a new scrollbar widget
 * @orientation: the desired orientation of the scrollbar
 *
 * Returns NULL on failure, or a pointer to the newly allocated scrollbar
 * widget on success.
 */
Ewl_Widget     *ewl_scrollbar_new(Ewl_Orientation orientation)
{
	Ewl_Scrollbar  *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = NEW(Ewl_Scrollbar, 1);
	if (!s)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ZERO(s, Ewl_Scrollbar, 1);

	/*
	 * Initialize the objects fields.
	 */
	ewl_scrollbar_init(s, orientation);

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * ewl_scrollbar_init - initialize a scrollbar to default values
 *
 * @s: the scrollbar to initialize
 * @orientation: the orientation for the scrollbar
 *
 * Returns no value.
 */
void ewl_scrollbar_init(Ewl_Scrollbar * s, Ewl_Orientation orientation)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	w = EWL_WIDGET(s);

	ewl_box_init(EWL_BOX(w), orientation);

	/*
	 * Create the basic widgets that are contained in the scrollbar.
	 */
	s->button_decrement = ewl_button_new(NULL);
	s->button_increment = ewl_button_new(NULL);
	s->seeker = ewl_seeker_new(orientation);

	/*
	 * Append callbacks for the scrollbars basic functions.
	 */
	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_scrollbar_realize, NULL);

	ewl_callback_append(s->button_decrement, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_scrollbar_decrement, s);

	/*
	 * Attach callbacks to the buttons and seeker to handle the various
	 * events.
	 */
	ewl_callback_append(s->button_increment, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_scrollbar_increment, s);
	ewl_callback_append(s->seeker, EWL_CALLBACK_CONFIGURE,
			    __ewl_scrollbar_seeker_configure, s);

	/*
	 * Set the default fill policies for the buttons and the seeker
	 */
	ewl_object_set_fill_policy(EWL_OBJECT(s->button_decrement),
				   EWL_FILL_POLICY_NORMAL);
	ewl_object_set_fill_policy(EWL_OBJECT(s->button_increment),
				   EWL_FILL_POLICY_NORMAL);

	/*
	 * Set the default alignment for the buttons.
	 */
	ewl_object_set_alignment(EWL_OBJECT(s->button_decrement),
				 EWL_ALIGNMENT_CENTER);
	ewl_object_set_alignment(EWL_OBJECT(s->button_increment),
				 EWL_ALIGNMENT_CENTER);

	/*
	 * Set the default amount of space that the seeker should fill.
	 */
	s->fill_percentage = 1.0;

	/*
	 * Append a value change callback to the seeker to catch when it
	 * moves.
	 */
	ewl_callback_append(s->seeker, EWL_CALLBACK_VALUE_CHANGED,
			    __ewl_scrollbar_seeker_value_changed, s);

	/*
	 * Define the maximum value that the seeker can reach, and the
	 * increments it takes to get there.
	 */
	ewl_seeker_set_range(EWL_SEEKER(s->seeker), 1.0);
	ewl_seeker_set_step(EWL_SEEKER(s->seeker), 0.05);

	/*
	 * Set the appearance strings for the parts of the scrollbar
	 */
	ewl_widget_set_appearance(s->button_decrement, "button_increment");
	ewl_widget_set_appearance(s->button_increment, "button_decrement");

	/*
	 * Setup a few orientation specific variables, such as appearance and
	 * packing order.
	 */
	if (orientation == EWL_ORIENTATION_HORIZONTAL) {

		/*
		 * Set the alignment of the buttons to the seeker.
		 */
		s->buttons_alignment = ewl_theme_data_get_int(EWL_WIDGET(s),
							      "/hscrollbar/button_order");

		if (s->buttons_alignment & EWL_ALIGNMENT_LEFT) {

			/*
			 * Place in decrement, increment, seeker order.
			 */
			ewl_container_append_child(EWL_CONTAINER(s),
						   s->button_decrement);
			ewl_container_append_child(EWL_CONTAINER(s),
						   s->button_increment);
			ewl_container_append_child(EWL_CONTAINER(s), s->seeker);
		} else if (s->buttons_alignment & EWL_ALIGNMENT_RIGHT) {

			/*
			 * Place in seeker, decrement, increment order.
			 */
			ewl_container_append_child(EWL_CONTAINER(s), s->seeker);
			ewl_container_append_child(EWL_CONTAINER(s),
						   s->button_decrement);
			ewl_container_append_child(EWL_CONTAINER(s),
						   s->button_increment);
		} else {

			/*
			 * Place in decrement, seeker, increment order.
			 */
			ewl_container_append_child(EWL_CONTAINER(s),
						   s->button_decrement);
			ewl_container_append_child(EWL_CONTAINER(s), s->seeker);
			ewl_container_append_child(EWL_CONTAINER(s),
						   s->button_increment);
		}

		/*
		 * Set the default value to the beginning of the seeker.
		 */
		ewl_seeker_set_value(EWL_SEEKER(s->seeker), 0);
	} else {

		/*
		 * Set the alignment of the buttons to the seeker.
		 */
		s->buttons_alignment = ewl_theme_data_get_int(EWL_WIDGET(s),
							      "/vscrollbar/button_order");

		if (s->buttons_alignment & EWL_ALIGNMENT_TOP) {

			/*
			 * Place in increment, decrement, seeker order.
			 */
			ewl_container_append_child(EWL_CONTAINER(s),
						   s->button_increment);
			ewl_container_append_child(EWL_CONTAINER(s),
						   s->button_decrement);
			ewl_container_append_child(EWL_CONTAINER(s), s->seeker);
		} else if (s->buttons_alignment & EWL_ALIGNMENT_BOTTOM) {

			/*
			 * Place in seeker, increment, decrement order.
			 */
			ewl_container_append_child(EWL_CONTAINER(s), s->seeker);
			ewl_container_append_child(EWL_CONTAINER(s),
						   s->button_increment);
			ewl_container_append_child(EWL_CONTAINER(s),
						   s->button_decrement);
		} else {

			/*
			 * Place in increment, seeker, decrement order.
			 */
			ewl_container_append_child(EWL_CONTAINER(s),
						   s->button_increment);
			ewl_container_append_child(EWL_CONTAINER(s), s->seeker);
			ewl_container_append_child(EWL_CONTAINER(s),
						   s->button_decrement);
		}

		/*
		 * Set the default value to the beginning of the seeker.
		 */
		ewl_seeker_set_value(EWL_SEEKER(s->seeker), 1.0);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_scrollbar_set_fill_percentage - set the percentage filled by drag button
 * @s: the scrollbar to change fill percentage
 * @fp: the percentage of the scrollbar filled by the drag button
 *
 * Returns no value. Changes the area of the scrollbar that is filled by the
 * drag button.
 */
void ewl_scrollbar_set_fill_percentage(Ewl_Scrollbar * s, double fp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	/*
	 * Perform a bounds check on the fill percentage.
	 */
	if (fp < 0)
		fp = 0;
	else if (fp > 1)
		fp = 1;

	/*
	 * Set the fill percentage on the seeker.
	 */
	s->fill_percentage = fp;

	/*
	 * Adjust the step based on the fill percentage.
	 */
	if (fp == 1 || fp == 0)
		ewl_seeker_set_step(EWL_SEEKER(s->seeker), 0.05);
	else
		ewl_seeker_set_step(EWL_SEEKER(s->seeker), 1.0 - fp);

	ewl_widget_configure(EWL_WIDGET(s));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_scrollbar_get_fill_percentage - get the percentage filled by drag button
 * @s: the scrollbar to retrieve fill percentage
 *
 * Returns the percentage of the scrollbar that the drag button fills on
 * success, -1 on failure.
 */
double ewl_scrollbar_get_fill_percentage(Ewl_Scrollbar * s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, -1);

	DRETURN_FLOAT(s->fill_percentage, DLEVEL_STABLE);
}

/**
 * ewl_scrollbar_get_value - get the current value of the dragbar
 * @s: the scrollbar to get the current value
 *
 * Returns the current value of the scrollbar @s.
 */
double ewl_scrollbar_get_value(Ewl_Scrollbar * s)
{
	double          v;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, -1);

	v = ewl_seeker_get_value(EWL_SEEKER(s->seeker));

	if (EWL_BOX(s)->orientation == EWL_ORIENTATION_VERTICAL)
		v = 1.0 - v;

	if (s->fill_percentage == 1.0)
		v = 0.0;

	DRETURN_FLOAT(v, DLEVEL_UNSTABLE);
}

/**
 * ewl_scrollbar_set_flag - set the flag mask for a scrollbar
 * @s: the scrollbar to set the flags
 * @f: the flags to set for the scrollbar
 *
 * Returns no value. Sets the flags @f for the scrollbar @s.
 */
void ewl_scrollbar_set_flag(Ewl_Scrollbar * s, Ewl_ScrollBar_Flags f)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	s->flag = f;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_scrollbar_get_flag - retrieve the current flags of a scrollbar
 * @s: the scrollbar to retrieve the flags
 *
 * Returns the flags from the scrollbars @s.
 */
Ewl_ScrollBar_Flags ewl_scrollbar_get_flag(Ewl_Scrollbar * s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0);

	DRETURN_INT(s->flag, DLEVEL_STABLE);
}


/*
 * Make sure that we show all the parts of the scrollbar
 */
void __ewl_scrollbar_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Scrollbar  *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SCROLLBAR(w);

	ewl_widget_realize(s->button_decrement);
	ewl_widget_realize(s->seeker);
	ewl_widget_realize(s->button_increment);

	ewl_object_set_maximum_size(EWL_OBJECT(s->button_decrement), 10, 10);
	ewl_object_set_maximum_size(EWL_OBJECT(s->button_increment), 10, 10);

	if (EWL_BOX(s)->orientation == EWL_ORIENTATION_HORIZONTAL) {
		ewl_object_set_preferred_h(EWL_OBJECT(s), MINIMUM_H(s->seeker));
		MINIMUM_H(s) = MINIMUM_H(s->seeker);
		MAXIMUM_H(s) = MAXIMUM_H(s->seeker);
	} else {
		ewl_object_set_preferred_w(EWL_OBJECT(s), MINIMUM_W(s->seeker));
		MINIMUM_W(s) = MINIMUM_W(s->seeker);
		MAXIMUM_W(s) = MAXIMUM_W(s->seeker);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Decrement the value of the scrollbar's seeker portion
 */
void __ewl_scrollbar_decrement(Ewl_Widget * w, void *ev_data, void *user_data)
{
	double          v;
	Ewl_Scrollbar  *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SCROLLBAR(user_data);

	/*
	 * ewl_seeker_decrease(EWL_SEEKER(s->seeker));
	 */

	v = ewl_seeker_get_value(EWL_SEEKER(s->seeker));
	v -= 0.05;

	if (v < 0.0)
		v = 0.0;

	ewl_seeker_set_value(EWL_SEEKER(s->seeker), v);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Decrement the value of the scrollbar's seeker portion
 */
void __ewl_scrollbar_increment(Ewl_Widget * w, void *ev_data, void *user_data)
{
	double          v;
	Ewl_Scrollbar  *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SCROLLBAR(user_data);

	/*
	 * ewl_seeker_increase(EWL_SEEKER(s->seeker));
	 */

	v = ewl_seeker_get_value(EWL_SEEKER(s->seeker));
	v += 0.05;

	if (v > 1.0)
		v = 1.0;

	ewl_seeker_set_value(EWL_SEEKER(s->seeker), v);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_scrollbar_seeker_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Scrollbar  *sc;
	Ewl_Seeker     *se;
	int             req_w = 0, req_h = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	sc = user_data;
	se = EWL_SEEKER(w);

	req_w = CURRENT_W(w) - (INSET_LEFT(w) + INSET_RIGHT(w));
	req_h = CURRENT_H(w) - (INSET_TOP(w) + INSET_BOTTOM(w));

	/*
	 * Adjust the size of the scrollbar based on the fill percentage and
	 * orientation.
	 */
	if (EWL_BOX(sc)->orientation == EWL_ORIENTATION_HORIZONTAL) {
		req_w *= sc->fill_percentage;
	} else {
		req_h *= sc->fill_percentage;
	}

	/*
	 * Request the new size for the dragbar.
	 */
	ewl_object_request_size(EWL_OBJECT(se->dragbar), req_w, req_h);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_scrollbar_seeker_value_changed(Ewl_Widget * w, void *ev_data,
				     void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	ewl_callback_call(user_data, EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
