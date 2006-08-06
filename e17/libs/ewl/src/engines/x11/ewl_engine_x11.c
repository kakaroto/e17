#include <Ewl_Engine_X11.h>
#include <Ewl.h>
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

/*
 * In general all of the X event handlers should find their matching window
 * with ewl_window_window_find, and not ewl_embed_evas_window_find. If the
 * embed function is used, then we get duplicate events for apps that setup
 * their own handlers and embed EWL. The exception to this is selection events
 * such as copy/paste and DND. These events need to be handled for embedded
 * EWL, but have no equivalent in the Evas callback namespace.
 */
static int ewl_ev_x_window_expose(void *data, int type, void *_ev);
static int ewl_ev_x_window_configure(void *data, int type, void *_ev);
static int ewl_ev_x_window_delete(void *data, int type, void *_ev);
static int ewl_ev_x_key_down(void *data, int type, void *_ev);
static int ewl_ev_x_key_up(void *data, int type, void *_ev);
static int ewl_ev_x_mouse_down(void *data, int type, void *_ev);
static int ewl_ev_x_mouse_wheel(void *data, int type, void *_ev);
static int ewl_ev_x_mouse_up(void *data, int type, void *_ev);
static int ewl_ev_x_mouse_move(void *data, int type, void *_ev);
static int ewl_ev_x_mouse_out(void *data, int type, void *_ev);
static int ewl_ev_x_focus_in(void *data, int type, void *_ev);
static int ewl_ev_x_focus_out(void *data, int type, void *_ev);
static int ewl_ev_x_paste(void *data, int type, void *_ev);
static int ewl_ev_dnd_position(void *data, int type, void *_ev);
static int ewl_ev_dnd_enter(void *data, int type, void *_ev);
static int ewl_ev_dnd_leave(void *data, int type, void *_ev);
static int ewl_ev_dnd_drop(void *data, int type, void *_ev);

static Ecore_Event_Handler *ee_expose_handler = NULL;
static Ecore_Event_Handler *ee_configure_handler = NULL;
static Ecore_Event_Handler *ee_delete_request_handler = NULL;
static Ecore_Event_Handler *ee_key_down_handler = NULL;
static Ecore_Event_Handler *ee_key_up_handler = NULL;
static Ecore_Event_Handler *ee_dnd_position_handler = NULL;
static Ecore_Event_Handler *ee_dnd_enter_handler = NULL;
static Ecore_Event_Handler *ee_dnd_leave_handler = NULL;
static Ecore_Event_Handler *ee_dnd_drop_handler = NULL;
static Ecore_Event_Handler *ee_selection_notify_handler = NULL;
static Ecore_Event_Handler *ee_mouse_down_handler = NULL;
static Ecore_Event_Handler *ee_mouse_up_handler = NULL;
static Ecore_Event_Handler *ee_mouse_move_handler = NULL;
static Ecore_Event_Handler *ee_mouse_wheel_handler = NULL;
static Ecore_Event_Handler *ee_mouse_out_handler = NULL;
static Ecore_Event_Handler *ee_focus_in_handler = NULL;
static Ecore_Event_Handler *ee_focus_out_handler = NULL;

static void ee_shutdown(Ewl_Engine *engine);
static int ee_init(Ewl_Engine *engine);
static void ee_window_new(Ewl_Window *win);
static void ee_window_destroy(Ewl_Window *win);
static void ee_window_move(Ewl_Window *win);
static void ee_window_resize(Ewl_Window *win);
static void ee_window_min_max_size_set(Ewl_Window *win);
static void ee_window_show(Ewl_Window *win);
static void ee_window_hide(Ewl_Window *win);
static void ee_window_title_set(Ewl_Window *win);
static void ee_window_name_class_set(Ewl_Window *win);
static void ee_window_borderless_set(Ewl_Window *win);
static void ee_window_dialog_set(Ewl_Window *win);
static void ee_window_fullscreen_set(Ewl_Window *win);
static void ee_window_transient_for(Ewl_Window *win);
static void ee_window_modal_set(Ewl_Window *win);
static void ee_window_raise(Ewl_Window *win);
static void ee_window_lower(Ewl_Window *win);
static int ee_keyboard_grab(Ewl_Window *win);
static void ee_keyboard_ungrab(Ewl_Window *win);
static int ee_pointer_grab(Ewl_Window *win);
static void ee_pointer_ungrab(Ewl_Window *win);
static void ee_window_selection_text_set(Ewl_Window *win, const char *txt);
static void ee_window_geometry_set(Ewl_Window *win, int *width, int *height);
static void ee_dnd_aware_set(Ewl_Window *win);

