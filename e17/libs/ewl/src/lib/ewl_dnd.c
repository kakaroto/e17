#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include <Ecore_Evas.h>
#include <Ecore_X.h>

#define EWL_DND_WINDOW_ROOT 0

static int ewl_dragging_current = 0;
static int ewl_dnd_move_count = 0;
static Ecore_Evas *ewl_dnd_drag_canvas;
static Evas *ewl_dnd_drag_evas;
static Evas_Object *ewl_dnd_drag_image;
static Ecore_X_Window ewl_dnd_evas_win;
static Ecore_X_Window ewl_dnd_drag_win = 0;

static Ewl_Widget *ewl_dnd_widget = NULL;

static Ecore_Hash *ewl_dnd_position_hash;
static int ewl_dnd_status = 0;

Ecore_Event_Handler *ewl_dnd_mouse_up_handler;
Ecore_Event_Handler *ewl_dnd_mouse_move_handler;

char *ewl_dnd_drop_types[] = { "text/uri-list" };

static int ewl_dnd_event_mouse_up(void *data, int type, void *event);
static int ewl_dnd_event_dnd_move(void *data, int type, void *event);

/**
 * @return Returns TRUE if the DND system was successfully initialized,
 * FALSE otherwise
 * @brief Initialize the DND sybsystem
 */
int
ewl_dnd_init(void) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_dnd_position_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
	if (!ewl_dnd_position_hash)
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_dragging_current = 0;
	ewl_dnd_status = 1;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @return Returns no value.
 * @brief Shuts down the EWL DND system
 */
void
ewl_dnd_shutdown(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ecore_hash_destroy(ewl_dnd_position_hash);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to add
 * @return Returns no value
 * @brief: Adds the given widget @a w to the position hash
 */
void
ewl_dnd_position_windows_set(Ewl_Widget *w) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ecore_hash_set(ewl_dnd_position_hash, w, (void *)1);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param widget: The widget to get the types for
 * @return Returns the Ewl_Dnd_Types for the given widget
 * @brief Get the Ewl_Dnd_Types for the given widget
 */
Ewl_Dnd_Types *
ewl_dnd_types_for_widget_get(Ewl_Widget *widget)
{
	Ewl_Widget *parent = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("widget", widget, NULL);
	DCHECK_TYPE_RET("widget", widget, EWL_WIDGET_TYPE, NULL);
	
	/* We need to get the top-level window widget.  Note
	 * that we assume here that a widget is
	 * a) Parented, and
	 * b) It's top-level parent is a window */
	parent = widget->parent;
	while (parent && parent->parent)
		parent = parent->parent;

	/* Now check if this obj we found is a window */
	if (parent && ewl_widget_type_is(parent, "window")) 
		DRETURN_PTR(&(EWL_WINDOW(parent)->dnd_types), DLEVEL_STABLE);

	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * @param w: The widget to start dragging
 * @return Returns no value
 * @brief Tells the widget to start dragging
 */
void
ewl_drag_start(Ewl_Widget *w) 
{
	int width, height;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (!ewl_dnd_status || ewl_dragging_current) 
		DRETURN(DLEVEL_STABLE);

	ewl_dragging_current = 1;
	ewl_dnd_widget = w;
	ewl_dnd_move_count = 0;
	
	ewl_dnd_mouse_up_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_UP,
							ewl_dnd_event_mouse_up, NULL);
	ewl_dnd_mouse_move_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_MOVE, 
							ewl_dnd_event_dnd_move, NULL);

	ewl_dnd_drag_canvas = ecore_evas_software_x11_new(NULL, 
							EWL_DND_WINDOW_ROOT, 
							64, 64, 64, 64); 
	ewl_dnd_drag_evas = ecore_evas_get(ewl_dnd_drag_canvas);

	ecore_evas_shaped_set(ewl_dnd_drag_canvas, 1);
	ecore_evas_software_x11_direct_resize_set(ewl_dnd_drag_canvas, 1);

	ewl_dnd_evas_win = ecore_evas_software_x11_window_get(ewl_dnd_drag_canvas);
	ecore_x_window_resize(ewl_dnd_evas_win, 64, 64);
	ecore_evas_override_set(ewl_dnd_drag_canvas, 1);
	
	/* ecore_evas_software_x11_direct_resize_set(ewl_dnd_drag_evas, 1); */
	ecore_evas_ignore_events_set(ewl_dnd_drag_canvas, 1);

	/* XXX Setup a cursor (This needs to become generic) */
	ewl_dnd_drag_image = evas_object_image_add(ewl_dnd_drag_evas);
	evas_object_image_file_set(ewl_dnd_drag_image, PACKAGE_DATA_DIR 
						"/images/World.png", 0);
 	evas_object_image_fill_set(ewl_dnd_drag_image, 0, 0, 50, 50);
	evas_object_resize(ewl_dnd_drag_image, 50, 50);
	evas_object_show(ewl_dnd_drag_image);

	/* Setup the dnd event capture window */
	ecore_x_window_geometry_get(EWL_DND_WINDOW_ROOT, NULL, NULL, 
							&width,  &height);
	ewl_dnd_drag_win = ecore_x_window_input_new(EWL_DND_WINDOW_ROOT, 0, 0,
								width, height);

	/* Finally show the drag window */
	ecore_x_window_show(ewl_dnd_drag_win);

	/* Confine the pointer to our event windows */	
	ecore_x_pointer_confine_grab(ewl_dnd_drag_win);
	ecore_x_keyboard_grab(ewl_dnd_drag_win);

	ecore_x_dnd_aware_set(ewl_dnd_drag_win, 1);
	ecore_x_dnd_aware_set(ewl_dnd_evas_win, 1);
	ecore_x_mwm_borderless_set(ewl_dnd_evas_win, 1);
	
	/* Start the drag operation */
	ecore_x_dnd_types_set(ewl_dnd_drag_win, ewl_dnd_drop_types, 1);
	ecore_x_dnd_begin(ewl_dnd_drag_win, NULL, 0);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns no value
 * @brief Disables DND
 */
void
ewl_dnd_disable(void) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_dnd_status = 0;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns no value
 * @brief Enables DND
 */
void
ewl_dnd_enable(void) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_dnd_status = 1;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns the current DND status
 * @brief Retrieves the current DND status
 */
int
ewl_dnd_status_get(void) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_INT(ewl_dnd_status, DLEVEL_STABLE);
}

