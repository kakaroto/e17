
#include <Ewl.h>

void            __ewl_selectionbar_realize(Ewl_Widget * w, void *ev_data,
					   void *user_data);
void            __ewl_selectionbar_configure(Ewl_Widget * w, void *ev_data,
					     void *user_data);
void            __ewl_selectionbar_focus_in(Ewl_Widget * w, void *ev_data,
					    void *user_data);
void            __ewl_selectionbar_focus_out(Ewl_Widget * w, void *ev_data,
					     void *user_data);
void            __ewl_selectionbar_animator(Ewl_Widget * w, void *ev_data,
					    void *user_data);
void            __open_bar_cb(int val, void *ev_data);
void            __close_bar_cb(int val, void *ev_data);

extern void     __ewl_box_configure(Ewl_Widget * w, void *user_data,
				    void *ev_data);

/**
 * ewl_selectionbar_new - create a new selectionbar
 *
 * Returns a pointer to a newly allocated scrollpane on success, NULL on
 * failure.
 */
Ewl_Widget     *
ewl_selectionbar_new(Ewl_Widget * parent)
{
	Ewl_Selectionbar *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = NEW(Ewl_Selectionbar, 1);
	if (!s)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	memset(s, 0, sizeof(Ewl_Selectionbar));


	ewl_selectionbar_init(s, parent);

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}


/**
 * ewl_selectionbar_init - initialize a new selectionbar
 * @s: the selectionbar to initialize
 * 
 * Returns no value. Sets up the default callbacks and values for the
 * the selectionbar
 */
void
ewl_selectionbar_init(Ewl_Selectionbar * s, Ewl_Widget * parent)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	w = EWL_WIDGET(s);


	ewl_floater_init(EWL_FLOATER(s), parent);
	ewl_object_set_fill_policy(EWL_OBJECT(s), EWL_FILL_POLICY_NORMAL);
	ewl_theme_data_set_str(w,
			       "/appearance/box/vertical/base/visible", "yes");


	s->scroller.top = ewl_vbox_new();
	ewl_object_set_fill_policy(EWL_OBJECT(s->scroller.top),
				   EWL_FILL_POLICY_SHRINK);
	ewl_container_append_child(EWL_CONTAINER(s), s->scroller.top);
	ewl_theme_data_set_str(s->scroller.top,
			       "/appearance/box/vertical/base/visible", "yes");


	s->scroller.bottom = ewl_vbox_new();
	ewl_object_set_fill_policy(EWL_OBJECT(s->scroller.bottom),
				   EWL_FILL_POLICY_SHRINK);
	ewl_container_append_child(EWL_CONTAINER(s), s->scroller.bottom);
	ewl_theme_data_set_str(s->scroller.bottom,
			       "/appearance/box/vertical/base/visible", "yes");


	ewl_callback_append(s->scroller.top, EWL_CALLBACK_FOCUS_OUT,
			    __ewl_selectionbar_focus_out, w);
	ewl_callback_append(s->scroller.bottom, EWL_CALLBACK_FOCUS_OUT,
			    __ewl_selectionbar_focus_out, w);


	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_selectionbar_configure, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_selectionbar_realize, NULL);
	ewl_callback_append(w, EWL_CALLBACK_FOCUS_IN,
			    __ewl_selectionbar_focus_in, NULL);
	ewl_callback_append(w, EWL_CALLBACK_FOCUS_OUT,
			    __ewl_selectionbar_focus_out, NULL);

	s->OPEN = 0;
	s->mouse_x = 0;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_selectionbar_set_size - set width/height
 *
 * Returns no value
 */
void
ewl_selectionbar_set_size(Ewl_Selectionbar * s, int w, int h1, int h2)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	s->w = w;
	s->h = h1;

	ewl_object_request_size(EWL_OBJECT(s), w, h2);
	ewl_widget_configure(EWL_WIDGET(s));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void
__ewl_selectionbar_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Selectionbar *s;
	Ewl_Floater    *f;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = EWL_SELECTIONBAR(w);
	f = EWL_FLOATER(w);


	if (!s->w)
		s->w = CURRENT_W(EWL_OBJECT(f->follows));
	if (!s->h)
		s->h = 45;

	ewl_object_request_size(EWL_OBJECT(s), s->w, 3);
	ewl_object_request_size(EWL_OBJECT(s->scroller.top), 30, 10);
	ewl_object_request_size(EWL_OBJECT(s->scroller.bottom), 30, 10);

	ewl_widget_realize(w);


	/*
	 * start the animation routine
	 */
	ecore_add_event_timer("close", 0.01, __close_bar_cb, 1, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}



