
#include <Ewl.h>

Ewl_Widget *last_selected;
Ewl_Widget *last_key;
Ewl_Widget *last_focused;
Ewl_Widget *dnd_widget;

unsigned int mouse_down;

static void ewl_ev_window_expose(Eevent * _ev);
static void ewl_ev_window_configure(Eevent * _ev);
static void ewl_ev_window_delete(Eevent * _ev);
static void ewl_ev_window_reparent(Eevent * _ev);
static void ewl_ev_key_down(Eevent * _ev);
static void ewl_ev_key_up(Eevent * _ev);
static void ewl_ev_mouse_down(Eevent * _ev);
static void ewl_ev_mouse_up(Eevent * _ev);
static void ewl_ev_mouse_move(Eevent * _ev);

int
ewl_ev_init(void)
{
	last_selected = NULL;
	last_key = NULL;
	last_focused = NULL;
	dnd_widget = NULL;
	mouse_down = FALSE;

	e_event_filter_handler_add(EV_WINDOW_EXPOSE, ewl_ev_window_expose);
	e_event_filter_handler_add(EV_WINDOW_CONFIGURE,
				   ewl_ev_window_configure);
	e_event_filter_handler_add(EV_WINDOW_DELETE, ewl_ev_window_delete);

/*	e_event_filter_handler_add(EV_WINDOW_REPARENT,
				   ewl_ev_window_reparent);*/
	e_event_filter_handler_add(EV_KEY_DOWN, ewl_ev_key_down);
	e_event_filter_handler_add(EV_KEY_UP, ewl_ev_key_up);
	e_event_filter_handler_add(EV_MOUSE_DOWN, ewl_ev_mouse_down);
	e_event_filter_handler_add(EV_MOUSE_UP, ewl_ev_mouse_up);
	e_event_filter_handler_add(EV_MOUSE_MOVE, ewl_ev_mouse_move);

	return 1;
}

static void
ewl_ev_window_expose(Eevent * _ev)
{
	Ev_Window_Expose *ev;
	Ewl_Window *window;

	ev = _ev->event;

	window = ewl_window_find_window_by_evas_window(ev->win);
	if (window) {
		evas_update_rect(EWL_WIDGET(window)->evas, ev->x, ev->y,
				 ev->w, ev->h);
	}
}

static void
ewl_ev_window_configure(Eevent * _ev)
{
	Ev_Window_Configure *ev;
	Ewl_Window *window;

	ev = _ev->event;

	window = ewl_window_find_window(ev->win);
	if (window) {
		EWL_OBJECT(window)->request.x = ev->x;
		EWL_OBJECT(window)->request.y = ev->y;
		EWL_OBJECT(window)->request.w = ev->w;
		EWL_OBJECT(window)->request.h = ev->h;
		ewl_callback_call(EWL_WIDGET(window),
				  EWL_CALLBACK_CONFIGURE);
	}
}

static void
ewl_ev_window_reparent(Eevent * _ev)
{
	Ev_Window_Reparent *ev = _ev->event;
	Ewl_Window *window;

	window = ewl_window_find_window(ev->win);
	if (window) {
		ewl_callback_call(EWL_WIDGET(window),
				  EWL_CALLBACK_REPARENT);
	}
}

/*
 * Retrieve the appropriate ewl_window using the x window id that is held
 * in the eevent, and call it's handlers for a window delete event.
 */
static void
ewl_ev_window_delete(Eevent * _ev)
{
	Ev_Window_Delete *ev;
	Ewl_Window *window;

	ev = _ev->event;

	window = ewl_window_find_window(ev->win);
	if (window) {
		ewl_callback_call(EWL_WIDGET(window),
				  EWL_CALLBACK_DELETE_WINDOW);
	}
}

