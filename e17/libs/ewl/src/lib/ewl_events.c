#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

extern Ecore_List *ewl_embed_list;
static unsigned int key_modifiers = 0;

#ifdef ENABLE_EWL_SOFTWARE_X11
int ewl_ev_x_window_expose(void *data, int type, void *_ev);
int ewl_ev_x_window_configure(void *data, int type, void *_ev);
int ewl_ev_x_window_delete(void *data, int type, void *_ev);

int ewl_ev_x_key_down(void *data, int type, void *_ev);
int ewl_ev_x_key_up(void *data, int type, void *_ev);
int ewl_ev_x_mouse_down(void *data, int type, void *_ev);
int ewl_ev_x_mouse_wheel(void *data, int type, void *_ev);
int ewl_ev_x_mouse_up(void *data, int type, void *_ev);
int ewl_ev_x_mouse_move(void *data, int type, void *_ev);
int ewl_ev_x_mouse_out(void *data, int type, void *_ev);
int ewl_ev_x_focus_in(void *data, int type, void *_ev);
int ewl_ev_x_focus_out(void *data, int type, void *_ev);
int ewl_ev_x_paste(void *data, int type, void *_ev);

int ewl_ev_dnd_position(void *data, int type, void *_ev);
int ewl_ev_dnd_enter(void *data, int type, void *_ev);
int ewl_ev_dnd_leave(void *data, int type, void *_ev);
int ewl_ev_dnd_drop(void *data, int type, void *_ev);
int ewl_ev_dnd_selection_notify(void *data, int type, void *_ev);
#endif

#ifdef ENABLE_EWL_FB
int ewl_ev_fb_key_down(void *data, int type, void *_ev);
int ewl_ev_fb_key_up(void *data, int type, void *_ev);
int ewl_ev_fb_mouse_down(void *data, int type, void *_ev);
int ewl_ev_fb_mouse_up(void *data, int type, void *_ev);
int ewl_ev_fb_mouse_move(void *data, int type, void *_ev);
#endif


/**
 * @return Returns true or false to indicate success in initializing events.
 * @brief Initialize the event handlers for dispatching to proper widgets
 */
int
ewl_ev_init(void)
{
	unsigned int engine;

	DENTER_FUNCTION(DLEVEL_STABLE);

	engine = ewl_engine_mask_get();
#ifdef ENABLE_EWL_SOFTWARE_X11
	/*
	 * Register dispatching functions for window events.
	 */
	if (engine & EWL_ENGINE_X11) {
		ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DAMAGE,
					ewl_ev_x_window_expose, NULL);
		ecore_event_handler_add(ECORE_X_EVENT_WINDOW_CONFIGURE,
					ewl_ev_x_window_configure, NULL);
		ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DELETE_REQUEST,
					ewl_ev_x_window_delete, NULL);

		/*
		 * Register dispatching functions for keyboard events.
		 */
		ecore_event_handler_add(ECORE_X_EVENT_KEY_DOWN,
					ewl_ev_x_key_down, NULL);
		ecore_event_handler_add(ECORE_X_EVENT_KEY_UP, ewl_ev_x_key_up,
					NULL);

		/* 
		 * Register dispatching functions for DND events 
		 */
		ecore_event_handler_add(ECORE_X_EVENT_XDND_POSITION,
					ewl_ev_dnd_position, NULL);
		ecore_event_handler_add(ECORE_X_EVENT_XDND_ENTER,
					ewl_ev_dnd_enter, NULL);
		ecore_event_handler_add(ECORE_X_EVENT_XDND_LEAVE,
					ewl_ev_dnd_leave, NULL);
		ecore_event_handler_add(ECORE_X_EVENT_XDND_DROP,
					ewl_ev_dnd_drop, NULL);
		ecore_event_handler_add(ECORE_X_EVENT_SELECTION_NOTIFY,
					ewl_ev_dnd_selection_notify, NULL);

		/*
		 * Finally, register dispatching functions for mouse events.
		 */
		ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_DOWN,
					ewl_ev_x_mouse_down, NULL);
		ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_UP,
					ewl_ev_x_mouse_up, NULL);
		ecore_event_handler_add(ECORE_X_EVENT_MOUSE_MOVE,
					ewl_ev_x_mouse_move, NULL);
		ecore_event_handler_add(ECORE_X_EVENT_MOUSE_WHEEL,
					ewl_ev_x_mouse_wheel, NULL);
		ecore_event_handler_add(ECORE_X_EVENT_MOUSE_OUT,
					ewl_ev_x_mouse_out, NULL);
		ecore_event_handler_add(ECORE_X_EVENT_WINDOW_FOCUS_IN,
					ewl_ev_x_focus_in, NULL);
		ecore_event_handler_add(ECORE_X_EVENT_WINDOW_FOCUS_OUT,
					ewl_ev_x_focus_out, NULL);

		/*
		 * Selection callbacks to allow for pasting.
		 */
		ecore_event_handler_add(ECORE_X_EVENT_SELECTION_NOTIFY,
					ewl_ev_x_paste, NULL);
	}