/**
 * @return Returns the current DND widget
 * @brief Retrieves the current DND widget
 */
Ewl_Widget *
ewl_dnd_drag_widget_get(void) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_PTR(ewl_dnd_widget, DLEVEL_STABLE);
}

/**
 * @return Returns no value.
 * @brief Clears the current DND widget
 */
void
ewl_dnd_drag_widget_clear(void) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_dnd_widget = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_dnd_event_dnd_move(void *data __UNUSED__, int type __UNUSED__, 
							void *event)
{
	Ecore_X_Event_Mouse_Move *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("event", event, FALSE);

	ev = event;

	if (!ewl_dnd_status) DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_dnd_move_count++;
	if (ewl_dnd_move_count == 1) 
		ecore_evas_show(ewl_dnd_drag_canvas);

	if (ewl_dnd_drag_canvas) 
		ecore_evas_move(ewl_dnd_drag_canvas, ev->x - 15, ev->y - 15);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_dnd_event_mouse_up(void *data __UNUSED__, int type __UNUSED__, 
						void *event __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (ewl_dnd_drag_canvas && ewl_dragging_current) {
		Ecore_List *pos;
		void *val;

		ecore_x_pointer_ungrab();
		ecore_x_keyboard_ungrab();

		ecore_event_handler_del(ewl_dnd_mouse_up_handler);
		ecore_event_handler_del(ewl_dnd_mouse_move_handler);
	
		ecore_evas_free(ewl_dnd_drag_canvas);
		ewl_dnd_drag_canvas = NULL;
		ecore_x_window_del(ewl_dnd_drag_win);
		ecore_x_dnd_drop();

		/* Kill all last position references so they don't get
		 * carried over to the next drag */
		pos = ecore_hash_keys(ewl_dnd_position_hash);
		ecore_list_goto_first(pos);
		while ((val = ecore_list_remove_first(pos))) {
			EWL_WINDOW(val)->dnd_last_position = NULL;
			ecore_hash_remove(ewl_dnd_position_hash, val);
		}
		ecore_list_destroy(pos);

		ewl_dragging_current = 0;
		ewl_widget_dnd_reset();
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