static void
ewl_ev_key_down(Eevent * _ev)
{
	Ewl_Window *window;
	Ev_Key_Down *ev;

	ev = _ev->event;

	window = ewl_window_find_window_by_evas_window(ev->win);

	if (window) {
		if (last_selected)
			ewl_callback_call_with_event_data(last_selected,
							  EWL_CALLBACK_KEY_DOWN,
							  ev);
		last_key = last_selected;
	}
}

static void
ewl_ev_key_up(Eevent * _ev)
{
	Ewl_Window *window;
	Ev_Key_Up *ev;

	ev = _ev->event;

	window = ewl_window_find_window_by_evas_window(ev->win);

	if (window) {
		if (last_key)
			ewl_callback_call_with_event_data(last_key,
							  EWL_CALLBACK_KEY_UP,
							  ev);
	}
}

static void
ewl_ev_mouse_down(Eevent * _ev)
{
	Ewl_Widget *widget;
	Ewl_Window *window;
	Ev_Mouse_Down *ev;

	ev = _ev->event;

	window = ewl_window_find_window_by_evas_window(ev->win);

	if (window) {
		widget = ewl_container_get_child_at_recursive(EWL_CONTAINER
							      (window),
							      ev->x,
							      ev->y);

		if (widget != last_selected) {
			if (last_selected) {
				last_selected->state &=
				    ~EWL_STATE_SELECTED;
				ewl_callback_call(last_selected,
						  EWL_CALLBACK_UNSELECT);
			}
			if (widget) {
				widget->state |= EWL_STATE_SELECTED;
				ewl_callback_call(widget,
						  EWL_CALLBACK_SELECT);
			}
		}

		if (widget) {
			widget->state |= EWL_STATE_PRESSED;
			ewl_callback_call_with_event_data(widget,
							  EWL_CALLBACK_MOUSE_DOWN,
							  ev);
		}

		last_selected = widget;
		mouse_down = 1;
	}
}

static void
ewl_ev_mouse_up(Eevent * _ev)
{
	Ewl_Window *window;
	Ev_Mouse_Up *ev;

	ev = _ev->event;

	window = ewl_window_find_window_by_evas_window(ev->win);
	if (window)
		if (last_selected) {
			last_selected->state &= ~EWL_STATE_PRESSED;
			ewl_callback_call_with_event_data(last_selected,
							  EWL_CALLBACK_MOUSE_UP,
							  ev);
		}
}

static void
ewl_ev_mouse_move(Eevent * _ev)
{
	Ewl_Widget *widget;
	Ewl_Window *window;
	Ev_Mouse_Move *ev;

	ev = _ev->event;

	window = ewl_window_find_window_by_evas_window(ev->win);
	if (window) {
		widget = ewl_container_get_child_at_recursive(EWL_CONTAINER
							      (window),
							      ev->x,
							      ev->y);

		if (widget) {
			widget->state |= EWL_STATE_HILITED;
			if (last_focused != widget)
				ewl_callback_call(widget,
						  EWL_CALLBACK_FOCUS_IN);

			ewl_callback_call_with_event_data(widget,
							  EWL_CALLBACK_MOUSE_MOVE,
							  ev);
		}
		if (last_focused != widget && last_focused) {
			last_focused->state &= ~EWL_STATE_HILITED;
			ewl_callback_call(last_focused,
					  EWL_CALLBACK_FOCUS_OUT);
		}

		if (last_focused && last_focused->state & EWL_STATE_DND)
			dnd_widget = last_focused;

		if (dnd_widget && dnd_widget->state & EWL_STATE_DND)
			ewl_callback_call_with_event_data(dnd_widget,
							  EWL_CALLBACK_MOUSE_MOVE,
							  ev);

		if (last_selected &&
		    last_selected->state & EWL_STATE_PRESSED)
			ewl_callback_call_with_event_data(last_selected,
							  EWL_CALLBACK_MOUSE_MOVE,
							  ev);
		else
			dnd_widget = NULL;

		last_focused = widget;
	}
}