void
__ewl_selectionbar_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewd_List       *children;
	Ewl_Widget     *current_child;
	Ewl_Selectionbar *s;
	Ewl_Floater    *f;
	int             wx, wy, ww, wh;
	int             cum_width;

	DENTER_FUNCTION(DLEVEL_STABLE);



	s = EWL_SELECTIONBAR(w);
	f = EWL_FLOATER(s);

	/* make sure the width is the same as the parent */
	ewl_object_get_current_size(EWL_OBJECT(f->follows), &ww, &wh);
	if (ww != CURRENT_W(EWL_OBJECT(w))) {
		ewl_object_request_size(EWL_OBJECT(w), ww, s->h);
		s->w = ww;
	}

	ewl_object_get_current_geometry(EWL_OBJECT(w), &wx, &wy, &ww, &wh);

	/* position the children */
	children = EWL_CONTAINER(s)->children;
	ewd_list_goto_first(children);
	cum_width = 0;
	while ((current_child = ewd_list_next(children)) != NULL) {
		if (current_child != EWL_WIDGET(s->scroller.top) &&
		    current_child != EWL_WIDGET(s->scroller.bottom)) {

			ewl_object_request_position(EWL_OBJECT
						    (current_child),
						    cum_width, wy + 15);

			cum_width += CURRENT_W(EWL_OBJECT(current_child));
			ewl_widget_configure(current_child);
		}
	}


	ewl_object_request_position(EWL_OBJECT(s->scroller.top), wx, wy);
	ewl_object_request_position(EWL_OBJECT(s->scroller.bottom),
				    wx, ((wy + wh) - 10));


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}



void
__ewl_selectionbar_focus_in(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewd_List       *children;
	Ewl_Widget     *current_child;
	Ewl_Selectionbar *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = EWL_SELECTIONBAR(w);

	if (!(s->OPEN)) {

		/* add animation callback to the bar and the children */

		children = EWL_CONTAINER(s)->children;
		ewd_list_goto_first(children);
		while ((current_child = ewd_list_next(children)) != NULL)
			ewl_callback_append(current_child,
					    EWL_CALLBACK_MOUSE_MOVE,
					    __ewl_selectionbar_animator, w);

		ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
				    __ewl_selectionbar_animator, NULL);

		ecore_add_event_timer("open", 0.01, __open_bar_cb, 1, w);

		ewl_widget_realize(EWL_WIDGET(s->scroller.top));
		ewl_widget_realize(EWL_WIDGET(s->scroller.bottom));

		ewl_object_request_size(EWL_OBJECT(s->scroller.top), 30, 10);
		ewl_object_request_size(EWL_OBJECT(s->scroller.bottom), 30, 10);


		ewl_widget_configure(EWL_WIDGET(s->scroller.top));
		ewl_widget_configure(EWL_WIDGET(s->scroller.bottom));
//              ewl_widget_configure(w);


		s->OPEN = 1;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}



void
__ewl_selectionbar_focus_out(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Window     *window;
	Ewl_Selectionbar *s;
	Ewl_Object     *obj;
	Ewd_List       *children;
	Ewl_Widget     *current_child;
	int             mx, my;	/* mouse x and y */
	int             wx, wy;	/* widget x and y */
	int             wl, wh;	/* widget length and height */

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = EWL_SELECTIONBAR(user_data);
	if (s == NULL)
		s = EWL_SELECTIONBAR(w);

	obj = EWL_OBJECT(s);

	/*
	 * First we need to check if the mouse actually left the
	 * selectionbar or if focus just went to one of it's children.
	 */
	window = ewl_window_find_window_by_widget(EWL_WIDGET(s));
	ecore_pointer_xy(window->evas_window, &mx, &my);

	wx = CURRENT_X(obj);
	wy = CURRENT_Y(obj);

	wl = s->w;
	wh = s->h;

	if ((((mx > wx) && (mx < (wx + wl)))
	     && ((my > wy) && (my < (wy + wh)))))
		return;


	/*
	 * If we are still in here that means the mouse has left the
	 * selectionbar and we take actions to collapse it.
	 */
	ewl_callback_del(EWL_WIDGET(s), EWL_CALLBACK_MOUSE_MOVE,
			 __ewl_selectionbar_animator);

	children = EWL_CONTAINER(s)->children;
	ewd_list_goto_first(children);
	while ((current_child = ewd_list_next(children)) != NULL)
		ewl_callback_del(current_child, EWL_CALLBACK_MOUSE_MOVE,
				 __ewl_selectionbar_animator);


	ecore_add_event_timer("close", 0.01, __close_bar_cb, 1, s);

	ewl_widget_hide(s->scroller.top);
	ewl_widget_hide(s->scroller.bottom);


	ewl_widget_configure(s->scroller.top);
	ewl_widget_configure(s->scroller.bottom);
//      ewl_widget_configure(EWL_WIDGET(s));

	s->OPEN = 0;
	s->mouse_x = 0;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}



