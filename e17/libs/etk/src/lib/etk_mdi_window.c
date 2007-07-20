/** @file etk_mdi_window.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_mdi_window.h"

#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_toplevel.h"

#include <Edje.h>

#include <stdlib.h>
#include <string.h>

/**
 * @addtogroup Etk_Mdi_Window
 * @{
 */

enum Etk_Widget_Signal_Id
{
   ETK_MDI_WINDOW_MOVED_SIGNAL,
   ETK_MDI_WINDOW_DELETE_EVENT_SIGNAL,
   ETK_MDI_WINDOW_NUM_SIGNALS
};

enum Etk_Mdi_Window_Property_Id
{
   ETK_MDI_WINDOW_TITLE_PROPERTY,
   ETK_MDI_WINDOW_MAXIMIZED_PROPERTY
};

static void _etk_mdi_window_constructor(Etk_Mdi_Window *mdi_window);
static void _etk_mdi_window_destructor(Etk_Mdi_Window *mdi_window);
static void _etk_mdi_window_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_mdi_window_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_mdi_window_realized_cb(Etk_Object *object, void *data);
static void _etk_mdi_window_titlebar_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_mdi_window_titlebar_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_mdi_window_titlebar_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_mdi_window_resize_mouse_in_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_mdi_window_resize_mouse_out_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_mdi_window_resize_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_mdi_window_resize_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_mdi_window_resize_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_mdi_window_maximize_mouse_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _etk_mdi_window_close_mouse_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static Etk_Bool _etk_mdi_window_delete_event_handler(Etk_Mdi_Window *mdi_window);

