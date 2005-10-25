#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static int ewl_selectionbar_open_bar_timer(void *ev_data);
static int ewl_selectionbar_close_bar_timer(void *ev_data);

static Ecore_Timer *open_timer;
static Ecore_Timer *close_timer;

/**
 * ewl_selectionbar_new - create a new selectionbar
 *
 * Returns a pointer to a newly allocated scrollpane on success, NULL on
 * failure.
 */
Ewl_Widget *
ewl_selectionbar_new(Ewl_Widget *parent)
{
	Ewl_Selectionbar *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("parent", parent, NULL);
	DCHECK_TYPE_RET("parent", parent, "widget", NULL);

	s = NEW(Ewl_Selectionbar, 1);
	if (!s)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_selectionbar_init(s, parent)) {
		ewl_widget_destroy(EWL_WIDGET(s));
		s = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * ewl_selectionbar_init - initialize a new selectionbar
 * @param s: the selectionbar to initialize
 * @param parent: the parent widget
 * 
 * Returns no value. Sets up the default callbacks and values for the
 * the selectionbar
 */
int
ewl_selectionbar_init(Ewl_Selectionbar *s, Ewl_Widget *parent)
{
	Ewl_Widget *w;
	Ewl_Embed *embed;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, FALSE);
	DCHECK_PARAM_PTR_RET("parent", parent, FALSE);
	DCHECK_TYPE_RET("parent", parent, "widget", FALSE);

	w = EWL_WIDGET(s);

	if (!ewl_floater_init(EWL_FLOATER(s)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(w, "selectionbar");
	ewl_widget_inherit(w, "selectionbar");

	ewl_floater_follow_set(EWL_FLOATER(s), parent);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    ewl_selectionbar_configure_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    ewl_selectionbar_realize_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_SHOW, ewl_selectionbar_show_cb,
			    NULL);

	s->bar = NEW(Ewl_Container, 1);
	if (!s->bar)
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	if (!ewl_container_init(EWL_CONTAINER(s->bar)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_object_fill_policy_set(EWL_OBJECT(s->bar), EWL_FLAG_FILL_HFILL |
				   EWL_FLAG_FILL_HSHRINK);
	ewl_container_child_append(EWL_CONTAINER(w), EWL_WIDGET(s->bar));
	ewl_callback_append(EWL_WIDGET(s->bar), EWL_CALLBACK_FOCUS_OUT,
			    ewl_selectionbar_focus_out_cb, w);
	ewl_callback_append(EWL_WIDGET(s->bar), EWL_CALLBACK_FOCUS_IN,
			    ewl_selectionbar_focus_in_cb, w);

	embed = ewl_embed_widget_find(parent);
	ewl_callback_append(EWL_WIDGET(embed), EWL_CALLBACK_CONFIGURE,
			    ewl_selectionbar_parent_configure_cb, w);

	s->scroller.top = ewl_vbox_new();
	ewl_object_size_request(EWL_OBJECT(s->scroller.top), 30, 10);
	ewl_container_child_append(s->bar, s->scroller.top);

/*	ewl_callback_append(s->scroller.top, EWL_CALLBACK_MOUSE_MOVE,
			ewl_selectionbar_mouse_move_child, s);
*/
	ewl_widget_appearance_set(s->scroller.top, "/selectionbar/scroller/top");

	s->scroller.bottom = ewl_vbox_new();
	ewl_object_size_request(EWL_OBJECT(s->scroller.bottom), 30, 10);
	ewl_container_child_append(s->bar, s->scroller.bottom);

/*	ewl_callback_append(s->scroller.bottom, EWL_CALLBACK_MOUSE_MOVE,
			ewl_selectionbar_mouse_move_child, s);
*/
	ewl_widget_appearance_set(s->scroller.bottom, "/selectionbar/scroller/bottom");

	ewl_container_add_notify_set(EWL_CONTAINER(w),
				 ewl_selectionbar_child_add_cb);

	s->OPEN = 1;
	s->mouse_x = 0;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

void
ewl_selectionbar_child_add_cb(Ewl_Container *parent, Ewl_Widget *child)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("parent", parent);
	DCHECK_PARAM_PTR("child", child);
	DCHECK_TYPE("parent", parent, "container");
	DCHECK_TYPE("child", child, "widget");

	ewl_container_child_append(EWL_SELECTIONBAR(parent)->bar, child);
	ewl_container_child_remove(parent, child);

	ewl_callback_append(child, EWL_CALLBACK_MOUSE_MOVE,
			    ewl_selectionbar_mouse_move_child_cb, parent);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_selectionbar_realize_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Selectionbar *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, "widget");

	s = EWL_SELECTIONBAR(w);

	if (!s->w)
		s->w = CURRENT_W(EWL_OBJECT(w->parent));
	if (!s->h)
		s->h = 80;

	ewl_object_size_request(EWL_OBJECT(w), s->w, s->h);
	ewl_object_size_request(EWL_OBJECT(s->bar), s->w, s->h);

	ewl_object_maximum_h_set(EWL_OBJECT(s->bar), s->h);

	ewl_widget_show(EWL_WIDGET(EWL_SELECTIONBAR(w)->scroller.top));
	ewl_widget_show(EWL_WIDGET(EWL_SELECTIONBAR(w)->scroller.bottom));
	ewl_widget_show(EWL_WIDGET(EWL_SELECTIONBAR(w)->bar));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_selectionbar_show_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Selectionbar *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, "widget");

	s = EWL_SELECTIONBAR(w);
	ecore_timer_add(0.01, ewl_selectionbar_close_bar_timer, s->bar);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_selectionbar_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Selectionbar *s;
//	Ecore_List *children;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, "widget");

	s = EWL_SELECTIONBAR(w);
	if (s->w < CURRENT_W(EWL_OBJECT(EWL_FLOATER(s)->follows)))
		s->w = CURRENT_W(EWL_OBJECT(EWL_FLOATER(s)->follows));

	if (s->w > CURRENT_W(EWL_OBJECT(EWL_FLOATER(s)->follows)))
		s->w = CURRENT_W(EWL_OBJECT(EWL_FLOATER(s)->follows));

	if (CURRENT_H(EWL_OBJECT(s)) < 5)
		ewl_object_h_request(EWL_OBJECT(s), 5);

	ewl_widget_configure(EWL_WIDGET(s->bar));

	/*
	children = EWL_CONTAINER(EWL_FLOATER(w)->follows)->children;
	if (children) {
		ecore_list_goto_first(children);
		while ((child = ecore_list_next(children)) != NULL)
			printf("child layer = %d\n", LAYER(child));
		s = s;
	}
	*/

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_selectionbar_parent_configure_cb(Ewl_Widget *w __UNUSED__,
				void *ev_data __UNUSED__, void *user_data)
{
	Ewl_Selectionbar *s;
	Ewl_Object *o;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, "widget");

	s = EWL_SELECTIONBAR(user_data);
	o = EWL_OBJECT(s);

	printf("bar height: %d\n", CURRENT_H(EWL_OBJECT(s->bar)));

	ewl_object_size_request(o, s->w, CURRENT_H(EWL_OBJECT(s->bar)));

	if (CURRENT_H(EWL_OBJECT(s->bar)) < 5)
		ewl_object_size_request(o, s->w, 5);
	ewl_selectionbar_focus_out_cb(EWL_WIDGET(s->bar), NULL, s);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_selectionbar_focus_in_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
							void *user_data)
{
	Ewl_Selectionbar *s;
	Ewl_Object *o;
	Ecore_List *children;
	Ewl_Widget *child;
	int cum_width;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_TYPE("w", w, "widget");

	s = EWL_SELECTIONBAR(user_data);
	o = EWL_OBJECT(w);
	children = EWL_CONTAINER(w)->children;
	cum_width = 0;

	if (s->OPEN)
		DRETURN(DLEVEL_STABLE);

	ecore_timer_add(0.01, ewl_selectionbar_open_bar_timer, w);

	ewl_widget_show(s->scroller.top);
	ewl_widget_show(s->scroller.bottom);

	ecore_list_goto_first(children);
	while ((child = ecore_list_next(children)) != NULL) {
		if (child != EWL_WIDGET(s->scroller.top) &&
		    child != EWL_WIDGET(s->scroller.bottom)) {

			ewl_object_size_request(EWL_OBJECT(child), 60, 40);

			ewl_object_position_request(EWL_OBJECT(child),
						    CURRENT_X(o) + cum_width,
						    CURRENT_Y(o) +
						    ((CURRENT_H(o) -
						      CURRENT_H(child)) / 2));

			ewl_widget_show(child);

			cum_width += CURRENT_W(child) + 5;
		}
	}
	s->OPEN = 1;

/*	ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
			ewl_selectionbar_children_animator, s);

*/
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_selectionbar_focus_out_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Selectionbar *s;
	Ecore_List *children;
	Ewl_Widget *child;
	Ewl_Embed *embed;
	Ewl_Object *o;
	Ewl_Event_Mouse_Out *ev;

	int mx, my;
	int wx, wy;
	int ww, wh;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_TYPE("w", w, "widget");

	ev = ev_data;
	s = EWL_SELECTIONBAR(user_data);
	o = EWL_OBJECT(w);
	children = EWL_CONTAINER(w)->children;

	/*
	 * First check that the mouse left the selectionbar and that
	 * focus didn't just go to one of it's children
	 */
	embed = ewl_embed_widget_find(w);

	mx = ev->x;
	my = ev->y;

	wx = CURRENT_X(o);
	wy = CURRENT_Y(o);
	ww = CURRENT_W(o);
	wh = CURRENT_H(o);

	if ((((mx > wx) && (mx < (wx + ww))) &&
	     ((my > wy) && (my < (wy + wh)))))
		DRETURN(DLEVEL_STABLE);

	ewl_callback_del(w, EWL_CALLBACK_MOUSE_MOVE,
			 ewl_selectionbar_children_animator_cb);

	ewl_widget_hide(s->scroller.top);
	ewl_widget_hide(s->scroller.bottom);

	ecore_list_goto_first(children);
	while ((child = ecore_list_next(children)) != NULL)
		if (child != EWL_WIDGET(s->scroller.top) &&
		    child != EWL_WIDGET(s->scroller.bottom))
			ewl_widget_hide(child);

	s->OPEN = 0;
	ecore_timer_add(0.01, ewl_selectionbar_close_bar_timer, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

int
ewl_selectionbar_open_bar_timer(void *ev_data)
{
	Ewl_Widget *w;
	Ewl_Object *o;
	Ewl_Object *so;
	Ewl_Selectionbar *s;
	int retval = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ecore_timer_del(close_timer);

	w = EWL_WIDGET(ev_data);
	o = EWL_OBJECT(w);
	s = EWL_SELECTIONBAR(w->parent);
	so = EWL_OBJECT(s);

	if (CURRENT_H(o) < s->h) {
		ewl_object_size_request(o, s->w, CURRENT_H(o) + s->h / 10);
		ewl_object_size_request(so, CURRENT_W(o), CURRENT_H(o));

		retval = TRUE;

		ewl_widget_configure(EWL_WIDGET(s));
	} else {
		ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
				    ewl_selectionbar_children_animator_cb, s);
		ewl_callback_append(s->scroller.top, EWL_CALLBACK_MOUSE_MOVE,
				    ewl_selectionbar_mouse_move_child_cb, s);
		ewl_callback_append(s->scroller.bottom, EWL_CALLBACK_MOUSE_MOVE,
				    ewl_selectionbar_mouse_move_child_cb, s);
	}

	DRETURN_INT(retval, DLEVEL_STABLE);
}

int
ewl_selectionbar_close_bar_timer(void *ev_data)
{
	Ewl_Widget *w;
	Ewl_Object *o;
	Ewl_Object *so;
	Ewl_Selectionbar *s;
	int retval = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ecore_timer_del(open_timer);

	w = EWL_WIDGET(ev_data);
	o = EWL_OBJECT(w);
	s = EWL_SELECTIONBAR(w->parent);
	so = EWL_OBJECT(s);

	if (CURRENT_H(o) > 10) {
		ewl_object_size_request(o, s->w,
					CURRENT_H(o) - (s->h / 10) + 5);
		ewl_object_size_request(so, s->w,
					CURRENT_H(o) - (s->h / 10) + 5);

		retval = TRUE;
	} else {
		ewl_object_size_request(o, s->w, 5);
		ewl_object_size_request(so, s->w, 5);
	}

	DRETURN_INT(retval, DLEVEL_STABLE);
}

void
ewl_selectionbar_mouse_move_child_cb(Ewl_Widget *w __UNUSED__, void *ev_data,
				  void *user_data)
{
	Ewl_Selectionbar *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);

	s = EWL_SELECTIONBAR(user_data);
	ewl_selectionbar_children_animator_cb(EWL_WIDGET(s->bar), ev_data, s);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_selectionbar_children_animator_cb(Ewl_Widget *w, void *ev_data,
				      void *user_data)
{

	Ewl_Widget *child;
	Ewl_Object *o;
	Ecore_List *children;
	Ewl_Event_Mouse_Move *ev;
	Ewl_Selectionbar *s;
	int x, old_x;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_TYPE("w", w, "widget");

	s = EWL_SELECTIONBAR(user_data);
	o = EWL_OBJECT(w);
	children = EWL_CONTAINER(w)->children;

	ev = ev_data;
	x = ev->x - CURRENT_X(o);

	if (!s->mouse_x) old_x = x;
	else old_x = s->mouse_x;

	s->mouse_x = x;
	ewl_object_position_request(EWL_OBJECT(s->scroller.top), x - 15,
				    CURRENT_Y(o) + 5);

	ewl_object_position_request(EWL_OBJECT(s->scroller.bottom),
				    x - 15, CURRENT_Y(o) + CURRENT_H(o) - 15);

	ecore_list_goto_first(children);
	while ((child = ecore_list_next(children)) != NULL) {
		if (child != EWL_WIDGET(s->scroller.bottom) &&
		    child != EWL_WIDGET(s->scroller.top)) {

			if (CURRENT_W(EWL_OBJECT(child)) < 2 ||
			    CURRENT_H(EWL_OBJECT(child)) < 2) {
				ewl_object_size_request(EWL_OBJECT(child),
						ewl_object_preferred_w_get
							(EWL_OBJECT(child)),
						ewl_object_preferred_h_get
							(EWL_OBJECT(child)));
			}

			ewl_object_position_request(EWL_OBJECT(child),
						    CURRENT_X(child) +
						    (old_x - x),
						    CURRENT_Y(o) +
						    ((CURRENT_H(o) -
						      CURRENT_H(child)) / 2));
		}
	}
}

