
#include <Ewl.h>


static void __ewl_scrollbar_realize(Ewl_Widget * s, void *ev_data,
				    void *user_data);
static void __ewl_scrollbar_decrement(Ewl_Widget * w, void *ev_data,
				      void *user_data);
static void __ewl_scrollbar_increment(Ewl_Widget * w, void *ev_data,
				      void *user_data);
void __ewl_scrollbar_seeker_configure(Ewl_Widget * w, void *ev_data,
				      void *user_data);
void __ewl_scrollbar_seeker_value_changed(Ewl_Widget * w, void *ev_data,
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

	ZERO(s, Ewl_Scrollbar, 1);

	s->button_decrement = ewl_button_new(NULL);
	s->button_increment = ewl_button_new(NULL);
	s->seeker = ewl_seeker_new(orientation);

	ewl_scrollbar_init(s, orientation);

	if (EWL_BOX(s)->orientation == EWL_ORIENTATION_HORIZONTAL)
	  {
		  switch (s->buttons_alignment)
		    {
		    case EWL_ALIGNMENT_LEFT:
			    ewl_container_append_child(EWL_CONTAINER(s),
						       s->button_decrement);
			    ewl_container_append_child(EWL_CONTAINER(s),
						       s->button_increment);
			    ewl_container_append_child(EWL_CONTAINER(s),
						       s->seeker);
			    break;
		    case EWL_ALIGNMENT_CENTER:
			    ewl_container_append_child(EWL_CONTAINER(s),
						       s->button_decrement);
			    ewl_container_append_child(EWL_CONTAINER(s),
						       s->seeker);
			    ewl_container_append_child(EWL_CONTAINER(s),
						       s->button_increment);
			    break;
		    default:
			    ewl_container_append_child(EWL_CONTAINER(s),
						       s->seeker);
			    ewl_container_append_child(EWL_CONTAINER(s),
						       s->button_decrement);
			    ewl_container_append_child(EWL_CONTAINER(s),
						       s->button_increment);
			    break;
		    }
	  }
	else
	  {
		  switch (s->buttons_alignment)
		    {
		    case EWL_ALIGNMENT_TOP:
			    ewl_container_append_child(EWL_CONTAINER(s),
						       s->button_decrement);
			    ewl_container_append_child(EWL_CONTAINER(s),
						       s->button_increment);
			    ewl_container_append_child(EWL_CONTAINER(s),
						       s->seeker);
			    break;
		    case EWL_ALIGNMENT_CENTER:
			    ewl_container_append_child(EWL_CONTAINER(s),
						       s->button_decrement);
			    ewl_container_append_child(EWL_CONTAINER(s),
						       s->seeker);
			    ewl_container_append_child(EWL_CONTAINER(s),
						       s->button_increment);
			    break;
		    default:
			    ewl_container_append_child(EWL_CONTAINER(s),
						       s->seeker);
			    ewl_container_append_child(EWL_CONTAINER(s),
						       s->button_increment);
			    ewl_container_append_child(EWL_CONTAINER(s),
						       s->button_decrement);
			    break;
		    }
	  }

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

void
ewl_scrollbar_set_fill_percentage(Ewl_Scrollbar * s, double fp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	if (fp < 0)
		fp = 0;
	else if (fp > 1)
		fp = 1;

	s->fill_percentage = fp;

	if (fp == 1 || fp == 0)
		ewl_seeker_set_step(EWL_SEEKER(s->seeker), 0.05);
	else
		ewl_seeker_set_step(EWL_SEEKER(s->seeker), 1.0 - fp);

	ewl_widget_configure(EWL_WIDGET(s));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

double
ewl_scrollbar_get_fill_percentage(Ewl_Scrollbar * s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, -1);

	DRETURN_FLOAT(s->fill_percentage, DLEVEL_STABLE);
}

double
ewl_scrollbar_get_value(Ewl_Scrollbar * s)
{
	double v;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, -1);

	v = ewl_seeker_get_value(EWL_SEEKER(s->seeker));

	if (EWL_BOX(s)->orientation == EWL_ORIENTATION_VERTICAL)
		v = 1.0 - v;

	if (s->fill_percentage == 1.0)
		v = 0.0;

	DRETURN_FLOAT(v, DLEVEL_STABLE);
}