static Etk_Signal *_etk_mdi_window_signals[ETK_MDI_WINDOW_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Mdi_Window
 * @return Returns the type of an Etk_Mdi_Window
 */
Etk_Type *etk_mdi_window_type_get(void)
{
   static Etk_Type *mdi_window_type = NULL;

   if (!mdi_window_type)
   {
      mdi_window_type = etk_type_new("Etk_Mdi_Window", ETK_BIN_TYPE, sizeof(Etk_Mdi_Window),
         ETK_CONSTRUCTOR(_etk_mdi_window_constructor), ETK_DESTRUCTOR(_etk_mdi_window_destructor));

      _etk_mdi_window_signals[ETK_MDI_WINDOW_MOVED_SIGNAL] = etk_signal_new("moved", mdi_window_type,
         -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_mdi_window_signals[ETK_MDI_WINDOW_DELETE_EVENT_SIGNAL] = etk_signal_new("delete-event", mdi_window_type,
         ETK_MEMBER_OFFSET(Etk_Mdi_Window, delete_event), etk_marshaller_BOOL__VOID, etk_accumulator_bool_or, NULL);

      etk_type_property_add(mdi_window_type, "title", ETK_MDI_WINDOW_TITLE_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(mdi_window_type, "maximized", ETK_MDI_WINDOW_MAXIMIZED_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));

      mdi_window_type->property_set = _etk_mdi_window_property_set;
      mdi_window_type->property_get = _etk_mdi_window_property_get;
   }

   return mdi_window_type;
}

/**
 * @brief Creates a new mdi_window
 * @return Returns the new mdi_window widget
 */
Etk_Widget *etk_mdi_window_new(void)
{
   return etk_widget_new(ETK_MDI_WINDOW_TYPE, "theme-group", "mdi_window", NULL);
}

/**
 * @brief Emits a delete-request on the mdi_window: it will call all the callbacks connected to the "delete-event" signal
 * and if all these callbacks return ETK_FALSE, the mdi_window will be destroyed. It has the same effect as if the user
 * had clicked on the "close" button of the mdi_window
 * @param mdi_window a mdi_window
 */
void etk_mdi_window_delete_request(Etk_Mdi_Window *mdi_window)
{
   Etk_Bool result;

   etk_signal_emit(_etk_mdi_window_signals[ETK_MDI_WINDOW_DELETE_EVENT_SIGNAL], ETK_OBJECT(mdi_window), &result);
   if (!result)
   {
      etk_widget_parent_set(ETK_WIDGET(mdi_window), NULL);
      etk_object_destroy(ETK_OBJECT(mdi_window));
   }
}

/**
 * @brief Sets the title of the mdi_window
 * @param mdi_window a mdi_window
 * @param title the title to set
 */
void etk_mdi_window_title_set(Etk_Mdi_Window *mdi_window, const char *title)
{
   if (!mdi_window)
      return;

   if (title != mdi_window->title)
   {
      free(mdi_window->title);
      mdi_window->title = title ? strdup(title) : NULL;
   }

   etk_widget_theme_part_text_set(ETK_WIDGET(mdi_window), "etk.text.title", mdi_window->title ? mdi_window->title : "");

   if (title != mdi_window->title)
      etk_object_notify(ETK_OBJECT(mdi_window), "title");
}

/**
 * @brief Gets the title of the mdi_window
 * @param mdi_window a mdi_window
 * @return Returns the title of the mdi_window
 */
const char *etk_mdi_window_title_get(Etk_Mdi_Window *mdi_window)
{
   if (!mdi_window)
      return NULL;

   return mdi_window->title;
}

void etk_mdi_window_move(Etk_Mdi_Window *mdi_window, int x, int y)
{
   if (!mdi_window)
      return;

   mdi_window->position.x = x;
   mdi_window->position.y = y;

   etk_signal_emit(_etk_mdi_window_signals[ETK_MDI_WINDOW_MOVED_SIGNAL], ETK_OBJECT(mdi_window), NULL);
}

void etk_mdi_window_position_get(Etk_Mdi_Window *mdi_window, int *x, int *y)
{
   if (!mdi_window)
      return;

   if (x) *x = mdi_window->position.x;
   if (y) *y = mdi_window->position.y;
}

/**
 * @brief Sets whether or not the mdi_window is maximized
 * @param mdi_window a mdi_window
 * @param maximized ETK_TRUE to maximize the mdi_window, ETK_FALSE to unmaximize it
 */
void etk_mdi_window_maximized_set(Etk_Mdi_Window *mdi_window, Etk_Bool maximized)
{
   if (!mdi_window)
      return;

   if (mdi_window->maximized != maximized)
   {
      mdi_window->maximized = maximized;
      etk_object_notify(ETK_OBJECT(mdi_window), "maximized");
   }
}

/**
 * @brief Gets whether the mdi_window is maximized
 * @param mdi_window a mdi_window
 * @return Returns ETK_TRUE if the mdi_window is maximized, ETK_FALSE otherwise
 */
Etk_Bool etk_mdi_window_maximized_get(Etk_Mdi_Window *mdi_window)
{
   if (!mdi_window)
      return ETK_FALSE;

   return mdi_window->maximized;
}
/**
 * @brief A utility function to use as a callback for the "delete-event" signal.
 * It will hide the mdi_window and return ETK_TRUE to prevent the program from quitting
 * @param mdi_window the mdi_window to hide
 * @param data the data passed when the signal is emitted - unused
 * @return Return ETK_TRUE so the the program won't quit
 */
Etk_Bool etk_mdi_window_hide_on_delete(Etk_Object *mdi_window, void *data)
{
   if (!mdi_window)
      return ETK_TRUE;

   etk_widget_hide(ETK_WIDGET(mdi_window));
   return ETK_TRUE;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_mdi_window_constructor(Etk_Mdi_Window *mdi_window)
{
   if (!mdi_window)
      return;

   mdi_window->position.x = 0;
   mdi_window->position.y = 0;
   mdi_window->title = NULL;
   mdi_window->maximized = ETK_FALSE;
   mdi_window->dragging = ETK_FALSE;
   mdi_window->resizing = ETK_FALSE;

   mdi_window->delete_event = _etk_mdi_window_delete_event_handler;

   etk_signal_connect("realized", ETK_OBJECT(mdi_window), ETK_CALLBACK(_etk_mdi_window_realized_cb), NULL);
}

/* Destroys the mdi_window */
static void _etk_mdi_window_destructor(Etk_Mdi_Window *mdi_window)
{
   if (!mdi_window)
      return;

   free(mdi_window->title);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_mdi_window_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Mdi_Window *mdi_window;

   if (!(mdi_window = ETK_MDI_WINDOW(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_MDI_WINDOW_TITLE_PROPERTY:
         etk_mdi_window_title_set(mdi_window, etk_property_value_string_get(value));
         break;
      case ETK_MDI_WINDOW_MAXIMIZED_PROPERTY:
         etk_mdi_window_maximized_set(mdi_window, etk_property_value_bool_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_mdi_window_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Mdi_Window *mdi_window;

   if (!(mdi_window = ETK_MDI_WINDOW(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_MDI_WINDOW_TITLE_PROPERTY:
         etk_property_value_string_set(value, mdi_window->title);
         break;
      case ETK_MDI_WINDOW_MAXIMIZED_PROPERTY:
         etk_property_value_bool_set(value, etk_mdi_window_maximized_get(mdi_window));
         break;
      default:
         break;
   }
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the mdi_window is realized */
static void _etk_mdi_window_realized_cb(Etk_Object *object, void *data)
{
   Etk_Mdi_Window *mdi_window;
   Evas_Object *theme_object;
   Evas_Object *o;

   if (!(mdi_window = ETK_MDI_WINDOW(object)) || !(theme_object = ETK_WIDGET(mdi_window)->theme_object))
      return;

   etk_mdi_window_title_set(mdi_window, mdi_window->title);

   o = edje_object_part_object_get(theme_object, "etk.event.titlebar");
   if (o)
   {
      evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, _etk_mdi_window_titlebar_mouse_down_cb, mdi_window);
      evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP, _etk_mdi_window_titlebar_mouse_up_cb, mdi_window);
      evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE, _etk_mdi_window_titlebar_mouse_move_cb, mdi_window);
   }

   o = edje_object_part_object_get(theme_object, "etk.event.resize");
   if (o)
   {
      evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_IN, _etk_mdi_window_resize_mouse_in_cb, mdi_window);
      evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_OUT, _etk_mdi_window_resize_mouse_out_cb, mdi_window);
      evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, _etk_mdi_window_resize_mouse_down_cb, mdi_window);
      evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP, _etk_mdi_window_resize_mouse_up_cb, mdi_window);
      evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE, _etk_mdi_window_resize_mouse_move_cb, mdi_window);
   }

   edje_object_signal_callback_add(theme_object, "mouse,clicked,1*", "etk.event.maximize",
      _etk_mdi_window_maximize_mouse_clicked_cb, mdi_window);
   edje_object_signal_callback_add(theme_object, "mouse,clicked,1*", "etk.event.close",
      _etk_mdi_window_close_mouse_clicked_cb, mdi_window);
}

/* Called when the titlebar of the mdi_window is pressed */
static void _etk_mdi_window_titlebar_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Mdi_Window *mdi_window;
   Evas_Event_Mouse_Down *ev;

   if (!(mdi_window = ETK_MDI_WINDOW(data)))
      return;

   ev = event_info;

   if (!mdi_window->maximized)
   {
      mdi_window->drag_offset_x = ev->canvas.x - mdi_window->position.x;
      mdi_window->drag_offset_y = ev->canvas.y - mdi_window->position.y;
      mdi_window->dragging = ETK_TRUE;
   }

   etk_widget_raise(ETK_WIDGET(mdi_window));
}

/* Called when the titlebar of the mdi_window is released */
static void _etk_mdi_window_titlebar_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Mdi_Window *mdi_window;

   if (!(mdi_window = ETK_MDI_WINDOW(data)))
      return;

   mdi_window->dragging = ETK_FALSE;
}

