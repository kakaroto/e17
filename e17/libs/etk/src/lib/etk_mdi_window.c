/** @file etk_mdi_window.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_mdi_window.h"
#include "etk_config.h"
#include "etk_engine.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

#include <Ecore_File.h>
#include <Edje.h>

#include <limits.h>
#include <stdlib.h>
#include <string.h>

/**
 * @addtogroup Etk_Mdi_Window
 * @{
 */

#define ETK_MDI_WINDOW_DATA(data) ((Etk_Mdi_Window_Data*)data)

enum Etk_Widget_Signal_Id
{
   ETK_MDI_WINDOW_MOVED_SIGNAL,
   ETK_MDI_WINDOW_RESIZED_SIGNAL,
   ETK_MDI_WINDOW_DELETE_EVENT_SIGNAL,
   ETK_MDI_WINDOW_NUM_SIGNALS
};

enum Etk_Mdi_Window_Property_Id
{
   ETK_MDI_WINDOW_TITLE_PROPERTY,
   ETK_MDI_WINDOW_MAXIMIZED_PROPERTY,
   ETK_MDI_WINDOW_DECORATED_PROPERTY
};

/* Engine specific data for Etk_Mdi_Window */
struct _Etk_Mdi_Window_Data
{
   Etk_Size size;
   Etk_Size min_size;
   Evas_Object *border;
   Etk_Position border_position;
   Etk_Bool borderless;
   Etk_Bool visible;
   Etk_Bool maximized;
   Etk_Bool is_dragging;
   int drag_offset_x;
   int drag_offset_y;
   Etk_Bool is_resizing;
   int resize_offset_x;
   int resize_offset_y;
   char *title;
};