#endif

#ifdef ENABLE_EWL_FB
	if (engine & EWL_ENGINE_FB) {
		ecore_event_handler_add(ECORE_FB_EVENT_KEY_DOWN,
					ewl_ev_fb_key_down, NULL);
		ecore_event_handler_add(ECORE_FB_EVENT_KEY_UP, ewl_ev_fb_key_up,
					NULL);
		ecore_event_handler_add(ECORE_FB_EVENT_MOUSE_BUTTON_DOWN,
					ewl_ev_fb_mouse_down, NULL);
		ecore_event_handler_add(ECORE_FB_EVENT_MOUSE_BUTTON_UP,
					ewl_ev_fb_mouse_up, NULL);
		ecore_event_handler_add(ECORE_FB_EVENT_MOUSE_MOVE,
					ewl_ev_fb_mouse_move, NULL);
	}
#endif

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @return Returns the current mask of modifier keys.
 * @brief Retrieve the current mask of modifiers that are set.
 */
unsigned int
ewl_ev_modifiers_get(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_INT(key_modifiers, DLEVEL_STABLE);
}

/**
 * @param modifiers: the key modifiers to set
 * @return Returns no value.
 * @brief Sets the key modifiers to the given value
 */
void
ewl_ev_modifiers_set(unsigned int modifiers)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	key_modifiers = modifiers;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

#ifdef ENABLE_EWL_SOFTWARE_X11

/*
 * In general all of the X event handlers should find their matching window
 * with ewl_window_window_find, and not ewl_embed_evas_window_find. If the
 * embed function is used, then we get duplicate events for apps that setup
 * their own handlers and embed EWL. The exception to this is selection events
 * such as copy/paste and DND. These events need to be handled for embedded
 * EWL, but have no equivalent in the Evas callback namespace.
 */

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the expose event information
 * @return Returns no value.
 * @brief Handles the exposing of a window
 *
 * Dispatches the expose event to the appropriate window for handling.
 */