/* Called when the titlebar of the mdi_window is moved */
static void _etk_mdi_window_titlebar_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Mdi_Window *mdi_window;
   Evas_Event_Mouse_Move *ev;

   if (!(mdi_window = ETK_MDI_WINDOW(data)) || !(mdi_window->dragging))
      return;

   ev = event_info;

   etk_mdi_window_move(mdi_window,
      ev->cur.canvas.x - mdi_window->drag_offset_x,
      ev->cur.canvas.y - mdi_window->drag_offset_y);
}

/* Called when the mouse pointer enters the resize-rect of the mdi_window's border */
static void _etk_mdi_window_resize_mouse_in_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
   Etk_Mdi_Window *mdi_window;

   if (!(mdi_window = ETK_MDI_WINDOW(data)) || mdi_window->maximized)
      return;

   etk_toplevel_pointer_push(etk_widget_toplevel_parent_get(ETK_WIDGET(mdi_window)), ETK_POINTER_RESIZE_BR);
}

/* Called when the mouse pointer leaves the resize-rect of the mdi_window's border */
static void _etk_mdi_window_resize_mouse_out_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
   Etk_Mdi_Window *mdi_window;

   if (!(mdi_window = ETK_MDI_WINDOW(data)))
      return;

   etk_toplevel_pointer_pop(etk_widget_toplevel_parent_get(ETK_WIDGET(mdi_window)), ETK_POINTER_RESIZE_BR);
}

