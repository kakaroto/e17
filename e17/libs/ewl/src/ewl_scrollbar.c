
#include <Ewl.h>

static void __ewl_scrollbar_realize(Ewl_Widget * s, void *ev_data,
				    void *user_data);

static void __ewl_scrollbar_decrement(Ewl_Widget * w, void *ev_data,
				      void *user_data);
static void __ewl_scrollbar_increment(Ewl_Widget * w, void *ev_data,
				      void *user_data);

/**
 * ewl_scrollbar_new - allocate and initialize a new scrollbar widget
 *
 * @orientation: the desired orientation of the scrollbar
 *
 * Returns NULL on failure, or a pointer to the newly allocated scrollbar
 * widget on success.
 */
Ewl_Widget *
ewl_scrollbar_new(Ewl_Orientation orientation)
{
	Ewl_Scrollbar *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = NEW(Ewl_Scrollbar, 1);
	if (!s)
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	memset(s, 0, sizeof(Ewl_Scrollbar));

	s->decrement = ewl_button_new(NULL);
	s->increment = ewl_button_new(NULL);
	s->seeker = ewl_seeker_new(orientation);

	ewl_scrollbar_init(s, orientation);

	ewl_container_append_child(EWL_CONTAINER(s), s->decrement);
	ewl_container_append_child(EWL_CONTAINER(s), s->increment);
	ewl_container_prepend_child(EWL_CONTAINER(s), s->seeker);

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
void
ewl_scrollbar_init(Ewl_Scrollbar * s, Ewl_Orientation orientation)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	ewl_box_init(EWL_BOX(s), orientation);

	ewl_callback_append(EWL_WIDGET(s), EWL_CALLBACK_REALIZE,
			    __ewl_scrollbar_realize, NULL);

	ewl_object_set_fill_policy(EWL_OBJECT(s), EWL_FILL_POLICY_FILL);

	ewl_callback_append(s->decrement, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_scrollbar_decrement, s);

	ewl_callback_append(s->increment, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_scrollbar_increment, s);

	ewl_object_set_maximum_size(EWL_OBJECT(s->decrement),
				    MINIMUM_W(s->seeker),
				    MINIMUM_W(s->seeker));
	ewl_object_set_maximum_size(EWL_OBJECT(s->increment),
				    MINIMUM_W(s->seeker),
				    MINIMUM_W(s->seeker));

	/*
	 * Keep the bounding box snugg to the edges of the seeker portion of
	 * the scrollbar. The buttons will still fit on the ends since the max
	 * and min dimensions are still unset for the ends.
	 */
	MAXIMUM_W(s) = MAXIMUM_W(s->seeker);
	MAXIMUM_H(s) = MAXIMUM_H(s->seeker);
	MAXIMUM_W(s) = MAXIMUM_W(s->seeker);
	MAXIMUM_H(s) = MAXIMUM_H(s->seeker);

	if (orientation == EWL_ORIENTATION_HORIZONTAL)
	  {
		  ewl_widget_set_appearance(s->decrement,
					    "/appearance/scrollbar/horizontal/increment");
		  ewl_widget_set_appearance(s->seeker,
					    "/appearance/scrollbar/horizontal");
		  ewl_widget_set_appearance(s->increment,
					    "/appearance/scrollbar/horizontal/decrement");
	  }
	else
	  {
		  ewl_widget_set_appearance(s->decrement,
					    "/appearance/scrollbar/vertical/increment");
		  ewl_widget_set_appearance(s->seeker,
					    "/appearance/scrollbar/vertical");
		  ewl_widget_set_appearance(s->increment,
					    "/appearance/scrollbar/vertical/decrement");
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Make sure that we show all the parts of the scrollbar
 */
static void
__ewl_scrollbar_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Scrollbar *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SCROLLBAR(w);

	ewl_widget_realize(s->decrement);
	ewl_widget_realize(s->seeker);
	ewl_widget_realize(s->increment);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Decrement the value of the scrollbar's seeker portion
 */
static void
__ewl_scrollbar_decrement(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Scrollbar *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SCROLLBAR(user_data);

	ewl_seeker_set_value(EWL_SEEKER(s->seeker),
			     ewl_seeker_get_value(EWL_SEEKER(s->seeker)) - 1);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Decrement the value of the scrollbar's seeker portion
 */
static void
__ewl_scrollbar_increment(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Scrollbar *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SCROLLBAR(user_data);

	ewl_seeker_set_value(EWL_SEEKER(s->seeker),
			     ewl_seeker_get_value(EWL_SEEKER(s->seeker)) + 1);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