int
ewl_ev_x_window_expose(void *data __UNUSED__, int type __UNUSED__, void * e)
{
	/*
	 * Widgets don't need to know about this usually, but we still need to
	 * let them know in case a widget is using a non-evas based draw method
	 */
	Ecore_X_Event_Window_Damage *ev;
	Ewl_Window *window;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	window = ewl_window_window_find((void *)ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	evas_damage_rectangle_add(EWL_EMBED(window)->evas, ev->x, ev->y, ev->w, ev->h);
	ewl_callback_call(EWL_WIDGET(window), EWL_CALLBACK_EXPOSE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the configure event information
 * @return Returns no value.
 * @brief Handles configure events that occur in windows
 *
 * Dispatches a configure even to the appropriate ewl window.
 */
int
ewl_ev_x_window_configure(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	/*
	 * When a configure event occurs, we must update the windows geometry
	 * based on the coordinates and dimensions given in the Ecore_Event.
	 */
	Ecore_X_Event_Window_Configure *ev;
	Ewl_Window *window;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	window = ewl_window_window_find((void *)ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	/*
	 * Save coords and queue a configure event if the window is moved.
	 */
	if ((ev->from_wm) && (ev->x != window->x)) {
		window->x = ev->x;
	}

	if ((ev->from_wm) && (ev->y != window->y)) {
		window->y = ev->y;
	}

	ewl_widget_configure(EWL_WIDGET(window));

	/*
	 * Configure events really only need to occur on resize.
	 */
	if ((ewl_object_current_w_get(EWL_OBJECT(window)) != ev->w) || (ewl_object_current_h_get(EWL_OBJECT(window)) != ev->h)) {

    /* 
     * Right now this is commented out, as it was preventing windows from resizing
     * when WE were the ones setting the size (mainly on initial show of the
     * window). It would be nice to add this back in to limit the # of window
     * resizes occuring. (As long as it doesn't break initial size.)
     */
		/* if (ev->from_wm) window->flags |= EWL_WINDOW_USER_CONFIGURE; */
		ewl_object_geometry_request(EWL_OBJECT(window), 0, 0, ev->w,
					    ev->h);
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the delete event information
 * @return Returns no value.
 * @brief Handles delete events that occur to windows
 *
 * Dispatches the delete event to the appropriate ewl window.
 */
int
ewl_ev_x_window_delete(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	/*
	 * Retrieve the appropriate ewl_window using the x window id that is
	 * held in the eevent, and call it's handlers for a window delete event.
	 */
	Ecore_X_Event_Window_Destroy *ev;
	Ewl_Window *window;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	window = ewl_window_window_find((void *)ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_callback_call(EWL_WIDGET(window), EWL_CALLBACK_DELETE_WINDOW);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the key down event information
 * @return Returns no value.
 * @brief Handles key down events in windows
 *
 * Dispatches the key down event to the appropriate ewl window.
 */
int
ewl_ev_x_key_down(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Window *window;
	Ecore_X_Event_Key_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;
	window = ewl_window_window_find((void *)ev->win);

	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	if (strstr(ev->keyname, "Shift_"))
		key_modifiers |= EWL_KEY_MODIFIER_SHIFT;
	else if (strstr(ev->keyname, "Control_"))
		key_modifiers |= EWL_KEY_MODIFIER_CTRL;
	else if (strstr(ev->keyname, "Alt_"))
		key_modifiers |= EWL_KEY_MODIFIER_ALT;
	else if (strstr(ev->keyname, "Mod_"))
		key_modifiers |= EWL_KEY_MODIFIER_MOD;
	else if (strstr(ev->keyname, "Super_"))
		key_modifiers |= EWL_KEY_MODIFIER_WIN;
	else if (strstr(ev->keyname, "Hyper_"))
		key_modifiers |= EWL_KEY_MODIFIER_WIN;
	else if (!ev->key_compose || iscntrl(*ev->key_compose))
		ewl_embed_key_down_feed(EWL_EMBED(window), ev->keyname, key_modifiers);
	else
		ewl_embed_key_down_feed(EWL_EMBED(window), ev->key_compose, key_modifiers);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the key up event information
 * @return Returns no value.
 * @brief Handles key up events in windows
 *
 * Dispatches the key up event to the appropriate ewl window.
 */
int
ewl_ev_x_key_up(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Window *window;
	Ecore_X_Event_Key_Up *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	window = ewl_window_window_find((void *)ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	if (strstr(ev->keyname, "Shift_"))
		key_modifiers &= ~EWL_KEY_MODIFIER_SHIFT;
	else if (strstr(ev->keyname, "Control_"))
		key_modifiers &= ~EWL_KEY_MODIFIER_CTRL;
	else if (strstr(ev->keyname, "Alt_"))
		key_modifiers &= ~EWL_KEY_MODIFIER_ALT;
	else if (strstr(ev->keyname, "Mod_"))
		key_modifiers &= ~EWL_KEY_MODIFIER_MOD;
	else if (strstr(ev->keyname, "Super_"))
		key_modifiers &= ~EWL_KEY_MODIFIER_WIN;
	else if (strstr(ev->keyname, "Hyper_"))
		key_modifiers &= ~EWL_KEY_MODIFIER_WIN;
	else if (!ev->key_compose || iscntrl(*ev->key_compose))
		ewl_embed_key_up_feed(EWL_EMBED(window), ev->keyname, key_modifiers);
	else
		ewl_embed_key_up_feed(EWL_EMBED(window), ev->key_compose, key_modifiers);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the mouse down event information
 * @return Returns no value.
 * @brief Handles mouse down events in windows
 *
 * Dispatches the mouse down event to the appropriate ewl window.
 * Also determines the widgets clicked state.
 */
int
ewl_ev_x_mouse_down(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	int clicks = 1;
	Ewl_Window *window;
	Ecore_X_Event_Mouse_Button_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	window = ewl_window_window_find((void *)ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	if (ev->double_click)
		clicks = 2;
	if (ev->triple_click)
		clicks = 3;

	ewl_embed_mouse_down_feed(EWL_EMBED(window), ev->button, clicks, ev->x, ev->y,
				  key_modifiers);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the mouse up event information
 * @return Returns no value.
 * @brief Handles mouse up events in windows
 *
 * Dispatches the mouse up event to the appropriate ewl window.
 * Also determines the widgets clicked state.
 */
int
ewl_ev_x_mouse_up(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Window *window;
	Ecore_X_Event_Mouse_Button_Up *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	window = ewl_window_window_find((void *)ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_embed_mouse_up_feed(EWL_EMBED(window), ev->button, ev->x, ev->y, key_modifiers);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}


/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the mouse move event information
 * @return Returns no value.
 * @brief Handles mouse move events in windows
 *
 * Dispatches the mouse move event to the appropriate ewl window.
 */
int
ewl_ev_x_mouse_move(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Window *window;
	Ecore_X_Event_Mouse_Move *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	window = ewl_window_window_find((void *)ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_embed_mouse_move_feed(EWL_EMBED(window), ev->x, ev->y, key_modifiers);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the mouse out event information
 * @return Returns no value.
 * @brief Handles the mouse out events in windows
 *
 * Dispatches the mouse out event to the appropriate ewl window.
 */
int
ewl_ev_x_mouse_out(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Window *window;
	Ecore_X_Event_Mouse_Out *ev = e;

	DENTER_FUNCTION(DLEVEL_STABLE);

	window = ewl_window_window_find((void *)ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_embed_mouse_out_feed(EWL_EMBED(window), ev->x, ev->y, key_modifiers);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the mouse wheel event information
 * @return Returns no value.
 * @brief Handles the mouse wheel events in windows
 *
 * Dispatches the mouse wheel event to the appropriate ewl window.
 */
int
ewl_ev_x_mouse_wheel(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Window *window;
	Ecore_X_Event_Mouse_Wheel *ev = e;

	DENTER_FUNCTION(DLEVEL_STABLE);

	window = ewl_window_window_find((void *)ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_embed_mouse_wheel_feed(EWL_EMBED(window), ev->x, ev->y, ev->z, ev->direction, key_modifiers);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the focus in event information
 * @return Returns no value.
 * @brief Handles the focus in events in windows
 *
 * Dispatches the focus in event to the appropriate ewl window.
 */
int
ewl_ev_x_focus_in(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Window *window;
	Ecore_X_Event_Window_Focus_In *ev = e;

	DENTER_FUNCTION(DLEVEL_STABLE);

	window = ewl_window_window_find((void *)ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_callback_call(EWL_WIDGET(window), EWL_CALLBACK_FOCUS_IN);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the focus out event information
 * @return Returns no value.
 * @brief Handles the focus out events in windows
 *
 * Dispatches the focus out event to the appropriate ewl window.
 */
int
ewl_ev_x_focus_out(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Window *window;
	Ecore_X_Event_Window_Focus_Out *ev = e;

	DENTER_FUNCTION(DLEVEL_STABLE);

	window = ewl_window_window_find((void *)ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_callback_call(EWL_WIDGET(window), EWL_CALLBACK_FOCUS_OUT);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the mouse out event information
 * @return Returns no value.
 * @brief Handles the data for a paste becoming available in windows
 *
 * Dispatches the mouse out event to the appropriate ewl window.
 */
int
ewl_ev_x_paste(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ecore_X_Event_Selection_Notify *ev;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, FALSE);

	ev = e;

	/* Handle everything *except* XDND selection */
	if (ev->selection != ECORE_X_SELECTION_XDND) 
		printf("Paste event received\n");

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the dnd position information
 * @return Returns TRUE on success or FALSE on failure.
 * @brief Handles the data for an XDND position event 
 *
 * Tells an XDND source if we can accept DND at this window location
 */
int
ewl_ev_dnd_position(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Window *window;
	Ecore_X_Event_Xdnd_Position *ev;
	int x, y, wx, wy;
	int px, py, pw, ph;
	Ecore_X_Rectangle rect;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, FALSE);

	ev = e;

	window = ewl_window_window_find((void *)ev->win);
	if (window) {
		Ewl_Embed *embed;
		
		ewl_window_position_get(EWL_WINDOW(window), &wx, &wy);
		x = ev->position.x - wx;
		y = ev->position.y - wy;

		/*
		 * Look for the child here
		 */
		embed = ewl_embed_evas_window_find((void *)ev->win);
		if (embed) {
			/* First see if we need to send an 'enter' to the widget */
			ewl_embed_dnd_position_feed(embed, x, y, &px, &py, &pw, &ph);
			
			/*rect.x = px;
			rect.y = py;
			rect.width = pw;
			rect.height = ph;*/

			rect.x = 0;
			rect.y = 0;
			rect.width = 0;
			rect.height = 0;				

			//printf("No more position until %d:%d:%d:%d\n", px,py,pw,ph);
		} else {
			rect.x = 0;
			rect.y = 0;
			rect.width = 0;
			rect.height = 0;	
		}
		ecore_x_dnd_send_status(1, 0, rect, ECORE_X_DND_ACTION_PRIVATE);
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the dnd 'enter' information
 * @return Returns TRUE on success or FALSE on failure.
 * @brief Handles the data for an XDND position event 
 *
 * Tells an XDND source if we can accept DND at this window location
 */
int
ewl_ev_dnd_enter(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Window *window;
	Ecore_X_Event_Xdnd_Enter *ev;
	int i = 0;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, FALSE);

	ev = e;

	window = ewl_window_window_find((void *)ev->win);
	if (window) {
		/*printf("Assigned enter types to window - '%s'\n", ev->types[0]);*/
		window->dnd_types.num_types = ev->num_types;
		window->dnd_types.types = malloc(sizeof(char*) * ev->num_types);
		
		for (i = 0; i < ev->num_types; i++) 
			window->dnd_types.types[i] = strdup(ev->types[i]);
	}
	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the dnd 'leave' information
 * @return Returns TRUE on success or FALSE on failure.
 * @brief Handles the data for an XDND position event 
 *
 * Tells an XDND source if we can accept DND at this window location
 */
int
ewl_ev_dnd_leave(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Window *window;
	Ecore_X_Event_Xdnd_Leave *ev;
	int i;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, FALSE);

	ev = e;

	window = ewl_window_window_find((void *)ev->win);
	if (window) {
		if (window->dnd_types.num_types > 0) {
			for (i = 0; i < window->dnd_types.num_types; i++)
				FREE(window->dnd_types.types[i]);

			FREE(window->dnd_types.types);
			window->dnd_types.types = NULL;
			window->dnd_types.num_types = 0;
			
		}
	}
	DRETURN_INT(TRUE, DLEVEL_STABLE);
}


/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the dnd 'drop' information
 * @return Returns TRUE on success or FALSE on failure.
 * @brief Handles the data for an XDND position event 
 *
 * A notification from a DND source that we have accepted a drop
 */
int
ewl_ev_dnd_drop(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Window *window;
	Ecore_X_Event_Xdnd_Drop *ev;
	int internal = 0;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, FALSE);

	ev = e;

	window = ewl_window_window_find((void *)ev->win);
	if (window) {
		int x,y,wx,wy;
		Ewl_Embed *embed= ewl_embed_evas_window_find((void *)ev->win);
		ewl_window_position_get(EWL_WINDOW(window), &wx, &wy);

		printf("Wx/y: %d:%d\n", wx,wy);
		
		/*printf("EWL got a DND drop event..\n");*/

		/* Request a DND data request */
		/* TODO this only supports retrieval of the first type in the request */
		if (window->dnd_types.num_types > 0)
			ecore_x_selection_xdnd_request(ev->win, 
				window->dnd_types.types[0]);


		printf("Drop!\n");


		if (ev->win == (Ecore_X_Window)window->window) {
			printf("Source is dest! - Retrieving local data\n");
			internal = 1;
		} else {
			printf("Source is not dest\n");
			//printf("%d:%d:%d\n", ev->source, ev->win,x_window);
			//ecore_x_selection_xdnd_request(ev->win, "text/uri-list");
		}

		x = ev->position.x - wx;
		y = ev->position.y - wy;
		
		ewl_embed_dnd_drop_feed(embed, x, y, internal);
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the dnd 'selection' information
 * @return Returns TRUE on success or FALSE on failure.
 * @brief Handles the data for an XDND selection notify event 
 *
 * A notification from a DND source that the selection data is ready
 */
int
ewl_ev_dnd_selection_notify(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Window *window;
	Ecore_X_Event_Selection_Notify *ev;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, FALSE);

	ev = e;

	if (ev->selection == ECORE_X_SELECTION_XDND) {
		/*printf("EWL DND selection notify\n");*/

		window = ewl_window_window_find((void *)ev->win);
		if (window) {
			/*printf(" ...Got a winow target for the DND selection event..\n");*/

			/*printf("Content: %d\n",ev->content);
			if (ev->content == ECORE_X_SELECTION_CONTENT_FILES) {
				Ecore_X_Selection_Data_Files* files = ev->data;
				printf("We've got some files! - '%s'\n", files->files[0]);
			} else {
				Ecore_X_Selection_Data_Files* files = ev->data;
				printf("We've got some files! - '%s'\n", files->files[0]);
			}*/
		}
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}


#endif

#ifdef ENABLE_EWL_FB

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the key down event information
 * @return Returns no value.
 * @brief Handles key down events in windows
 *
 * Dispatches the key down event to the appropriate ewl window.
 */
int
ewl_ev_fb_key_down(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Embed *embed;
	Ecore_Fb_Event_Key_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	embed = ecore_list_goto_first(ewl_embed_list);

	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_embed_key_down_feed(embed, ev->keyname, key_modifiers);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the key up event information
 * @return Returns no value.
 * @brief Handles key down events in windows
 *
 * Dispatches the key down event to the appropriate ewl window.
 */
int
ewl_ev_fb_key_up(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Embed *embed;
	Ecore_Fb_Event_Key_Up *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;
	embed = ecore_list_goto_first(ewl_embed_list);

	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_embed_key_down_feed(embed, ev->keyname, key_modifiers);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the mouse down event information
 * @return Returns no value.
 * @brief Handles mouse down events in windows
 *
 * Dispatches the mouse down event to the appropriate ewl window.
 * Also determines the widgets clicked state.
 */
int
ewl_ev_fb_mouse_down(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	int clicks = 1;
	Ewl_Embed *embed;
	Ecore_Fb_Event_Mouse_Button_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	embed = ecore_list_goto_first(ewl_embed_list);
	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	if (ev->double_click)
		clicks = 2;
	if (ev->triple_click)
		clicks = 3;

	/* Feed a mouse move, since they don't occur prior to mouse down on a
	 * touchscreen */
	ewl_embed_mouse_move_feed(embed, ev->x, ev->y, key_modifiers);

	ewl_embed_mouse_down_feed(embed, ev->button, clicks, ev->x, ev->y,
				  key_modifiers);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the mouse up event information
 * @return Returns no value.
 * @brief Handles mouse up events in windows
 *
 * Dispatches the mouse up event to the appropriate ewl window.
 * Also determines the widgets clicked state.
 */
int
ewl_ev_fb_mouse_up(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Embed *embed;
	Ecore_Fb_Event_Mouse_Button_Up *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	embed = ecore_list_goto_first(ewl_embed_list);
	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_embed_mouse_move_feed(embed, ev->x, ev->y, key_modifiers);
	ewl_embed_mouse_up_feed(embed, ev->button, ev->x, ev->y, key_modifiers);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the mouse move event information
 * @return Returns no value.
 * @brief Handles mouse move events in windows
 *
 * Dispatches the mouse move event to the appropriate ewl window.
 */
int
ewl_ev_fb_mouse_move(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Embed *embed;
	Ecore_Fb_Event_Mouse_Move *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	embed = ecore_list_goto_first(ewl_embed_list);
	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_embed_mouse_move_feed(embed, ev->x, ev->y, key_modifiers);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

#endif