/* Called when the mouse presses the resize-rect of the mdi_window's border */
static void _etk_mdi_window_resize_mouse_down_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
   Etk_Mdi_Window *mdi_window;
   Evas_Event_Mouse_Down *ev;
   int w, h;

   if (!(mdi_window = ETK_MDI_WINDOW(data)) || mdi_window->maximized)
      return;

   ev = event_info;
   etk_widget_geometry_get(ETK_WIDGET(mdi_window), NULL, NULL, &w, &h);
   mdi_window->resizing = ETK_TRUE;
   mdi_window->resize_offset_x = ev->canvas.x - w;
   mdi_window->resize_offset_y = ev->canvas.y - h;
}

/* Called when the mouse releases the resize-rect of the mdi_window's border */
static void _etk_mdi_window_resize_mouse_up_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
   Etk_Mdi_Window *mdi_window;

   if (!(mdi_window = ETK_MDI_WINDOW(data)))
      return;

   mdi_window->resizing = ETK_FALSE;
}

static void _etk_mdi_window_resize_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Mdi_Window *mdi_window;
   Evas_Event_Mouse_Move *ev;

   if (!(mdi_window = ETK_MDI_WINDOW(data)) || !(mdi_window->resizing))
      return;

   ev = event_info;
   etk_widget_size_request_set(ETK_WIDGET(mdi_window),
      ev->cur.canvas.x - mdi_window->resize_offset_x,
      ev->cur.canvas.y - mdi_window->resize_offset_y);
}

/* Called when the mouse releases the maximize button */
static void _etk_mdi_window_maximize_mouse_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Mdi_Window *mdi_window;

   if (!(mdi_window = ETK_MDI_WINDOW(data)))
      return;

   etk_mdi_window_maximized_set(mdi_window, !etk_mdi_window_maximized_get(mdi_window));
   etk_widget_raise(ETK_WIDGET(mdi_window));
}

/* Called when the mouse releases the close button */
static void _etk_mdi_window_close_mouse_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Mdi_Window *mdi_window;

   if (!(mdi_window = ETK_MDI_WINDOW(data)))
      return;

   etk_mdi_window_delete_request(mdi_window);
}

/* Default handler for the "delete-event" signal */
static Etk_Bool _etk_mdi_window_delete_event_handler(Etk_Mdi_Window *mdi_window)
{
   return ETK_FALSE;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Mdi_Window
 *
 * @image html widgets/mdi_window.png
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Bin
 *         - Etk_Mdi_Window
 *
 * \par Properties:
 * @prop_name "title": The title of the mdi_window
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 */
