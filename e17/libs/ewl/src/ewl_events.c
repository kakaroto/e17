
#include <Ewl.h>


extern Ewl_Widget     *last_selected;
extern Ewl_Widget     *last_key;
extern Ewl_Widget     *last_focused;
extern Ewl_Widget     *dnd_widget;


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
	ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DAMAGE,
				ewl_ev_window_expose, NULL);
	ecore_event_handler_add(ECORE_X_EVENT_WINDOW_CONFIGURE,
				ewl_ev_window_configure, NULL);
	ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DELETE_REQUEST,
				ewl_ev_window_delete, NULL);

	/*
	 * Register dispatching functions for keyboard events.
	 */
	ecore_event_handler_add(ECORE_X_EVENT_KEY_DOWN, ewl_ev_key_down, NULL);
	ecore_event_handler_add(ECORE_X_EVENT_KEY_UP, ewl_ev_key_up, NULL);

	/*
	 * Finally, register dispatching functions for mouse events.
	 */
	ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_DOWN,
				ewl_ev_mouse_down, NULL);
	ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_UP, ewl_ev_mouse_up,
				NULL);
	ecore_event_handler_add(ECORE_X_EVENT_MOUSE_MOVE,
				ewl_ev_mouse_move, NULL);
	ecore_event_handler_add(ECORE_X_EVENT_MOUSE_OUT, ewl_ev_mouse_out,
				NULL);

	DRETURN_INT(1, DLEVEL_STABLE);
}

/**
 * ewl_ev_window_expose - handles the exposing of a window
 * @_ev: the expose event information
 *
 * Returns no value. Dispatches the expose event to the appropriate window for
 * handling.
 */