static Ewl_Engine_Info engine_funcs = {
	{
		ee_init,
		ee_shutdown,
		ee_window_new,
		ee_window_destroy,
		ee_window_move,
		ee_window_resize,
		ee_window_min_max_size_set,
		ee_window_show,
		ee_window_hide,
		ee_window_title_set,
		ee_window_name_class_set,
		ee_window_borderless_set,
		ee_window_dialog_set,
		ee_window_fullscreen_set,
		ee_window_transient_for,
		ee_window_modal_set,
		ee_window_raise,
		ee_window_lower,
		ee_keyboard_grab,
		ee_keyboard_ungrab,
		ee_pointer_grab,
		ee_pointer_ungrab,
		ee_window_selection_text_set,
		ee_window_geometry_set,
		ee_dnd_aware_set,
		NULL	
	}
};

Ecore_DList *
ewl_engine_dependancies(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

Ewl_Engine *
ewl_engine_create(void)
{
	Ewl_Engine_X11 *engine;

	DENTER_FUNCTION(DLEVEL_STABLE);

	engine = NEW(Ewl_Engine_X11, 1);
	if (!engine)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ee_init(EWL_ENGINE(engine)))
	{
		FREE(engine);
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}
	
	DRETURN_PTR(EWL_ENGINE(engine), DLEVEL_STABLE);
}