static void _etk_mdi_window_constructor(Etk_Mdi_Window *mdi_window);
static void _etk_mdi_window_destructor(Etk_Mdi_Window *mdi_window);
static void _etk_mdi_window_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_mdi_window_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_mdi_window_shown_cb(Etk_Object *object, void *data);
static void _etk_mdi_window_hidden_cb(Etk_Object *object, void *data);
static void _etk_mdi_window_size_requested_cb(Etk_Object *object, Etk_Size *requested_size, void *data);
static void _etk_mdi_window_evas_position_get(Etk_Toplevel *toplevel, int *x, int *y);
static void _etk_mdi_window_screen_position_get(Etk_Toplevel *toplevel, int *x, int *y);
static void _etk_mdi_window_size_get(Etk_Toplevel *toplevel, int *w, int *h);
static void _etk_mdi_window_disable(Etk_Mdi_Window *mdi_window);
static void _etk_mdi_window_enable(Etk_Mdi_Window *mdi_window);
static void _etk_mdi_window_realized_cb(Etk_Object *object, void *data);
static void _etk_mdi_window_unrealized_cb(Etk_Object *object, void *data);
static void _etk_mdi_window_titlebar_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_mdi_window_titlebar_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_mdi_window_titlebar_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_mdi_window_resize_mouse_in_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_mdi_window_resize_mouse_out_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_mdi_window_resize_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_mdi_window_resize_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_mdi_window_resize_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_mdi_window_maximize_mouse_up_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _etk_mdi_window_close_mouse_up_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static Etk_Bool _etk_mdi_window_delete_event_handler(Etk_Mdi_Window *mdi_window);
static char *_etk_mdi_window_wm_theme_path_get(void);
static char *_etk_mdi_window_wm_theme_find(const char *theme_name);

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
      mdi_window_type = etk_type_new("Etk_Mdi_Window", ETK_TOPLEVEL_TYPE, sizeof(Etk_Mdi_Window), ETK_CONSTRUCTOR(_etk_mdi_window_constructor), ETK_DESTRUCTOR(_etk_mdi_window_destructor));

      /* FIXME proper emit signal move, resize */
      _etk_mdi_window_signals[ETK_MDI_WINDOW_MOVED_SIGNAL] = etk_signal_new("moved", mdi_window_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_mdi_window_signals[ETK_MDI_WINDOW_RESIZED_SIGNAL] = etk_signal_new("resized", mdi_window_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_mdi_window_signals[ETK_MDI_WINDOW_DELETE_EVENT_SIGNAL] = etk_signal_new("delete-event", mdi_window_type, ETK_MEMBER_OFFSET(Etk_Mdi_Window, delete_event), etk_marshaller_BOOL__VOID, etk_accumulator_bool_or, NULL);

      etk_type_property_add(mdi_window_type, "title", ETK_MDI_WINDOW_TITLE_PROPERTY, ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(mdi_window_type, "maximized", ETK_MDI_WINDOW_MAXIMIZED_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(mdi_window_type, "decorated", ETK_MDI_WINDOW_DECORATED_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));

      mdi_window_type->property_set = _etk_mdi_window_property_set;
      mdi_window_type->property_get = _etk_mdi_window_property_get;
   }

   return mdi_window_type;
}

/**
 * @brief Creates a new mdi_window
 * @return Returns the new mdi_window widget
 */
Etk_Widget *etk_mdi_window_new(Evas *evas)
{
   Etk_Widget *mdi_window;

   mdi_window = etk_widget_new(ETK_MDI_WINDOW_TYPE, "theme-group", "window", NULL);
   ETK_TOPLEVEL(mdi_window)->evas = evas;
   etk_object_notify(ETK_OBJECT(mdi_window), "evas");

   return mdi_window;
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
      etk_object_destroy(ETK_OBJECT(mdi_window));
}

/**
 * @brief Sets the title of the mdi_window
 * @param mdi_window the mdi_window
 * @param title the title to set
 */
void etk_mdi_window_title_set(Etk_Mdi_Window *mdi_window, const char *title)
{
   if (!mdi_window)
      return;

   if (mdi_window->data->title != title)
   {
      free(mdi_window->data->title);
      mdi_window->data->title = strdup(title);

      if (mdi_window->data->border)
         edje_object_part_text_set(mdi_window->data->border, "etk.text.title", title ? title : "");

      etk_object_notify(ETK_OBJECT(mdi_window), "title");
   }
}

/**
 * @brief Gets the title of the mdi_window
 * @param mdi_window the mdi_window
 * @return Returns the title of the mdi_window
 */
const char *etk_mdi_window_title_get(Etk_Mdi_Window *mdi_window)
{
   if (!mdi_window)
      return NULL;

   return mdi_window->data->title;
}

/**
 * @brief Moves the mdi_window to the position (x, y)
 * @param mdi_window a mdi_window
 * @param x the x position
 * @param y the y position
 */
void etk_mdi_window_move(Etk_Mdi_Window *mdi_window, int x, int y)
{
   if (!mdi_window)
      return;

   mdi_window->data->border_position.x = x;
   mdi_window->data->border_position.y = y;
   if (mdi_window->data->border && !mdi_window->data->maximized)
      evas_object_move(mdi_window->data->border, x, y);
}

/**
 * @brief Resizes the mdi_window
 * @param mdi_window a mdi_window
 * @param w the new width of the mdi_window
 * @param h the new height of the mdi_window
 */
void etk_mdi_window_resize(Etk_Mdi_Window *mdi_window, int w, int h)
{
   if (!mdi_window)
      return;

   mdi_window->data->size.w = ETK_MAX(mdi_window->data->min_size.w, w);
   mdi_window->data->size.h = ETK_MAX(mdi_window->data->min_size.h, h);
   if (mdi_window->data->border && ETK_WIDGET(mdi_window)->smart_object && !mdi_window->data->maximized)
   {
      int border_w, border_h;

      edje_extern_object_min_size_set(ETK_WIDGET(mdi_window)->smart_object, mdi_window->data->size.w, mdi_window->data->size.h);
      edje_object_part_swallow(mdi_window->data->border, "etk.swallow.content", ETK_WIDGET(mdi_window)->smart_object);
      edje_object_size_min_calc(mdi_window->data->border, &border_w, &border_h);
      evas_object_resize(mdi_window->data->border, border_w, border_h);
   }
}

/**
 * @brief Gets the geometry of the mdi_window
 * @param mdi_window a mdi_window
 * @param x the location where to set the x position the mdi_window
 * @param y the location where to set the y position the mdi_window
 * @param w the location where to set the width of the mdi_window
 * @param h the location where to set the height of the mdi_window
 */
void etk_mdi_window_geometry_get(Etk_Mdi_Window *mdi_window, int *x, int *y, int *w, int *h)
{
   if (!mdi_window)
      return;
   etk_toplevel_screen_position_get(ETK_TOPLEVEL(mdi_window), x, y);
   etk_toplevel_size_get(ETK_TOPLEVEL(mdi_window), w, h);
}

/**
 * @brief Makes a mdi_window modal for another mdi_window
 * @param window_to_modal the mdi_window to make modal
 * @param mdi_window the mdi_window on which @a window_to_modal will modal'ed on, or NULL to disable the modal state
 */
void etk_mdi_window_modal_for_window(Etk_Mdi_Window *window_to_modal, Etk_Mdi_Window *mdi_window)
{
   if (!window_to_modal)
      return;

   if (mdi_window)
   {
      /* TODO: grab the input */
      etk_signal_connect_swapped("shown", ETK_OBJECT(window_to_modal), ETK_CALLBACK(_etk_mdi_window_disable), mdi_window);
      etk_signal_connect_swapped("hidden", ETK_OBJECT(window_to_modal), ETK_CALLBACK(_etk_mdi_window_enable), mdi_window);
      etk_signal_connect_swapped("delete-event", ETK_OBJECT(window_to_modal), ETK_CALLBACK(_etk_mdi_window_enable), mdi_window);
   }
   else
   {
      etk_signal_disconnect("shown", ETK_OBJECT(window_to_modal), ETK_CALLBACK(_etk_mdi_window_disable));
      etk_signal_disconnect("hidden", ETK_OBJECT(window_to_modal), ETK_CALLBACK(_etk_mdi_window_enable));
      etk_signal_disconnect("delete-event", ETK_OBJECT(window_to_modal), ETK_CALLBACK(_etk_mdi_window_enable));
   }
}

/**
 * @brief Raises the mdi_window.
 * @param mdi_window a mdi_window
 */
void etk_mdi_window_raise(Etk_Mdi_Window *mdi_window)
{
   if (!mdi_window)
      return;

   if (mdi_window->data->border)
   {
      evas_object_raise(mdi_window->data->border);
   }
}

/**
 * @brief Lowers the mdi_window.
 * @param mdi_window a mdi_window
 */
void etk_mdi_window_lower(Etk_Mdi_Window *mdi_window)
{
   if (!mdi_window)
      return;

   if (mdi_window->data->border)
      evas_object_lower(mdi_window->data->border);
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

   if (mdi_window->data->maximized != maximized)
   {
      mdi_window->data->maximized = maximized;
      if (mdi_window->data->border)
      {
         if (maximized)
         {
            int w, h;
            evas_output_size_get(ETK_TOPLEVEL(mdi_window)->evas, &w, &h);
            evas_object_move(mdi_window->data->border, 0, 0);
            evas_object_resize(mdi_window->data->border, w, h);
         }
         else
         {
            evas_object_move(mdi_window->data->border, mdi_window->data->border_position.x, mdi_window->data->border_position.y);
            etk_mdi_window_resize(mdi_window, mdi_window->data->size.w, mdi_window->data->size.h);
         }
      }

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

   return mdi_window->data->maximized;
}

/**
 * @brief Sets wheter the mdi_window is decorated
 * @param mdi_window a mdi_window
 * @param decorated if @a decorated is ETK_FALSE, the border of the mdi_window will be hidden
 */
void etk_mdi_window_decorated_set(Etk_Mdi_Window *mdi_window, Etk_Bool decorated)
{
   if (!mdi_window)
      return;

   if (mdi_window->data->borderless == !decorated)
      return;

   mdi_window->data->borderless = !decorated;
   if (mdi_window->data->border)
   {
      /* Recreate the border */
      _etk_mdi_window_unrealized_cb(ETK_OBJECT(mdi_window), NULL);
      _etk_mdi_window_realized_cb(ETK_OBJECT(mdi_window), NULL);
   }
   etk_object_notify(ETK_OBJECT(mdi_window), "decorated");
}

/**
 * @brief Gets whether the mdi_window is decorated (i.e. whether the border of the mdi_window is shown)
 * @param mdi_window a mdi_window
 * @return Returns ETK_TRUE if the mdi_window is decorated
 */
Etk_Bool etk_mdi_window_decorated_get(Etk_Mdi_Window *mdi_window)
{
   if (!mdi_window)
      return ETK_TRUE;

   return !mdi_window->data->borderless;
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
   etk_widget_hide(ETK_WIDGET(mdi_window));
   return ETK_TRUE;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members and build the mdi_window */
static void _etk_mdi_window_constructor(Etk_Mdi_Window *mdi_window)
{
   if (!mdi_window)
      return;

   /* TODO should we use the theme itself, or have a separate wm theme? */
   mdi_window->wm_theme_file = _etk_mdi_window_wm_theme_path_get();
   mdi_window->wait_size_request = ETK_TRUE;
   mdi_window->data = malloc(sizeof(Etk_Mdi_Window_Data));
   mdi_window->data->size.w = 32;
   mdi_window->data->size.h = 32;
   mdi_window->data->min_size.w = 0;
   mdi_window->data->min_size.h = 0;
   mdi_window->data->border = NULL;
   mdi_window->data->border_position.x = 0;
   mdi_window->data->border_position.y = 0;
   mdi_window->data->borderless = ETK_FALSE;
   mdi_window->data->visible = ETK_FALSE;
   mdi_window->data->maximized = ETK_FALSE;
   mdi_window->data->is_dragging = ETK_FALSE;
   mdi_window->data->is_resizing = ETK_FALSE;
   mdi_window->data->title = NULL;

   mdi_window->delete_event = _etk_mdi_window_delete_event_handler;
   ETK_TOPLEVEL(mdi_window)->evas_position_get = _etk_mdi_window_evas_position_get;
   ETK_TOPLEVEL(mdi_window)->screen_position_get = _etk_mdi_window_screen_position_get;
   ETK_TOPLEVEL(mdi_window)->size_get = _etk_mdi_window_size_get;

   etk_signal_connect("size-requested", ETK_OBJECT(mdi_window), ETK_CALLBACK(_etk_mdi_window_size_requested_cb), NULL);
   etk_signal_connect("realized", ETK_OBJECT(mdi_window), ETK_CALLBACK(_etk_mdi_window_realized_cb), NULL);
   etk_signal_connect("unrealized", ETK_OBJECT(mdi_window), ETK_CALLBACK(_etk_mdi_window_unrealized_cb), NULL);
   etk_signal_connect("shown", ETK_OBJECT(mdi_window), ETK_CALLBACK(_etk_mdi_window_shown_cb), NULL);
   etk_signal_connect("hidden", ETK_OBJECT(mdi_window), ETK_CALLBACK(_etk_mdi_window_hidden_cb), NULL);
}

/* Destroys the mdi_window */
static void _etk_mdi_window_destructor(Etk_Mdi_Window *mdi_window)
{
   if (!mdi_window)
      return;

   free(mdi_window->wm_theme_file);
   free(mdi_window->data->title);
   free(mdi_window->data);
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
      case ETK_MDI_WINDOW_DECORATED_PROPERTY:
         etk_mdi_window_decorated_set(mdi_window, etk_property_value_bool_get(value));
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
         etk_property_value_string_set(value, etk_mdi_window_title_get(mdi_window));
         break;
      case ETK_MDI_WINDOW_MAXIMIZED_PROPERTY:
         etk_property_value_bool_set(value, etk_mdi_window_maximized_get(mdi_window));
         break;
      case ETK_MDI_WINDOW_DECORATED_PROPERTY:
         etk_property_value_bool_set(value, etk_mdi_window_decorated_get(mdi_window));
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

/* Called when the mdi_window is shown */
static void _etk_mdi_window_shown_cb(Etk_Object *object, void *data)
{
   Etk_Mdi_Window *mdi_window;

   if (!(mdi_window = ETK_MDI_WINDOW(object)) || mdi_window->wait_size_request)
      return;

   mdi_window->data->visible = ETK_TRUE;
   if (mdi_window->data->border)
      evas_object_show(mdi_window->data->border);
}

/* Called when the mdi_window is hidden */
static void _etk_mdi_window_hidden_cb(Etk_Object *object, void *data)
{
   Etk_Mdi_Window *mdi_window;

   if (!(mdi_window = ETK_MDI_WINDOW(object)) || mdi_window->wait_size_request)
      return;

   mdi_window->data->visible = ETK_FALSE;
   if (mdi_window->data->border)
      evas_object_hide(mdi_window->data->border);
}

/* Called when the "size-requested" signal is emitted */
static void _etk_mdi_window_size_requested_cb(Etk_Object *object, Etk_Size *requested_size, void *data)
{
   Etk_Mdi_Window *mdi_window;

   if (!(mdi_window = ETK_MDI_WINDOW(object)) || !requested_size)
      return;

   if (requested_size->w >= 0 && requested_size->h >= 0)
   {
      mdi_window->data->min_size.w = requested_size->w;
      mdi_window->data->min_size.h = requested_size->h;
      if (mdi_window->data->size.w < requested_size->w || mdi_window->data->size.h < requested_size->h)
         etk_mdi_window_resize(mdi_window, requested_size->w, requested_size->h);

      if (mdi_window->wait_size_request)
      {
         mdi_window->wait_size_request = ETK_FALSE;
         if (etk_widget_is_visible(ETK_WIDGET(mdi_window)))
         {
            mdi_window->data->visible = ETK_TRUE;
            if (mdi_window->data->border)
               evas_object_show(mdi_window->data->border);
         }
      }
   }
}

/* Gets the evas position of the mdi_window */
static void _etk_mdi_window_evas_position_get(Etk_Toplevel *toplevel, int *x, int *y)
{
   if (ETK_WIDGET(toplevel)->smart_object)
   {
      evas_object_geometry_get(ETK_WIDGET(toplevel)->smart_object, x, y, NULL, NULL);
   }
   else
   {
      if (x) *x = 0;
      if (y) *y = 0;
   }
}

/* Gets the screen position of the mdi_window */
static void _etk_mdi_window_screen_position_get(Etk_Toplevel *toplevel, int *x, int *y)
{
   if (ETK_WIDGET(toplevel)->smart_object)
   {
      evas_object_geometry_get(ETK_WIDGET(toplevel)->smart_object, x, y, NULL, NULL);
   }
   else
   {
      if (x) *x = 0;
      if (y) *y = 0;
   }
}

/* Gets the size of the mdi_window */
static void _etk_mdi_window_size_get(Etk_Toplevel *toplevel, int *w, int *h)
{
   Etk_Mdi_Window *mdi_window = ETK_MDI_WINDOW(toplevel);

   if (mdi_window->data->maximized && ETK_WIDGET(mdi_window)->smart_object)
      evas_object_geometry_get(ETK_WIDGET(mdi_window)->smart_object, NULL, NULL, w, h);
   else
   {
      if (w) *w = mdi_window->data->size.w;
      if (h) *h = mdi_window->data->size.h;
   }
}

/* Disable a mdi_window */
static void _etk_mdi_window_disable(Etk_Mdi_Window *mdi_window)
{
   if (!mdi_window)
      return;

   etk_widget_disabled_set(ETK_WIDGET(mdi_window), ETK_TRUE);
}

/* Enable a mdi_window */
static void _etk_mdi_window_enable(Etk_Mdi_Window *mdi_window)
{
   if (!mdi_window)
      return;

   etk_widget_disabled_set(ETK_WIDGET(mdi_window), ETK_FALSE);
}

/* Called when the mdi_window is realized: it creates the border */
static void _etk_mdi_window_realized_cb(Etk_Object *object, void *data)
{
   Etk_Mdi_Window *mdi_window;
   int border_w, border_h;
   Evas_Object *o;

   if (!(mdi_window = ETK_MDI_WINDOW(object)))
      return;

   mdi_window->data->border = edje_object_add(ETK_TOPLEVEL(mdi_window)->evas);
   if (mdi_window->data->borderless)
      edje_object_file_set(mdi_window->data->border, mdi_window->wm_theme_file, "etk/wm_borderless");
   else
      edje_object_file_set(mdi_window->data->border, mdi_window->wm_theme_file, "etk/wm_border");
   edje_object_part_text_set(mdi_window->data->border, "etk.text.title", mdi_window->data->title ? mdi_window->data->title : "");

   edje_extern_object_min_size_set(ETK_WIDGET(mdi_window)->smart_object, mdi_window->data->size.w, mdi_window->data->size.h);
   edje_object_part_swallow(mdi_window->data->border, "etk.swallow.content", ETK_WIDGET(mdi_window)->smart_object);
   edje_object_size_min_calc(mdi_window->data->border, &border_w, &border_h);

   if (mdi_window->data->maximized)
   {
      int w, h;
      evas_output_size_get(ETK_TOPLEVEL(mdi_window)->evas, &w, &h);
      evas_object_move(mdi_window->data->border, 0, 0);
      evas_object_resize(mdi_window->data->border, w, h);
   }
   else
   {
      evas_object_move(mdi_window->data->border, mdi_window->data->border_position.x, mdi_window->data->border_position.y);
      evas_object_resize(mdi_window->data->border, border_w, border_h);
   }

   if (mdi_window->data->visible)
      evas_object_show(mdi_window->data->border);

   o = edje_object_part_object_get(mdi_window->data->border, "etk.event.titlebar");
   if (o)
   {
      evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, _etk_mdi_window_titlebar_mouse_down_cb, mdi_window);
      evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP, _etk_mdi_window_titlebar_mouse_up_cb, mdi_window);
      evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE, _etk_mdi_window_titlebar_mouse_move_cb, mdi_window);
   }

   o = edje_object_part_object_get(mdi_window->data->border, "etk.event.resize");
   if (o)
   {
      evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_IN, _etk_mdi_window_resize_mouse_in_cb, mdi_window);
      evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_OUT, _etk_mdi_window_resize_mouse_out_cb, mdi_window);
      evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, _etk_mdi_window_resize_mouse_down_cb, mdi_window);
      evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP, _etk_mdi_window_resize_mouse_up_cb, mdi_window);
      evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE, _etk_mdi_window_resize_mouse_move_cb, mdi_window);
   }

   edje_object_signal_callback_add(mdi_window->data->border, "mouse,clicked,1*", "etk.event.maximize",
      _etk_mdi_window_maximize_mouse_up_cb, mdi_window);
   edje_object_signal_callback_add(mdi_window->data->border, "mouse,clicked,1*", "etk.event.close",
      _etk_mdi_window_close_mouse_up_cb, mdi_window);
}