int ewl_ev_window_expose(void *data, int type, void * _ev)
{
	/*
	 * Widgets don't need to know about this usually, but we still need to
	 * let them know in case a widget is using a non-evas based draw method
	 */
	Ecore_X_Event_Window_Damage *ev;
	Ewl_Embed      *embed;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = _ev;

	embed = ewl_embed_find_by_evas_window(ev->win);
	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	evas_damage_rectangle_add(embed->evas, ev->x, ev->y, ev->w, ev->h);
	ewl_callback_call(EWL_WIDGET(embed), EWL_CALLBACK_EXPOSE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * ewl_ev_window_configure - handles configure events that occur in windows
 * @_ev: the configure event information
 *
 * Returns no value. Dispatches a configure even to the appropriate ewl
 * window.
 */
int ewl_ev_window_configure(void *data, int type, void *_ev)
{
	/*
	 * When a configure event occurs, we must update the windows geometry
	 * based on the coordinates and dimensions given in the Ecore_Event.
	 */
	Ecore_X_Event_Window_Configure *ev;
	Ewl_Window     *window;

	DENTER_FUNCTION(DLEVEL_STABLE);
	ev = _ev;

	window = ewl_window_find_window(ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	window->x = ev->x;
	window->y = ev->y;

	/*
	 * Configure events really only need to occur on resize.
	 */
	if (CURRENT_W(window) != ev->w || CURRENT_H(window) != ev->h) {
		ewl_object_request_geometry(EWL_OBJECT(window), 0, 0, ev->w,
					    ev->h);
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * ewl_ev_window_delete - handles delete events that occur to windows
 * @_ev: the delete event information
 *
 * Returns no value. Dispatches the delete event to the appropriate ewl
 * window.
 */
int ewl_ev_window_delete(void *data, int type, void *_ev)
{
	/*
	 * Retrieve the appropriate ewl_window using the x window id that is
	 * held in the eevent, and call it's handlers for a window delete event.
	 */
	Ecore_X_Event_Window_Destroy *ev;
	Ewl_Window     *window;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = _ev;

	window = ewl_window_find_window(ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_callback_call(EWL_WIDGET(window), EWL_CALLBACK_DELETE_WINDOW);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * ewl_ev_key_down - handles key down events in windows
 * @_ev: the key down event information
 *
 * Returns no value. Dispatches the key down event to the appropriate ewl
 * window.
 */
int ewl_ev_key_down(void *data, int type, void *_ev)
{
	Ewl_Widget     *temp;
	Ewl_Embed      *embed;
	Ecore_X_Event_Key_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = _ev;

	embed = ewl_embed_find_by_evas_window(ev->win);

	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	/*
	 * If a widget has been selected then we send the keystroke to the
	 * appropriate widget.
	 */
	if (!last_key || !ewl_container_parent_of(EWL_WIDGET(embed),
				last_key)) {
		if (last_selected)
			last_key = last_selected;
		else
			last_key = EWL_WIDGET(embed);
	}

	/*
	 * Dispatcher of key down events, these get sent to the last widget
	 * selected, and every parent above it.
	 */
	temp = last_key;
	while (temp) {
		if (!(ewl_object_has_state(EWL_OBJECT(temp),
					EWL_FLAG_STATE_DISABLED)))
			ewl_callback_call_with_event_data(temp,
					EWL_CALLBACK_KEY_DOWN, ev);
		temp = temp->parent;
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * ewl_ev_key_up - handles key up events in windows
 * @_ev: the key up event information
 *
 * Returns no value. Dispatches the key up event to the appropriate ewl
 * window.
 */
int ewl_ev_key_up(void *data, int type, void *_ev)
{
	/*
	 * Dispatch key up events to the appropriate widget
	 */
	Ewl_Embed      *embed;
	Ecore_X_Event_Key_Up *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = _ev;

	embed = ewl_embed_find_by_evas_window(ev->win);
	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	/*
	 * If a widget has received a keydown event, then it should receive a
	 * matching key up event
	 */
	if (last_key)
		ewl_callback_call_with_event_data(last_key, EWL_CALLBACK_KEY_UP,
						  ev);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}


/**
 * ewl_ev_mouse_down - handles mouse down events in windows
 * @_ev: the mouse down event information
 *
 * Returns no value. Dispatches the mouse down event to the appropriate ewl
 * window. Also determines the widgets clicked state.
 */
int ewl_ev_mouse_down(void *data, int type, void *_ev)
{
	Ewl_Widget     *widget = NULL;
	Ewl_Widget     *temp = NULL;
	Ewl_Embed      *embed;
	Ecore_X_Event_Mouse_Button_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = _ev;

	embed = ewl_embed_find_by_evas_window(ev->win);
	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	widget = ewl_container_get_child_at_recursive(EWL_CONTAINER(embed),
			ev->x, ev->y);

	/*
	 * Determine whether this widget has already been selected, if not,
	 * deselect the previously selected widget and notify it of the
	 * change. Then select the new widget and notify it of the selection.
	 */
	if (widget != last_selected) {
		if (last_selected) {
			ewl_object_remove_state(EWL_OBJECT(last_selected),
					EWL_FLAG_STATE_SELECTED);
			ewl_callback_call(last_selected, EWL_CALLBACK_DESELECT);
		}

		if (widget && !(ewl_object_has_state(EWL_OBJECT(widget),
					EWL_FLAG_STATE_DISABLED))) {
			ewl_object_add_state(EWL_OBJECT(widget),
					EWL_FLAG_STATE_SELECTED);
			ewl_callback_call(widget, EWL_CALLBACK_SELECT);
		}
	}

	/*
	 * While the mouse is down the widget has a pressed state, the widget
	 * and its parents are notified in this change of state.
	 */
	temp = widget;
	while (temp) {
		if (!(ewl_object_has_state(EWL_OBJECT(temp),
					EWL_FLAG_STATE_DISABLED))) {
			ewl_object_add_state(EWL_OBJECT(temp),
					EWL_FLAG_STATE_PRESSED);
			ewl_callback_call_with_event_data(temp,
					EWL_CALLBACK_MOUSE_DOWN, ev);

			if (ev->double_click) {
				ewl_callback_call_with_event_data(temp,
						EWL_CALLBACK_DOUBLE_CLICKED,
						ev);
			}
		}
		temp = temp->parent;
	}

	/*
	 * Save the newly selected widget for further reference
	 */
	last_key = last_selected = widget;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}


/**
 * ewl_ev_mouse_up - handles mouse up events in windows
 * @_ev: the mouse up event information
 *
 * Returns no value. Dispatches the mouse up event to the appropriate ewl
 * window. Also determines the widgets clicked state.
 */
int ewl_ev_mouse_up(void *data, int type, void *_ev)
{
	Ewl_Widget     *temp;
	Ewl_Embed      *embed;
	Ecore_X_Event_Mouse_Button_Up *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = _ev;

	embed = ewl_embed_find_by_evas_window(ev->win);
	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	/*
	 * When the mouse is released the widget no longer has a pressed state,
	 * the widget and its parents are notified in this change of state.
	 */
	temp = last_selected;
	while (temp) {
		if (!(ewl_object_has_state(EWL_OBJECT(temp),
				EWL_FLAG_STATE_DISABLED))) {
			ewl_object_remove_state(EWL_OBJECT(temp),
					EWL_FLAG_STATE_PRESSED);
			ewl_callback_call_with_event_data(temp,
					EWL_CALLBACK_MOUSE_UP, ev);

		}

		temp = temp->parent;
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}


/**
 * ewl_ev_mouse_move - handles mouse move events in windows
 * @_ev: the mouse move event information
 *
 * Returns no value. Dispatches the mouse move event to the appropriate ewl
 * window.
 */
int ewl_ev_mouse_move(void *data, int type, void *_ev)
{
	Ewl_Widget     *widget;
	Ewl_Embed      *embed;
	Ecore_X_Event_Mouse_Move *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = _ev;

	embed = ewl_embed_find_by_evas_window(ev->win);
	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	widget = ewl_container_get_child_at_recursive(EWL_CONTAINER(embed),
			ev->x, ev->y);

	/*
	 * Defocus all widgets up to the level of a shared parent of old and
	 * newly focused widgets.
	 */
	while (last_focused && (widget != last_focused) &&
			!ewl_container_parent_of(last_focused, widget)) {
		ewl_object_remove_state(EWL_OBJECT(last_focused),
				EWL_FLAG_STATE_HILITED);
		ewl_callback_call(last_focused, EWL_CALLBACK_FOCUS_OUT);
		last_focused = last_focused->parent;
	}

	/*
	 * Pass out the movement event up the chain, allows parents to
	 * react to mouse movement in their children.
	 */
	last_focused = widget;
	while (last_focused) {

		if (!(ewl_object_has_state(EWL_OBJECT(last_focused),
					EWL_FLAG_STATE_DISABLED))) {

			/*
			 * First mouse move event in a widget marks it focused.
			 */
			if (!(ewl_object_has_state(EWL_OBJECT(last_focused),
						EWL_FLAG_STATE_HILITED))) {
				ewl_object_add_state(EWL_OBJECT(last_focused),
						EWL_FLAG_STATE_HILITED);
				ewl_callback_call(last_focused,
						EWL_CALLBACK_FOCUS_IN);
			}

			ewl_callback_call_with_event_data(last_focused,
					EWL_CALLBACK_MOUSE_MOVE, ev);
		}
		last_focused = last_focused->parent;
	}

	last_focused = widget;

	if (dnd_widget && ewl_object_has_state(EWL_OBJECT(dnd_widget),
				EWL_FLAG_STATE_DND))
		ewl_callback_call_with_event_data(dnd_widget,
						  EWL_CALLBACK_MOUSE_MOVE, ev);

	if (last_selected && ewl_object_has_state(EWL_OBJECT(last_selected),
				EWL_FLAG_STATE_PRESSED))
		ewl_callback_call_with_event_data(last_selected,
						  EWL_CALLBACK_MOUSE_MOVE, ev);
	else
		dnd_widget = NULL;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * ewl_ev_mouse_out - handles the mouse out events in windows
 * @_ev: the mouse out event information
 *
 * Returns no value. Dispatches the mouse out event to the appropriate ewl
 * window.
 */
int ewl_ev_mouse_out(void *data, int type, void *_ev)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	while (last_focused) {
		ewl_callback_call(last_focused, EWL_CALLBACK_FOCUS_OUT);
		last_focused = last_focused->parent;
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}
