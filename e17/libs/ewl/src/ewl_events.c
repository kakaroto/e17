
#include <Ewl.h>


Ewl_Widget     *last_selected = NULL;
Ewl_Widget     *last_key = NULL;
Ewl_Widget     *last_focused = NULL;
Ewl_Widget     *dnd_widget = NULL;

static void     ewl_ev_window_expose(Ecore_Event * _ev);
static void     ewl_ev_window_configure(Ecore_Event * _ev);
static void     ewl_ev_window_delete(Ecore_Event * _ev);

static void     ewl_ev_key_down(Ecore_Event * _ev);
static void     ewl_ev_key_up(Ecore_Event * _ev);
static void     ewl_ev_mouse_down(Ecore_Event * _ev);
static void     ewl_ev_mouse_up(Ecore_Event * _ev);
static void     ewl_ev_mouse_move(Ecore_Event * _ev);
static void     ewl_ev_mouse_out(Ecore_Event * _ev);


/**
 * ewl_ev_init - initialize the event handlers for dispatching to proper widgets
 *
 * Returns true or false to indicate success in initiating the event filters.
 */
int ewl_ev_init(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Register dispatching functions for window events.
	 */
	ecore_event_filter_handler_add(ECORE_EVENT_WINDOW_EXPOSE,
				       ewl_ev_window_expose);
	ecore_event_filter_handler_add(ECORE_EVENT_WINDOW_CONFIGURE,
				       ewl_ev_window_configure);
	ecore_event_filter_handler_add(ECORE_EVENT_WINDOW_DELETE,
				       ewl_ev_window_delete);

	/*
	 * Register dispatching functions for keyboard events.
	 */
	ecore_event_filter_handler_add(ECORE_EVENT_KEY_DOWN, ewl_ev_key_down);
	ecore_event_filter_handler_add(ECORE_EVENT_KEY_UP, ewl_ev_key_up);

	/*
	 * Finally, register dispatching functions for mouse events.
	 */
	ecore_event_filter_handler_add(ECORE_EVENT_MOUSE_DOWN,
				       ewl_ev_mouse_down);
	ecore_event_filter_handler_add(ECORE_EVENT_MOUSE_UP, ewl_ev_mouse_up);
	ecore_event_filter_handler_add(ECORE_EVENT_MOUSE_MOVE,
				       ewl_ev_mouse_move);
	ecore_event_filter_handler_add(ECORE_EVENT_MOUSE_OUT, ewl_ev_mouse_out);

	DRETURN_INT(1, DLEVEL_STABLE);
}

/**
 * ewl_ev_window_expose - handles the exposing of a window
 * @_ev: the expose event information
 *
 * Returns no value. Dispatches the expose event to the appropriate window for
 * handling.
 */