/* Called when the mdi_window is unrealized: it destroys the border */
static void _etk_mdi_window_unrealized_cb(Etk_Object *object, void *data)
{
   Etk_Mdi_Window *mdi_window;

   if (!(mdi_window = ETK_MDI_WINDOW(object)))
      return;

   if (mdi_window->data->border)
   {
      evas_object_del(mdi_window->data->border);
      mdi_window->data->border = NULL;
   }
}

/* Called when the titlebar of the mdi_window is pressed */
static void _etk_mdi_window_titlebar_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Mdi_Window *mdi_window;
   Evas_Event_Mouse_Down *ev;

   if (!(mdi_window = ETK_MDI_WINDOW(data)))
      return;

   ev = event_info;

   if (!mdi_window->data->maximized)
   {
      mdi_window->data->drag_offset_x = ev->canvas.x - mdi_window->data->border_position.x;
      mdi_window->data->drag_offset_y = ev->canvas.y - mdi_window->data->border_position.y;
      mdi_window->data->is_dragging = ETK_TRUE;
   }

   etk_mdi_window_raise(mdi_window);
}

/* Called when the titlebar of the mdi_window is released */
static void _etk_mdi_window_titlebar_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Mdi_Window *mdi_window;

   if (!(mdi_window = ETK_MDI_WINDOW(data)))
      return;

   mdi_window->data->is_dragging = ETK_FALSE;
}