void
ewl_scrollbar_set_flag(Ewl_Scrollbar * s, Ewl_ScrollBar_Flags f)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	s->flag = f;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_ScrollBar_Flags
ewl_scrollbar_get_flag(Ewl_Scrollbar * s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0);

	DRETURN_INT(s->flag, DLEVEL_STABLE);
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
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	w = EWL_WIDGET(s);

	ewl_box_init(EWL_BOX(w), orientation);

	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_scrollbar_realize, NULL);

	ewl_callback_append(s->button_decrement, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_scrollbar_decrement, s);

	ewl_callback_append(s->button_increment, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_scrollbar_increment, s);
	ewl_callback_append(s->seeker, EWL_CALLBACK_CONFIGURE,
			    __ewl_scrollbar_seeker_configure, s);

	ewl_object_set_fill_policy(EWL_OBJECT(s->button_decrement),
				   EWL_FILL_POLICY_NORMAL);
	ewl_object_set_fill_policy(EWL_OBJECT(s->button_increment),
				   EWL_FILL_POLICY_NORMAL);

	ewl_object_set_alignment(EWL_OBJECT(s->button_decrement),
				 EWL_ALIGNMENT_CENTER);
	ewl_object_set_alignment(EWL_OBJECT(s->button_increment),
				 EWL_ALIGNMENT_CENTER);

	s->fill_percentage = 1.0;

	ewl_callback_append(s->seeker, EWL_CALLBACK_VALUE_CHANGED,
			    __ewl_scrollbar_seeker_value_changed, s);

	ewl_seeker_set_range(EWL_SEEKER(s->seeker), 1);
	ewl_seeker_set_step(EWL_SEEKER(s->seeker), 0.05);

	if (orientation == EWL_ORIENTATION_HORIZONTAL)
	  {
		  ewl_widget_set_appearance(s->button_decrement,
					    "/appearance/scrollbar/horizontal/button_increment");
		  ewl_widget_set_appearance(s->seeker,
					    "/appearance/scrollbar/horizontal");
		  ewl_widget_set_appearance(s->button_increment,
					    "/appearance/scrollbar/horizontal/button_decrement");
		  s->buttons_alignment = EWL_ALIGNMENT_RIGHT;
	  }
	else
	  {
		  ewl_widget_set_appearance(s->button_decrement,
					    "/appearance/scrollbar/vertical/button_increment");
		  ewl_widget_set_appearance(s->seeker,
					    "/appearance/scrollbar/vertical");
		  ewl_widget_set_appearance(s->button_increment,
					    "/appearance/scrollbar/vertical/button_decrement");
		  s->buttons_alignment = EWL_ALIGNMENT_BOTTOM;
		  ewl_seeker_set_value(EWL_SEEKER(s->seeker), 1);
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

	ewl_widget_realize(s->button_decrement);
	ewl_widget_realize(s->seeker);
	ewl_widget_realize(s->button_increment);

	MAXIMUM_W(s->button_decrement) = 10;
	MAXIMUM_H(s->button_decrement) = 10;

	MAXIMUM_W(s->button_increment) = 10;
	MAXIMUM_H(s->button_increment) = 10;

	if (EWL_BOX(s)->orientation == EWL_ORIENTATION_HORIZONTAL)
	  {
		  MINIMUM_H(s) = MINIMUM_H(s->seeker);
		  MAXIMUM_H(s) = MAXIMUM_H(s->seeker);
	  }
	else
	  {
		  MINIMUM_W(s) = MINIMUM_W(s->seeker);
		  MAXIMUM_W(s) = MAXIMUM_W(s->seeker);
	  }

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

	ewl_seeker_decrease(EWL_SEEKER(s->seeker));

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

	ewl_seeker_increase(EWL_SEEKER(s->seeker));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_scrollbar_seeker_configure(Ewl_Widget * w, void *ev_data,
				 void *user_data)
{
	Ewl_Scrollbar *sc;
	Ewl_Seeker *se;
	int ll = 0, rr = 0, tt = 0, bb = 0;
	int req_w = 0, req_h = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	sc = user_data;
	se = EWL_SEEKER(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &ll, &rr, &tt, &bb);

	req_w = REQUEST_W(w) - ll - rr;
	req_h = REQUEST_H(w) - tt - bb;


	if (EWL_BOX(sc)->orientation == EWL_ORIENTATION_HORIZONTAL)
	  {
		  req_w *= sc->fill_percentage;
		  ewl_object_request_w(EWL_OBJECT(se->dragbar), req_w);
	  }
	else
	  {
		  req_h *= sc->fill_percentage;
		  ewl_object_request_h(EWL_OBJECT(se->dragbar), req_h);
	  }

	ewl_widget_configure(se->dragbar);

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
