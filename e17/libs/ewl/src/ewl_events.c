#include <Ewl.h>

#ifdef HAVE_CONFIG_H
#include "ewl-config.h"
#endif

#ifdef HAVE_EVAS_ENGINE_FB_H
#include "Ecore_Fb.h"
#endif

#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
#include "Ecore_X.h"
#endif

unsigned int key_modifiers = 0;

extern Ewl_Widget     *last_selected;
extern Ewl_Widget     *last_key;
extern Ewl_Widget     *last_focused;
extern Ewl_Widget     *dnd_widget;

extern Ewd_List       *embed_list;;

#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
int ewl_ev_x_window_expose(void *data, int type, void *_ev);
int ewl_ev_x_window_configure(void *data, int type, void *_ev);
int ewl_ev_x_window_delete(void *data, int type, void *_ev);

int ewl_ev_x_key_down(void *data, int type, void *_ev);
int ewl_ev_x_key_up(void *data, int type, void *_ev);
int ewl_ev_x_mouse_down(void *data, int type, void *_ev);
int ewl_ev_x_mouse_up(void *data, int type, void *_ev);
int ewl_ev_x_mouse_move(void *data, int type, void *_ev);
int ewl_ev_x_mouse_out(void *data, int type, void *_ev);
int ewl_ev_x_paste(void *data, int type, void *_ev);
#endif

#ifdef HAVE_EVAS_ENGINE_FB
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
int ewl_ev_init(void)
{
	unsigned int engine;
	DENTER_FUNCTION(DLEVEL_STABLE);

	engine = ewl_get_engine_mask();
#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
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
		 * Finally, register dispatching functions for mouse events.
		 */
		ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_DOWN,
					ewl_ev_x_mouse_down, NULL);
		ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_UP,
					ewl_ev_x_mouse_up, NULL);
		ecore_event_handler_add(ECORE_X_EVENT_MOUSE_MOVE,
					ewl_ev_x_mouse_move, NULL);
		ecore_event_handler_add(ECORE_X_EVENT_MOUSE_OUT,
					ewl_ev_x_mouse_out, NULL);

		/*
		 * Selection callbacks to allow for pasting.
		 */
		ecore_event_handler_add(ECORE_X_EVENT_SELECTION_NOTIFY,
					ewl_ev_x_paste, NULL);
	}
#endif

#ifdef HAVE_EVAS_ENGINE_FB
	if (engine & EWL_ENGINE_FB) {
		ecore_event_handler_add(ECORE_FB_EVENT_KEY_DOWN,
					ewl_ev_fb_key_down, NULL);
		ecore_event_handler_add(ECORE_FB_EVENT_KEY_UP, ewl_ev_fb_key_up,
					NULL);
		ecore_event_handler_add(ECORE_FB_EVENT_MOUSE_DOWN,
					ewl_ev_fb_mouse_down, NULL);
		ecore_event_handler_add(ECORE_FB_EVENT_MOUSE_UP,
					ewl_ev_fb_mouse_up, NULL);
		ecore_event_handler_add(ECORE_FB_EVENT_MOUSE_MOVE,
					ewl_ev_fb_mouse_move, NULL);
	}
#endif

	DRETURN_INT(1, DLEVEL_STABLE);
}

/**
 * @return Returns the current mask of modifier keys.
 * @brief Retrieve the current mask of modifiers that are set.
 */
unsigned int ewl_ev_get_modifiers()
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DRETURN_INT(key_modifiers, DLEVEL_STABLE);
}

#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the expose event information
 * @return Returns no value.
 * @brief Handles the exposing of a window
 *
 * Dispatches the expose event to the appropriate window for handling.
 */