static int
ee_init(Ewl_Engine *engine)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("engine", engine, FALSE);

	/* If the event handlers are already setup don't do it again */
	if (ee_expose_handler)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	if (!ecore_x_init(NULL))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ee_expose_handler = ecore_event_handler_add(
					ECORE_X_EVENT_WINDOW_DAMAGE,
					ewl_ev_x_window_expose, NULL);
	ee_configure_handler = ecore_event_handler_add(
					ECORE_X_EVENT_WINDOW_CONFIGURE,
					ewl_ev_x_window_configure, NULL);
	ee_delete_request_handler = ecore_event_handler_add(
					ECORE_X_EVENT_WINDOW_DELETE_REQUEST,
					ewl_ev_x_window_delete, NULL);

	/*
	 * Register dispatching functions for keyboard events.
	 */
	ee_key_down_handler = ecore_event_handler_add(
						ECORE_X_EVENT_KEY_DOWN,
						ewl_ev_x_key_down, NULL);
	ee_key_up_handler = ecore_event_handler_add(
						ECORE_X_EVENT_KEY_UP, 
						ewl_ev_x_key_up, NULL);

	/*
	 * Register dispatching functions for DND events
	 */
	ee_dnd_position_handler = ecore_event_handler_add(
						ECORE_X_EVENT_XDND_POSITION,
						ewl_ev_dnd_position, NULL);
	ee_dnd_enter_handler = ecore_event_handler_add(
						ECORE_X_EVENT_XDND_ENTER,
						ewl_ev_dnd_enter, NULL);
	ee_dnd_leave_handler = ecore_event_handler_add(
						ECORE_X_EVENT_XDND_LEAVE,
						ewl_ev_dnd_leave, NULL);
	ee_dnd_drop_handler = ecore_event_handler_add(
						ECORE_X_EVENT_XDND_DROP,
						ewl_ev_dnd_drop, NULL);

	/*
	 * Selection callbacks to allow for pasting.
	 */
	ee_selection_notify_handler = ecore_event_handler_add(
						ECORE_X_EVENT_SELECTION_NOTIFY,
						ewl_ev_x_paste, NULL);

	/*
	 * Finally, register dispatching functions for mouse events.
	 */
	ee_mouse_down_handler = ecore_event_handler_add(
						ECORE_X_EVENT_MOUSE_BUTTON_DOWN,
						ewl_ev_x_mouse_down, NULL);
	ee_mouse_up_handler = ecore_event_handler_add(
						ECORE_X_EVENT_MOUSE_BUTTON_UP,
						ewl_ev_x_mouse_up, NULL);
	ee_mouse_move_handler = ecore_event_handler_add(
						ECORE_X_EVENT_MOUSE_MOVE,
						ewl_ev_x_mouse_move, NULL);
	ee_mouse_wheel_handler = ecore_event_handler_add(
						ECORE_X_EVENT_MOUSE_WHEEL,
						ewl_ev_x_mouse_wheel, NULL);
	ee_mouse_out_handler = ecore_event_handler_add(
						ECORE_X_EVENT_MOUSE_OUT,
						ewl_ev_x_mouse_out, NULL);
	ee_focus_in_handler = ecore_event_handler_add(
						ECORE_X_EVENT_WINDOW_FOCUS_IN,
						ewl_ev_x_focus_in, NULL);
	ee_focus_out_handler = ecore_event_handler_add(
						ECORE_X_EVENT_WINDOW_FOCUS_OUT,
						ewl_ev_x_focus_out, NULL);

	if (!ee_expose_handler || !ee_configure_handler 
			|| !ee_delete_request_handler
			|| !ee_key_down_handler || !ee_key_up_handler 
			|| !ee_dnd_position_handler || !ee_dnd_enter_handler
			|| !ee_dnd_leave_handler || !ee_dnd_drop_handler
			|| !ee_selection_notify_handler || !ee_mouse_down_handler
			|| !ee_mouse_up_handler || !ee_mouse_move_handler
			|| !ee_mouse_wheel_handler || !ee_mouse_out_handler
			|| !ee_focus_in_handler || !ee_focus_out_handler)
	{
		ee_shutdown(EWL_ENGINE(engine));

		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	engine->name = strdup("x11");
	engine->functions = &engine_funcs;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static void
ee_shutdown(Ewl_Engine *engine)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("engine", engine);

	if (ee_expose_handler)
		ecore_event_handler_del(ee_expose_handler);
	ee_expose_handler = NULL;

	if (ee_configure_handler)
		ecore_event_handler_del(ee_configure_handler);
	ee_configure_handler = NULL;

	if (ee_delete_request_handler)
		ecore_event_handler_del(ee_delete_request_handler);
	ee_delete_request_handler = NULL;

	if (ee_key_down_handler)
		ecore_event_handler_del(ee_key_down_handler);
	ee_key_down_handler = NULL;

	if (ee_key_up_handler)
		ecore_event_handler_del(ee_key_up_handler);
	ee_key_up_handler = NULL;

	if (ee_dnd_position_handler)
		ecore_event_handler_del(ee_dnd_position_handler);
	ee_dnd_position_handler = NULL;

	if (ee_dnd_enter_handler)
		ecore_event_handler_del(ee_dnd_enter_handler);
	ee_dnd_enter_handler = NULL;

	if (ee_dnd_leave_handler)
		ecore_event_handler_del(ee_dnd_leave_handler);
	ee_dnd_leave_handler = NULL;

	if (ee_dnd_drop_handler)
		ecore_event_handler_del(ee_dnd_drop_handler);
	ee_dnd_drop_handler = NULL;

	if (ee_selection_notify_handler)
		ecore_event_handler_del(ee_selection_notify_handler);
	ee_selection_notify_handler = NULL;

	if (ee_mouse_down_handler)
		ecore_event_handler_del(ee_mouse_down_handler);
	ee_mouse_down_handler = NULL;

	if (ee_mouse_up_handler)
		ecore_event_handler_del(ee_mouse_up_handler);
	ee_mouse_up_handler = NULL;

	if (ee_mouse_move_handler)
		ecore_event_handler_del(ee_mouse_move_handler);
	ee_mouse_move_handler = NULL;

	if (ee_mouse_wheel_handler)
		ecore_event_handler_del(ee_mouse_wheel_handler);
	ee_mouse_wheel_handler = NULL;

	if (ee_mouse_out_handler)
		ecore_event_handler_del(ee_mouse_out_handler);
	ee_mouse_out_handler = NULL;

	if (ee_focus_in_handler)
		ecore_event_handler_del(ee_focus_in_handler);
	ee_focus_in_handler = NULL;

	if (ee_focus_out_handler)
		ecore_event_handler_del(ee_focus_out_handler);
	ee_focus_out_handler = NULL;

	ecore_x_shutdown();
	engine->functions = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_new(Ewl_Window *win)
{
	Ecore_X_Window xwin;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (win->flags & EWL_WINDOW_OVERRIDE)
		xwin = ecore_x_window_override_new(0,
				EWL_EMBED(win)->x, EWL_EMBED(win)->y,
				ewl_object_current_w_get(EWL_OBJECT(win)),
				ewl_object_current_h_get(EWL_OBJECT(win)));
	else
		xwin = ecore_x_window_new(0,
				EWL_EMBED(win)->x, EWL_EMBED(win)->y,
				ewl_object_current_w_get(EWL_OBJECT(win)),
				ewl_object_current_h_get(EWL_OBJECT(win)));

	ecore_x_icccm_protocol_set(xwin, ECORE_X_WM_PROTOCOL_DELETE_REQUEST, 1);
	win->window = (void *)xwin;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_destroy(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	ee_window_hide(win);
	ecore_x_window_del((Ecore_X_Window)EWL_EMBED(win)->evas_window);
	ecore_x_window_del((Ecore_X_Window)EWL_WINDOW(win)->window);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_move(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	ecore_x_window_move((Ecore_X_Window)win->window, 
					EWL_EMBED(win)->x, 
					EWL_EMBED(win)->y);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_resize(Ewl_Window *win)
{
	int width, height;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	width = ewl_object_current_w_get(EWL_OBJECT(win));
	height = ewl_object_current_h_get(EWL_OBJECT(win));

	ecore_x_window_resize((Ecore_X_Window)win->window, width, height);

	if (EWL_EMBED(win)->evas_window != win->window)
		ecore_x_window_resize((Ecore_X_Window)EWL_EMBED(win)->evas_window,
						width, height);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_min_max_size_set(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	ecore_x_icccm_size_pos_hints_set((Ecore_X_Window)win->window,
					0, ECORE_X_GRAVITY_NW,
					ewl_object_minimum_w_get(EWL_OBJECT(win)),
					ewl_object_minimum_h_get(EWL_OBJECT(win)),
					ewl_object_maximum_w_get(EWL_OBJECT(win)),
					ewl_object_maximum_h_get(EWL_OBJECT(win)),
					0, 0, /* base */
					0, 0, /* step */
					0, 0); /* aspect */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_show(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	ecore_x_window_show((Ecore_X_Window)win->window);
	ecore_x_window_show((Ecore_X_Window)EWL_EMBED(win)->evas_window);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_hide(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	ecore_x_window_hide((Ecore_X_Window)EWL_EMBED(win)->evas_window);
	ecore_x_window_hide((Ecore_X_Window)EWL_WINDOW(win)->window);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_title_set(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	ecore_x_icccm_title_set((Ecore_X_Window)win->window, win->title);
	ecore_x_netwm_name_set((Ecore_X_Window)win->window, win->title);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_name_class_set(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	ecore_x_icccm_name_class_set((Ecore_X_Window)win->window, win->name,
				(win->classname ? win->classname : win->name));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_borderless_set(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	ecore_x_mwm_borderless_set((Ecore_X_Window)win->window, 
				(!!(win->flags & EWL_WINDOW_BORDERLESS)));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_dialog_set(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	ecore_x_netwm_window_type_set((Ecore_X_Window)win->window,
			((!!(win->flags & EWL_WINDOW_DIALOG)) ? 
					ECORE_X_WINDOW_TYPE_DIALOG :
					ECORE_X_WINDOW_TYPE_NORMAL));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_fullscreen_set(Ewl_Window *win)
{
	int fullscreen;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE); 

	fullscreen = (!!(win->flags & EWL_WINDOW_FULLSCREEN));

	if (REALIZED(win))
		ecore_x_netwm_state_request_send((Ecore_X_Window)win->window,
				0, ECORE_X_WINDOW_STATE_FULLSCREEN,
				ECORE_X_WINDOW_STATE_UNKNOWN, fullscreen);

	else if (win->window && fullscreen)
	{
		Ecore_X_Window_State states[] =
				{ECORE_X_WINDOW_STATE_FULLSCREEN};

		ecore_x_netwm_window_state_set((Ecore_X_Window)win->window,
								states, 1);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_transient_for(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (win->transient)
		ecore_x_icccm_transient_for_set((Ecore_X_Window)win->window,
					(Ecore_X_Window)win->transient->window);
	else
		ecore_x_icccm_transient_for_unset((Ecore_X_Window)win->window);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_modal_set(Ewl_Window *win)
{
	int modal;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE); 

	modal = (!!(win->flags & EWL_WINDOW_MODAL));

	if (REALIZED(win))
		ecore_x_netwm_state_request_send((Ecore_X_Window)win->window,
					0, ECORE_X_WINDOW_STATE_MODAL,
					ECORE_X_WINDOW_STATE_UNKNOWN, modal);

	else if (win->window && modal)
	{
		Ecore_X_Window_State states[] =
				{ECORE_X_WINDOW_STATE_MODAL};

		ecore_x_netwm_window_state_set((Ecore_X_Window)win->window,
								states, 1);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_raise(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	ecore_x_window_raise((Ecore_X_Window)win->window);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_lower(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	ecore_x_window_lower((Ecore_X_Window)win->window);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ee_keyboard_grab(Ewl_Window *win)
{
	int ret = FALSE;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("win", win, FALSE);
	DCHECK_TYPE_RET("win", win, EWL_WINDOW_TYPE, FALSE);

	if ((!!(win->flags & EWL_WINDOW_GRAB_KEYBOARD)))
		ret = ecore_x_keyboard_grab((Ecore_X_Window)win->window);
	else
		ecore_x_keyboard_ungrab();

	DRETURN_INT(ret, DLEVEL_STABLE);
}

static void
ee_keyboard_ungrab(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	ecore_x_keyboard_ungrab();
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ee_pointer_grab(Ewl_Window *win)
{
	int ret = FALSE;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("win", win, FALSE);
	DCHECK_TYPE_RET("win", win, EWL_WINDOW_TYPE, FALSE);

	if ((!!(win->flags & EWL_WINDOW_GRAB_POINTER)))
		ret = ecore_x_pointer_grab((Ecore_X_Window)win->window);
	else
		ecore_x_pointer_ungrab();

	DRETURN_INT(ret, DLEVEL_STABLE);
}

static void
ee_pointer_ungrab(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	ecore_x_pointer_ungrab();
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_selection_text_set(Ewl_Window *win, const char *txt)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_PARAM_PTR("txt", txt);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (txt)
		ecore_x_selection_primary_set(
				(Ecore_X_Window)win->window,
				(unsigned char *)txt, strlen(txt) + 1);
	else
		ecore_x_selection_primary_clear();

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_geometry_set(Ewl_Window *win, int *width, int *height)
{
	int w, h;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ecore_x_window_geometry_get((win ? (Ecore_X_Window)win->window : (Ecore_X_Window)0),
							NULL, NULL, &w, &h);
	if (width) *width = w;
	if (height) *height = w;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_dnd_aware_set(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	ecore_x_dnd_aware_set((Ecore_X_Window)win->window,
			(!!(win->flags & EWL_FLAG_PROPERTY_DND_AWARE)));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
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

static int
ewl_ev_x_window_configure(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	/*
	 * When a configure event occurs, we must update the windows geometry
	 * based on the coordinates and dimensions given in the Ecore_Event.
	 */
	Ecore_X_Event_Window_Configure *ev;
	Ewl_Window *window;
	Ewl_Embed *embed;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	embed = ewl_embed_evas_window_find((void *)ev->win);
	if (!embed)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	/*
	 * Save coords and queue a configure event if the window is moved.
	 */
	if ((ev->from_wm) && (ev->x != embed->x))
		embed->x = ev->x;

	if ((ev->from_wm) && (ev->y != embed->y))
		embed->y = ev->y;

	window = ewl_window_window_find((void *)ev->win);
	/*
	 * we can finish when the embed is not a window
	 */
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_widget_configure(EWL_WIDGET(window));

	/*
	 * Configure events really only need to occur on resize.
	 */
	if ((ewl_object_current_w_get(EWL_OBJECT(window)) != ev->w)
			|| (ewl_object_current_h_get(EWL_OBJECT(window)) != ev->h)) {

		/* 
		 * Right now this is commented out, as it was preventing windows 
		 * from resizing when WE were the ones setting the size 
		 * (mainly on initial show of the window). It would be nice to 
		 * add this back in to limit the # of window resizes occuring. 
		 * (As long as it doesn't break initial size.)
		 */
		/* if (ev->from_wm) window->flags |= EWL_WINDOW_USER_CONFIGURE; */
		ewl_object_geometry_request(EWL_OBJECT(window), 0, 0, ev->w,
									ev->h);
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
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

static int
ewl_ev_x_key_down(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Window *window;
	Ecore_X_Event_Key_Down *ev;
	unsigned int key_modifiers;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;
	window = ewl_window_window_find((void *)ev->win);

	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	key_modifiers = ewl_ev_modifiers_get();

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
		ewl_embed_key_down_feed(EWL_EMBED(window), ev->keyname,
							key_modifiers);
	else
		ewl_embed_key_down_feed(EWL_EMBED(window), ev->key_compose,
							key_modifiers);

	ewl_ev_modifiers_set(key_modifiers);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_x_key_up(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Window *window;
	Ecore_X_Event_Key_Up *ev;
	unsigned int key_modifiers;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	window = ewl_window_window_find((void *)ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	key_modifiers = ewl_ev_modifiers_get();

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
		ewl_embed_key_up_feed(EWL_EMBED(window), ev->keyname,
							key_modifiers);
	else
		ewl_embed_key_up_feed(EWL_EMBED(window), ev->key_compose,
							key_modifiers);

	ewl_ev_modifiers_set(key_modifiers);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_x_mouse_down(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	int clicks = 1;
	Ewl_Window *window;
	Ecore_X_Event_Mouse_Button_Down *ev;
	unsigned int key_modifiers;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	window = ewl_window_window_find((void *)ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	if (ev->double_click)
		clicks = 2;
	if (ev->triple_click)
		clicks = 3;

	key_modifiers = ewl_ev_modifiers_get();
	ewl_embed_mouse_down_feed(EWL_EMBED(window), ev->button, clicks,
						ev->x, ev->y, key_modifiers);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_x_mouse_up(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Window *window;
	Ecore_X_Event_Mouse_Button_Up *ev;
	unsigned int key_modifiers;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	window = ewl_window_window_find((void *)ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	key_modifiers = ewl_ev_modifiers_get();

	ewl_embed_mouse_up_feed(EWL_EMBED(window), ev->button, ev->x,
						ev->y, key_modifiers);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_x_mouse_move(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Window *window;
	Ecore_X_Event_Mouse_Move *ev;
	unsigned int key_modifiers;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = e;

	window = ewl_window_window_find((void *)ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	key_modifiers = ewl_ev_modifiers_get();
	ewl_embed_mouse_move_feed(EWL_EMBED(window), ev->x, ev->y,
							key_modifiers);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_x_mouse_out(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Window *window;
	Ecore_X_Event_Mouse_Out *ev = e;
	unsigned int key_modifiers;

	DENTER_FUNCTION(DLEVEL_STABLE);

	window = ewl_window_window_find((void *)ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	key_modifiers = ewl_ev_modifiers_get();
	ewl_embed_mouse_out_feed(EWL_EMBED(window), ev->x, ev->y,
						key_modifiers);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_x_mouse_wheel(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ewl_Window *window;
	Ecore_X_Event_Mouse_Wheel *ev = e;
	unsigned int key_modifiers;

	DENTER_FUNCTION(DLEVEL_STABLE);

	window = ewl_window_window_find((void *)ev->win);
	if (!window)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	key_modifiers = ewl_ev_modifiers_get();
	ewl_embed_mouse_wheel_feed(EWL_EMBED(window), ev->x, ev->y,
					ev->z, ev->direction, key_modifiers);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}


static int
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

static int
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

static int
ewl_ev_x_paste(void *data __UNUSED__, int type __UNUSED__, void *e)
{
	Ecore_X_Event_Selection_Notify *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, FALSE);

	ev = e;

	/* Handle everything *except* XDND selection */
	if (ev->selection != ECORE_X_SELECTION_XDND)
		printf("Paste event received\n");

	else if (ev->selection == ECORE_X_SELECTION_XDND)
	{
		Ewl_Window *window;
		Ecore_X_Selection_Data *data = ev->data;

                window = ewl_window_window_find((void *)ev->win);
		if (window) 
		{
			if (data->content == ECORE_X_SELECTION_CONTENT_FILES) 
			{
				Ecore_X_Selection_Data_Files* files = ev->data;
				printf("We've got some files! - '%s'\n", files->files[0]);
			} 
			else if (data->content == ECORE_X_SELECTION_CONTENT_TEXT)
			{
				Ecore_X_Selection_Data_Text* text = ev->data;
				printf("We've got some text! - '%s'\n", text->text);
			} 
			else 
			{
				Ecore_X_Selection_Data *data = ev->data;
				printf("\nUnknown DND selection received, type: %d target: %s\n", data->content, ev->target);
				printf("\tData length: %d\n", data->length);
			}
		}

	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
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

		ewl_embed_window_position_get(EWL_EMBED(window), &wx, &wy);
		x = ev->position.x - wx;
		y = ev->position.y - wy;

		/*
		 * Look for the child here
		 */
		embed = ewl_embed_evas_window_find((void *)ev->win);
		if (embed) {
			/* First see if we need to send an 'enter' 
			 * to the widget */
			ewl_embed_dnd_position_feed(embed, x, y, &px,
							&py, &pw, &ph);

			/*rect.x = px;
			rect.y = py;
			rect.width = pw;
			rect.height = ph;*/

			rect.x = 0;
			rect.y = 0;
			rect.width = 0;
			rect.height = 0;
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

static int
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
		window->dnd_types.num_types = ev->num_types;
		window->dnd_types.types = malloc(sizeof(char*) * ev->num_types);

		for (i = 0; i < ev->num_types; i++)
			window->dnd_types.types[i] = strdup(ev->types[i]);
	}
	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
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

static int
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
		int i;
		int x,y,wx,wy;
		Ewl_Embed *embed= ewl_embed_evas_window_find((void *)ev->win);
		ewl_embed_window_position_get(EWL_EMBED(window), &wx, &wy);

		/* Request a DND data request */
		for (i = 0; i < window->dnd_types.num_types; i++) {
			/* FIXME: This function does not exist. */
			if (ewl_dnd_type_supported(window->dnd_types.types[i])) {
				ecore_x_selection_xdnd_request(ev->win,
					window->dnd_types.types[i]);
				break;
			}
		}
		if (i == window->dnd_types.num_types)
			printf("No matching type found\n");

		printf("Drop!\n");

		if (ev->source == (Ecore_X_Window)window->window) {
			printf("Source is dest! - Retrieving local data\n");
			internal = 1;
		} else {
			printf("Source is not dest\n");
		}

		x = ev->position.x - wx;
		y = ev->position.y - wy;

		ewl_embed_dnd_drop_feed(embed, x, y, internal);
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}