static void ewl_ev_window_expose(Ecore_Event * _ev)
{
	/*
	 * Widgets don't need to know about this usually, but we still need to
	 * let them know in case a widget is using a non-evas based draw method
	 */
	Ecore_Event_Window_Expose *ev;
	Ewl_Window     *window;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = _ev->event;

	window = ewl_window_find_window_by_evas_window(ev->win);
	if (!window)
		DRETURN(DLEVEL_STABLE);

	evas_damage_rectangle_add(window->evas, ev->x, ev->y, ev->w, ev->h);
	ewl_callback_call(EWL_WIDGET(window), EWL_CALLBACK_EXPOSE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_ev_window_configure - handles configure events that occur in windows
 * @_ev: the configure event information
 *
 * Returns no value. Dispatches a configure even to the appropriate ewl
 * window.
 */
static void ewl_ev_window_configure(Ecore_Event * _ev)
{
	/*
	 * When a configure event occurs, we must update the windows geometry
	 * based on the coordinates and dimensions given in the Ecore_Event.
	 */
	Ecore_Event_Window_Configure *ev;
	Ewl_Window     *window;

	DENTER_FUNCTION(DLEVEL_STABLE);
	ev = _ev->event;

	window = ewl_window_find_window(ev->win);
	if (!window)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Configure events really only need to occur on resize.
	 */
	if (CURRENT_W(window) != ev->w || CURRENT_H(window) != ev->h) {
		ewl_object_request_geometry(EWL_OBJECT(window), 0, 0, ev->w,
					    ev->h);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_ev_window_delete - handles delete events that occur to windows
 * @_ev: the delete event information
 *
 * Returns no value. Dispatches the delete event to the appropriate ewl
 * window.
 */
static void ewl_ev_window_delete(Ecore_Event * _ev)
{
	/*
	 * Retrieve the appropriate ewl_window using the x window id that is
	 * held in the eevent, and call it's handlers for a window delete event.
	 */
	Ecore_Event_Window_Delete *ev;
	Ewl_Window     *window;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = _ev->event;

	window = ewl_window_find_window(ev->win);

	if (!window)
		DRETURN(DLEVEL_STABLE);

	ewl_callback_call(EWL_WIDGET(window), EWL_CALLBACK_DELETE_WINDOW);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_ev_key_down - handles key down events in windows
 * @_ev: the key down event information
 *
 * Returns no value. Dispatches the key down event to the appropriate ewl
 * window.
 */
static void ewl_ev_key_down(Ecore_Event * _ev)
{
	/*
	 * Dispatcher of key down events, these get sent to the last widget
	 * selected.
	 */
	Ewl_Window     *window;
	Ecore_Event_Key_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = _ev->event;

	window = ewl_window_find_window_by_evas_window(ev->win);

	if (!window)
		DRETURN(DLEVEL_STABLE);

	/*
	 * If a widget has been selected then we send the keystroke to the
	 * appropriate widget.
	 */
	if (last_selected) {
		ewl_callback_call_with_event_data(last_selected,
						  EWL_CALLBACK_KEY_DOWN, ev);
		last_key = last_selected;
	} else {
		ewl_callback_call_with_event_data(EWL_WIDGET(window),
						  EWL_CALLBACK_KEY_DOWN, ev);
		last_key = EWL_WIDGET(window);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_ev_key_up - handles key up events in windows
 * @_ev: the key up event information
 *
 * Returns no value. Dispatches the key up event to the appropriate ewl
 * window.
 */
static void ewl_ev_key_up(Ecore_Event * _ev)
{
	/*
	 * Dispatch key up events to the appropriate widget
	 */
	Ewl_Window     *window;
	Ecore_Event_Key_Up *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = _ev->event;

	window = ewl_window_find_window_by_evas_window(ev->win);
	if (!window)
		DRETURN(DLEVEL_STABLE);

	/*
	 * If a widget has received a keydown event, then it should receive a
	 * matching key up event
	 */
	if (last_key)
		ewl_callback_call_with_event_data(last_key, EWL_CALLBACK_KEY_UP,
						  ev);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_ev_mouse_down - handles mouse down events in windows
 * @_ev: the mouse down event information
 *
 * Returns no value. Dispatches the mouse down event to the appropriate ewl
 * window. Also determines the widgets clicked state.
 */
static void ewl_ev_mouse_down(Ecore_Event * _ev)
{
	Ewl_Widget     *widget = NULL;
	Ewl_Window     *window;
	Ecore_Event_Mouse_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = _ev->event;

	window = ewl_window_find_window_by_evas_window(ev->win);
	if (!window)
		DRETURN(DLEVEL_STABLE);

	widget = ewl_window_get_child_at(window, ev->x, ev->y);

	/*
	 * Determine whether this widget has already been selected, if not,
	 * deselect the previously selected widget and notify it of the
	 * change. Then select the new widget and notify it of the selection.
	 */
	if (widget != last_selected) {
		if (last_selected) {
			last_selected->state &= ~EWL_STATE_SELECTED;
			ewl_callback_call(last_selected, EWL_CALLBACK_DESELECT);
		}

		if (widget && !(widget->state & EWL_STATE_DISABLED)) {
			widget->state |= EWL_STATE_SELECTED;
			ewl_callback_call(widget, EWL_CALLBACK_SELECT);
		}
	}

	/*
	 * While the mouse is down the widget has a pressed state, the widget
	 * is notified in this change of state.
	 */
	if (widget && !(widget->state & EWL_STATE_DISABLED)) {
		widget->state |= EWL_STATE_PRESSED;
		ewl_callback_call_with_event_data(widget,
						  EWL_CALLBACK_MOUSE_DOWN, ev);
	}

	/*
	 * Save the newly selected widget for further reference
	 */
	last_selected = widget;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_ev_mouse_up - handles mouse up events in windows
 * @_ev: the mouse up event information
 *
 * Returns no value. Dispatches the mouse up event to the appropriate ewl
 * window. Also determines the widgets clicked state.
 */
static void ewl_ev_mouse_up(Ecore_Event * _ev)
{
	Ewl_Window     *window;
	Ecore_Event_Mouse_Up *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = _ev->event;

	window = ewl_window_find_window_by_evas_window(ev->win);
	if (!window)
		DRETURN(DLEVEL_STABLE);

	if (last_selected && !(last_selected->state & EWL_STATE_DISABLED)) {
		last_selected->state &= ~EWL_STATE_PRESSED;
		ewl_callback_call_with_event_data(last_selected,
						  EWL_CALLBACK_MOUSE_UP, ev);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_ev_mouse_move - handles mouse move events in windows
 * @_ev: the mouse move event information
 *
 * Returns no value. Dispatches the mouse move event to the appropriate ewl
 * window.
 */
static void ewl_ev_mouse_move(Ecore_Event * _ev)
{
	Ewl_Widget     *widget;
	Ewl_Window     *window;
	Ecore_Event_Mouse_Move *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = _ev->event;

	window = ewl_window_find_window_by_evas_window(ev->win);
	if (!window)
		DRETURN(DLEVEL_STABLE);

	widget = ewl_window_get_child_at(window, ev->x, ev->y);

	if (last_focused && (widget != last_focused)) {
		last_focused->state &= ~EWL_STATE_HILITED;
		ewl_callback_call(last_focused, EWL_CALLBACK_FOCUS_OUT);
	}

	if (widget && !(widget->state & (EWL_STATE_DISABLED | EWL_STATE_HILITED))) {
		widget->state |= EWL_STATE_HILITED;
		ewl_callback_call(widget, EWL_CALLBACK_FOCUS_IN);

		ewl_callback_call_with_event_data(widget,
						  EWL_CALLBACK_MOUSE_MOVE, ev);
		last_focused = widget;
	}

	if (dnd_widget && dnd_widget->state & EWL_STATE_DND)
		ewl_callback_call_with_event_data(dnd_widget,
						  EWL_CALLBACK_MOUSE_MOVE, ev);

	if (last_selected && last_selected->state & EWL_STATE_PRESSED)
		ewl_callback_call_with_event_data(last_selected,
						  EWL_CALLBACK_MOUSE_MOVE, ev);
	else
		dnd_widget = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_ev_mouse_out - handles the mouse out events in windows
 * @_ev: the mouse out event information
 *
 * Returns no value. Dispatches the mouse out event to the appropriate ewl
 * window.
 */
static void ewl_ev_mouse_out(Ecore_Event * _ev)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (last_focused) {
		ewl_callback_call(last_focused, EWL_CALLBACK_FOCUS_OUT);
		last_focused = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