int ewl_ev_x_window_expose(void *data, int type, void * e)
{
	/*
	 * Widgets don't need to know about this usually, but we still need to
	 * let them know in case a widget is using a non-evas based draw method
	 */
	Ecore_X_Event_Window_Damage *ev;
	Ewl_Embed      *embed;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	embed = ewl_embed_find_by_evas_window((void *)ev->win);
	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	evas_damage_rectangle_add(embed->evas, ev->x, ev->y, ev->w, ev->h);
	ewl_callback_call(EWL_WIDGET(embed), EWL_CALLBACK_EXPOSE);

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
int ewl_ev_x_window_configure(void *data, int type, void *e)
{
	/*
	 * When a configure event occurs, we must update the windows geometry
	 * based on the coordinates and dimensions given in the Ecore_Event.
	 */
	Ecore_X_Event_Window_Configure *ev;
	Ewl_Window     *window;

	DENTER_FUNCTION(DLEVEL_STABLE);
	ev = e;

	window = ewl_window_find_window((void *)ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	window->x = ev->x;
	window->y = ev->y;

	/*
	 * Configure events really only need to occur on resize.
	 */
	if (CURRENT_W(window) != ev->w || CURRENT_H(window) != ev->h) {
		window->flags |= EWL_WINDOW_USER_CONFIGURE;
		ewl_object_request_geometry(EWL_OBJECT(window), 0, 0, ev->w,
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
int ewl_ev_x_window_delete(void *data, int type, void *e)
{
	/*
	 * Retrieve the appropriate ewl_window using the x window id that is
	 * held in the eevent, and call it's handlers for a window delete event.
	 */
	Ecore_X_Event_Window_Destroy *ev;
	Ewl_Window     *window;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	window = ewl_window_find_window((void *)ev->win);
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
int ewl_ev_x_key_down(void *data, int type, void *e)
{
	Ewl_Embed      *embed;
	Ecore_X_Event_Key_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	embed = ewl_embed_find_by_evas_window((void *)ev->win);

	if (!embed)
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
	else if (strlen(ev->keyname) > 1 || !ev->key_compose)
		ewl_embed_feed_key_down(embed, ev->keyname, key_modifiers);
	else
		ewl_embed_feed_key_down(embed, ev->key_compose, key_modifiers);

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
int ewl_ev_x_key_up(void *data, int type, void *e)
{
	Ewl_Embed      *embed;
	Ecore_X_Event_Key_Up *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	embed = ewl_embed_find_by_evas_window((void *)ev->win);
	if (!embed)
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
	else if (strlen(ev->keyname) > 1 || !ev->key_compose)
		ewl_embed_feed_key_up(embed, ev->keyname, key_modifiers);
	else
		ewl_embed_feed_key_up(embed, ev->key_compose, key_modifiers);

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
int ewl_ev_x_mouse_down(void *data, int type, void *e)
{
	Ewl_Embed      *embed;
	Ecore_X_Event_Mouse_Button_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	embed = ewl_embed_find_by_evas_window((void *)ev->win);
	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_embed_feed_mouse_down(embed, ev->button, ev->x, ev->y,
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
int ewl_ev_x_mouse_up(void *data, int type, void *e)
{
	Ewl_Embed      *embed;
	Ecore_X_Event_Mouse_Button_Up *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	embed = ewl_embed_find_by_evas_window((void *)ev->win);
	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_embed_feed_mouse_up(embed, ev->button, ev->x, ev->y, key_modifiers);

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
int ewl_ev_x_mouse_move(void *data, int type, void *e)
{
	Ewl_Embed      *embed;
	Ecore_X_Event_Mouse_Move *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	embed = ewl_embed_find_by_evas_window((void *)ev->win);
	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_embed_feed_mouse_move(embed, ev->x, ev->y, key_modifiers);

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
int ewl_ev_x_mouse_out(void *data, int type, void *e)
{
	Ewl_Embed      *embed;
	Ecore_X_Event_Mouse_Out *ev = e;

	DENTER_FUNCTION(DLEVEL_STABLE);

	embed = ewl_embed_find_by_evas_window((void *)ev->win);
	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_embed_feed_mouse_out(embed, ev->x, ev->y, key_modifiers);

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
int ewl_ev_x_paste(void *data, int type, void *e)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	printf("Paste event received\n");

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

#endif

#ifdef HAVE_EVAS_ENGINE_FB

/**
 * @param data: user specified data passed to the function
 * @param type: the type of event triggering the function call
 * @param e: the key down event information
 * @return Returns no value.
 * @brief Handles key down events in windows
 *
 * Dispatches the key down event to the appropriate ewl window.
 */
int ewl_ev_fb_key_down(void *data, int type, void *e)
{
	Ewl_Widget     *temp;
	Ewl_Embed      *embed;
	Ecore_Fb_Event_Key_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	embed = ewd_list_goto_first(embed_list);

	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_ev_feed_key_down(embed, ev->keyname);

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
int ewl_ev_fb_key_up(void *data, int type, void *e)
{
	Ewl_Widget     *temp;
	Ewl_Embed      *embed;
	Ecore_Fb_Event_Key_Up *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	embed = ewd_list_goto_first(embed_list);

	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_ev_feed_key_down(embed, ev->keyname);

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
int ewl_ev_fb_mouse_down(void *data, int type, void *e)
{
	Ewl_Embed      *embed;
	Ecore_Fb_Event_Mouse_Button_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	embed = ewl_embed_find_by_evas_window(ev->win);
	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_embed_feed_mouse_down(embed, ev->button, ev->x, ev->y);

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
int ewl_ev_fb_mouse_move(void *data, int type, void *e)
{
	Ewl_Widget     *widget;
	Ewl_Embed      *embed;
	Ecore_Fb_Event_Mouse_Move *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	embed = ewl_embed_find_by_evas_window(ev->win);
	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_embed_feed_mouse_move(embed, ev->x, ev->y);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

#endif