void
__open_bar_cb(int val, void *ev_data)
{
	Ewl_Window     *window;
	Ewl_Selectionbar *s;
	Ewl_Widget     *current_child, *w;
	Ewl_Object     *o;
	Ewd_List       *children;
	int             mx, my;	/* mouse x/y */
	int             wx, wy;	/* widget x/y */
	int             wl, wh;	/* widget length/height */
	int             cx, cy;	/* children x/y */
	int             x, y;	/* calculations x/y */
	int             width_children;	/* with of all the children combined */

	o = EWL_OBJECT(ev_data);
	s = EWL_SELECTIONBAR(ev_data);
	w = EWL_WIDGET(ev_data);

	children = EWL_CONTAINER(s)->children;
	ecore_del_event_timer("close");

	if (CURRENT_H(o) < s->h) {
		ewl_object_request_size(o, s->w, CURRENT_H(o) + s->h / 10);
		ecore_add_event_timer("open", 0.01, __open_bar_cb, ++val, w);
		ewl_widget_configure(w);

	}

	else {
		window = ewl_window_find_window_by_widget(w);

		ecore_pointer_xy(window->evas_window, &mx, &my);
		wx = CURRENT_X(o);
		wy = CURRENT_Y(o);
		wl = s->w;
		wh = s->h;


		/* calculate total width of the children */
		ewd_list_goto_first(children);
		width_children = 0;
		while ((current_child = ewd_list_next(children)) != NULL) {
			if (current_child != EWL_WIDGET(s->scroller.bottom)
			    && current_child != EWL_WIDGET(s->scroller.top))

				width_children +=
					CURRENT_W(EWL_OBJECT(current_child));

		}


		/* place the children correctly accordint to where the
		 * mouse entered the selectionbar */
		ewd_list_goto_first(children);
		while ((current_child = ewd_list_next(children)) != NULL) {
			if (current_child != EWL_WIDGET(s->scroller.bottom)
			    && current_child != EWL_WIDGET(s->scroller.top)) {
				cy = CURRENT_Y(EWL_OBJECT(current_child));
				cx = CURRENT_X(EWL_OBJECT(current_child));

				x = (wx + wl) - (mx - wx) + ((cx - wx) -
							     (width_children
							      / 2));
				y = cy;

				ewl_object_request_position(EWL_OBJECT
							    (current_child),
							    x, y);

				ewl_widget_configure(current_child);
			}
		}

	}

}


void
__close_bar_cb(int val, void *ev_data)
{

	Ewl_Object     *o = EWL_OBJECT(ev_data);
	Ewl_Selectionbar *s = EWL_SELECTIONBAR(ev_data);


	ecore_del_event_timer("open");

	if (CURRENT_H(EWL_OBJECT(ev_data)) > 2) {
		ewl_object_request_size(o, s->w,
					CURRENT_H(o) - (s->h / 10) + 2);
		ecore_add_event_timer("close", 0.01, __close_bar_cb, ++val,
				      ev_data);
	}

	ewl_widget_configure(EWL_WIDGET(o));
}



void
__ewl_selectionbar_animator(Ewl_Widget * w, void *ev_data, void *user_data)
{

	Ewl_Widget     *current_child;
	Ewl_Selectionbar *s;
	Ewl_Object     *o;
	int             x, old_x;
	Ewd_List       *children;
	Ecore_Event_Mouse_Move *ev;

	if ((s = EWL_SELECTIONBAR(user_data)) == NULL)
		s = EWL_SELECTIONBAR(w);

	o = EWL_OBJECT(s);
	children = EWL_CONTAINER(s)->children;

	ev = ev_data;
	x = ev->x - CURRENT_X(o);
	if (!s->mouse_x)
		old_x = x;
	else
		old_x = s->mouse_x;
	s->mouse_x = x;

	CURRENT_X(EWL_OBJECT(s->scroller.bottom)) = x - 15;
	CURRENT_X(EWL_OBJECT(s->scroller.top)) = x - 15;


	ewl_widget_configure(s->scroller.top);
	ewl_widget_configure(s->scroller.bottom);


	ewd_list_goto_first(children);
	while ((current_child = ewd_list_next(children)) != NULL) {
		if (current_child != EWL_WIDGET(s->scroller.bottom) &&
		    current_child != EWL_WIDGET(s->scroller.top)) {

			CURRENT_X(EWL_OBJECT(current_child)) =
				CURRENT_X(EWL_OBJECT(current_child))
				+ (old_x - x);

			ewl_widget_configure(current_child);
		}
	}

}