/* Called when the titlebar of the mdi_window is moved */
static void _etk_mdi_window_titlebar_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Mdi_Window *mdi_window;
   Evas_Event_Mouse_Move *ev;

   if (!(mdi_window = ETK_MDI_WINDOW(data)))
      return;

   ev = event_info;

   if (mdi_window->data->is_dragging)
   {
      etk_mdi_window_move(mdi_window,
                          ev->cur.canvas.x - mdi_window->data->drag_offset_x,
                          ev->cur.canvas.y - mdi_window->data->drag_offset_y);
   }
}

/* Called when the mouse pointer enters the resize-rect of the mdi_window's border */
static void _etk_mdi_window_resize_mouse_in_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
   Etk_Mdi_Window *mdi_window;

   if (!(mdi_window = ETK_MDI_WINDOW(data)))
      return;

   /* TODO change mouse pointer to ETK_POINTER_RESIZE_BR */
}

/* Called when the mouse pointer leaves the resize-rect of the mdi_window's border */
static void _etk_mdi_window_resize_mouse_out_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
   Etk_Mdi_Window *mdi_window;

   if (!(mdi_window = ETK_MDI_WINDOW(data)))
      return;

   /* TODO restore mouse pointer */
}

/* Called when the mouse presses the resize-rect of the mdi_window's border */
static void _etk_mdi_window_resize_mouse_down_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
   Etk_Mdi_Window *mdi_window;
   Evas_Event_Mouse_Down *ev;

   if (!(mdi_window = ETK_MDI_WINDOW(data)))
      return;

   ev = event_info;

   if (!mdi_window->data->maximized)
   {
      mdi_window->data->is_resizing = ETK_TRUE;
      mdi_window->data->resize_offset_x = ev->canvas.x - mdi_window->data->size.w;
      mdi_window->data->resize_offset_y = ev->canvas.y - mdi_window->data->size.h;
   }
}

