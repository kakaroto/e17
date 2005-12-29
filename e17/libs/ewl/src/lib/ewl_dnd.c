#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include <Ecore_Evas.h>
#include <Ecore_X.h>

#define EWL_DND_WINDOW_ROOT 0
static int ewl_dragging_current = 0;
static int ewl_dnd_move_count = 0;
static Ecore_Evas* _ewl_dnd_drag_canvas;
static Evas* _ewl_dnd_drag_evas;
static Evas_Object* _ewl_dnd_drag_image;
static Ecore_X_Window _ewl_dnd_evas_win;
static Ecore_X_Window _ewl_dnd_drag_win = 0;

static Ewl_Widget* _ewl_dnd_widget = NULL;

static Ecore_Hash* ewl_dnd_position_hash;
static int _ewl_dnd_status=0;

Ecore_Event_Handler* _ewl_dnd_mouse_up_handler;
Ecore_Event_Handler* _ewl_dnd_mouse_move_handler;


char *drop_types[] = { "text/uri-list" };
char *data[] = { "file:///home/chaos/wlan-key2" };


void ewl_dnd_init(void) {
	ewl_dnd_position_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
	ewl_dragging_current = 0;
	_ewl_dnd_status = 1;
}


void ewl_dnd_position_windows_set(Ewl_Widget* w) {
	ecore_hash_set(ewl_dnd_position_hash, w, (void*)1);
}



Ewl_Dnd_Types *
ewl_dnd_types_for_widget_get(Ewl_Widget *widget)
{
	Ewl_Widget *parent = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("widget", widget, NULL);
	DCHECK_TYPE_RET("widget", widget, "widget", NULL);
	
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



static int
_ewl_dnd_event_dnd_move(void *data __UNUSED__, int type __UNUSED__, 
							void *event)
{
	Ecore_X_Event_Mouse_Move *ev;
	ev = event;

	if (!_ewl_dnd_status) return 1;

	//printf("%d:%d\n", ev->x, ev->y);
	ewl_dnd_move_count++;

	if (ewl_dnd_move_count == 1) {
		ecore_evas_show(_ewl_dnd_drag_canvas);
	}

	if (_ewl_dnd_drag_canvas) ecore_evas_move(_ewl_dnd_drag_canvas, ev->x - 15, ev->y - 15);


	return 1;
}


static int
_ewl_dnd_event_mouse_up(void *data __UNUSED__, int type __UNUSED__, 
						void *event __UNUSED__)
{
	if (_ewl_dnd_drag_canvas && ewl_dragging_current) {
		Ecore_List* pos;
		void* val;

		ecore_x_pointer_ungrab();
		ecore_x_keyboard_ungrab();

		ecore_event_handler_del(_ewl_dnd_mouse_up_handler);
		ecore_event_handler_del(_ewl_dnd_mouse_move_handler);
	
		//evas_object_del(_ewl_dnd_drag_image);
		ecore_evas_free(_ewl_dnd_drag_canvas);
		//ecore_evas_hide(_ewl_dnd_drag_canvas);
		_ewl_dnd_drag_canvas = NULL;


		ecore_x_window_del(_ewl_dnd_drag_win);
		
	
		//ecore_x_window_del(_ewl_dnd_evas_win);
		ecore_x_dnd_drop();

		/* Kill all last position references so they don't get
		 * carried over to the next drag */
		pos = ecore_hash_keys(ewl_dnd_position_hash);
		ecore_list_goto_first(pos);
		while ( (val = ecore_list_remove_first(pos)) ) {
			EWL_WINDOW(val)->dnd_last_position = NULL;
			ecore_hash_remove(ewl_dnd_position_hash, val);
		}
		ecore_list_destroy(pos);

		ewl_dragging_current = 0;
		ewl_widget_dnd_reset();
	}

	return 1;
	
}


void ewl_drag_start(Ewl_Widget* w) {
	int width,height;

	if (!_ewl_dnd_status) return;

	if (ewl_dragging_current) {
		return;
	}

	ewl_dragging_current = 1;
	_ewl_dnd_widget = w;
	ewl_dnd_move_count = 0;

	
	_ewl_dnd_mouse_up_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_UP,_ewl_dnd_event_mouse_up, NULL);
	_ewl_dnd_mouse_move_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_MOVE, _ewl_dnd_event_dnd_move, NULL);

	_ewl_dnd_drag_canvas = ecore_evas_software_x11_new(NULL, EWL_DND_WINDOW_ROOT, 64,64,64,64); 
	_ewl_dnd_drag_evas = ecore_evas_get(_ewl_dnd_drag_canvas);

	ecore_evas_shaped_set(_ewl_dnd_drag_canvas, 1);
	ecore_evas_software_x11_direct_resize_set(_ewl_dnd_drag_canvas,1);

	_ewl_dnd_evas_win = ecore_evas_software_x11_window_get(_ewl_dnd_drag_canvas);
	ecore_x_window_resize(_ewl_dnd_evas_win, 64,64);
	ecore_evas_override_set(_ewl_dnd_drag_canvas, 1);

	
	//ecore_evas_software_x11_direct_resize_set(_ewl_dnd_drag_evas,1);
	ecore_evas_ignore_events_set(_ewl_dnd_drag_canvas, 1);



	/*Setup a cursor*/
	_ewl_dnd_drag_image = evas_object_image_add(_ewl_dnd_drag_evas);
	evas_object_image_file_set(_ewl_dnd_drag_image, PACKAGE_DATA_DIR "/images/World.png",0);	
 	evas_object_image_fill_set(_ewl_dnd_drag_image, 0,0,50,50);
	evas_object_resize(_ewl_dnd_drag_image, 50,50);
	evas_object_show(_ewl_dnd_drag_image);
	



	/*Setup the dnd event capture window*/
	ecore_x_window_geometry_get(EWL_DND_WINDOW_ROOT, NULL, NULL, &width,  &height);
	_ewl_dnd_drag_win = ecore_x_window_input_new(EWL_DND_WINDOW_ROOT, 
		0, 0,
		width,height);

	/*Finally show the drag window*/
	ecore_x_window_show(_ewl_dnd_drag_win);


	/*Confine the pointer to our event windows*/	
	ecore_x_pointer_confine_grab(_ewl_dnd_drag_win);
	ecore_x_keyboard_grab(_ewl_dnd_drag_win);

	ecore_x_dnd_aware_set(_ewl_dnd_drag_win, 1);
	ecore_x_dnd_aware_set(_ewl_dnd_evas_win, 1);
	ecore_x_mwm_borderless_set(_ewl_dnd_evas_win, 1);
	
	/*Start the drag operation*/
	ecore_x_dnd_types_set(_ewl_dnd_drag_win, drop_types, 1);
	ecore_x_dnd_begin(_ewl_dnd_drag_win, NULL, 0);


}


void ewl_dnd_disable(void) {
	_ewl_dnd_status=0;
}

void ewl_dnd_enable(void) {
	_ewl_dnd_status=1;
}

int ewl_dnd_status_get(void) {
	return _ewl_dnd_status;
}

Ewl_Widget* ewl_dnd_drag_widget_get(void) {
	return _ewl_dnd_widget;
}

void ewl_dnd_drag_widget_clear(void) {
	_ewl_dnd_widget = NULL;
}