/* Called when the mouse releases the resize-rect of the mdi_window's border */
static void _etk_mdi_window_resize_mouse_up_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
   Etk_Mdi_Window *mdi_window;

   if (!(mdi_window = ETK_MDI_WINDOW(data)))
      return;

   mdi_window->data->is_resizing = ETK_FALSE;
}

static void _etk_mdi_window_resize_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Mdi_Window *mdi_window;
   Evas_Event_Mouse_Move *ev;

   if (!(mdi_window = ETK_MDI_WINDOW(data)))
      return;

   ev = event_info;

   if (mdi_window->data->is_resizing)
   {
      etk_mdi_window_resize(mdi_window,
                            ev->cur.canvas.x - mdi_window->data->resize_offset_x,
                            ev->cur.canvas.y - mdi_window->data->resize_offset_y);
   }
}

/* Called when the mouse releases the maximize button */
static void _etk_mdi_window_maximize_mouse_up_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Mdi_Window *mdi_window;

   if (!(mdi_window = ETK_MDI_WINDOW(data)))
      return;

   etk_mdi_window_maximized_set(mdi_window, !etk_mdi_window_maximized_get(mdi_window));
   etk_mdi_window_raise(mdi_window);
}

/* Called when the mouse releases the close button */
static void _etk_mdi_window_close_mouse_up_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
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

static char *_etk_mdi_window_wm_theme_path_get(void)
{
   char *wm_theme_file;

   wm_theme_file = _etk_mdi_window_wm_theme_find(etk_config_wm_theme_get());
   if (!wm_theme_file)
   {
      /* fallback to default theme */
      wm_theme_file = _etk_mdi_window_wm_theme_find("default");
      if (!wm_theme_file)
      {
         return NULL;
      }
   }
   return wm_theme_file;
}

/* Finds the theme called "theme_name" in the subdir wm and returns its path, or NULL on failure */
static char *_etk_mdi_window_wm_theme_find(const char *theme_name)
{
   char path[PATH_MAX];
   char *home;

   if (!theme_name)
      return NULL;

   if ((home = getenv("HOME")))
   {
      snprintf(path, PATH_MAX, "%s/.e/etk/wm/%s.edj", home, theme_name);
      if (ecore_file_exists(path))
         return strdup(path);
   }

   snprintf(path, PATH_MAX, PACKAGE_DATA_DIR "/wm/%s.edj", theme_name);
   if (ecore_file_exists(path))
      return strdup(path);

   return NULL;
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
 * An Etk_Mdi_Window represents a top-level widget and consists of a title bar with window decorations and frame.
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Bin
 *         - Etk_Toplevel
 *           - Etk_Mdi_Window
 */
