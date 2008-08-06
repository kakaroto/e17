/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_mdi_window.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_mdi_window.h"

#include <stdlib.h>
#include <string.h>

#include <Edje.h>

#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_toplevel.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Mdi_Window
 * @{
 */

int ETK_MDI_WINDOW_MOVED_SIGNAL;
int ETK_MDI_WINDOW_DELETE_EVENT_SIGNAL;

enum Etk_Mdi_Window_Property_Id
{
   ETK_MDI_WINDOW_TITLE_PROPERTY,
   ETK_MDI_WINDOW_MAXIMIZED_PROPERTY,
   ETK_MDI_WINDOW_DRAGGABLE_PROPERTY,
   ETK_MDI_WINDOW_RESIZABLE_PROPERTY,
   ETK_MDI_WINDOW_DECORATED_PROPERTY
};

static void _etk_mdi_window_constructor(Etk_Mdi_Window *mdi_window);
static void _etk_mdi_window_destructor(Etk_Mdi_Window *mdi_window);
static void _etk_mdi_window_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_mdi_window_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static Etk_Bool _etk_mdi_window_realized_cb(Etk_Object *object, void *data);
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
      const Etk_Signal_Description signals[] = {
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_MDI_WINDOW_MOVED_SIGNAL,
            "moved", etk_marshaller_INT_INT),
         ETK_SIGNAL_DESC_HANDLER(ETK_MDI_WINDOW_DELETE_EVENT_SIGNAL,
            "delete-event", Etk_Mdi_Window, delete_event,
            etk_marshaller_VOID),
         ETK_SIGNAL_DESCRIPTION_SENTINEL
      };

      mdi_window_type = etk_type_new("Etk_Mdi_Window", ETK_BIN_TYPE,
         sizeof(Etk_Mdi_Window), ETK_CONSTRUCTOR(_etk_mdi_window_constructor),
         ETK_DESTRUCTOR(_etk_mdi_window_destructor), signals);

      etk_type_property_add(mdi_window_type, "title", ETK_MDI_WINDOW_TITLE_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(mdi_window_type, "maximized", ETK_MDI_WINDOW_MAXIMIZED_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(mdi_window_type, "draggable", ETK_MDI_WINDOW_DRAGGABLE_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(mdi_window_type, "resizable", ETK_MDI_WINDOW_RESIZABLE_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(mdi_window_type, "decorated", ETK_MDI_WINDOW_DECORATED_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));

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
 * and if all these callbacks return ETK_TRUE, the mdi_window will be destroyed. It has the same effect as if the user
 * had clicked on the "close" button of the mdi_window
 * @param mdi_window a mdi_window
 */
void etk_mdi_window_delete_request(Etk_Mdi_Window *mdi_window)
{
   Etk_Bool result;

   result = etk_signal_emit(ETK_MDI_WINDOW_DELETE_EVENT_SIGNAL, ETK_OBJECT(mdi_window));
   if (result)
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

/**
 * @brief Moves a mdi_window to the position (x, y)
 * @param mdi_window a mdi_window
 * @param x the x position where to move the mdi_window
 * @param y the y position where to move the mdi_window
 */
void etk_mdi_window_move(Etk_Mdi_Window *mdi_window, int x, int y)
{
   if (!mdi_window)
      return;

   etk_signal_emit(ETK_MDI_WINDOW_MOVED_SIGNAL, ETK_OBJECT(mdi_window), x, y);
}

/**
 * @brief Sets whether or not the mdi_window is maximized
 * @param mdi_window a mdi_window
 * @param maximized ETK_TRUE to maximize the mdi_window, ETK_FALSE to unmaximize it
 */
void etk_mdi_window_maximized_set(Etk_Mdi_Window *mdi_window, Etk_Bool maximized)
{
   if (!mdi_window || mdi_window->maximized == maximized)
      return;

   mdi_window->maximized = maximized;
   etk_object_notify(ETK_OBJECT(mdi_window), "maximized");
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
 * @brief Sets whether or not the mdi_window is draggable
 * @param mdi_window a mdi_window
 * @param draggable ETK_TRUE to enable drag on the mdi_window, ETK_FALSE otherwise
 */
void etk_mdi_window_draggable_set(Etk_Mdi_Window *mdi_window, Etk_Bool draggable)
{
   if (!mdi_window || mdi_window->draggable == draggable)
      return;

   mdi_window->dragging = ETK_FALSE;
   mdi_window->draggable = draggable;
   etk_object_notify(ETK_OBJECT(mdi_window), "draggable");
}

/**
 * @brief Gets whether the mdi_window is draggable
 * @param mdi_window a mdi_window
 * @return Returns ETK_TRUE if the mdi_window is draggable, ETK_FALSE otherwise
 */
Etk_Bool etk_mdi_window_draggable_get(Etk_Mdi_Window *mdi_window)
{
   if (!mdi_window)
      return ETK_FALSE;

   return mdi_window->draggable;
}

/**
 * @brief Sets whether or not the mdi_window is draggable
 * @param mdi_window a mdi_window
 * @param resizable ETK_TRUE to enable resize on the mdi_window, ETK_FALSE otherwise
 */
void etk_mdi_window_resizable_set(Etk_Mdi_Window *mdi_window, Etk_Bool resizable)
{
   if (!mdi_window || mdi_window->resizable == resizable)
      return;

   mdi_window->resizing = ETK_FALSE;
   mdi_window->resizable = resizable;
   /* make sure the pointer is correct */
   // etk_toplevel_pointer_pop(etk_widget_toplevel_parent_get(ETK_WIDGET(mdi_window)), ETK_POINTER_RESIZE_BR);
   etk_object_notify(ETK_OBJECT(mdi_window), "resizable");
}

/**
 * @brief Gets whether the mdi_window is resizable
 * @param mdi_window a mdi_window
 * @return Returns ETK_TRUE if the mdi_window is resizable, ETK_FALSE otherwise
 */
Etk_Bool etk_mdi_window_resizable_get(Etk_Mdi_Window *mdi_window)
{
   if (!mdi_window)
      return ETK_FALSE;

   return mdi_window->resizable;
}

/**
 * @brief Sets whether or not the mdi_window is decorated
 * @param mdi_window a mdi_window
 * @param decorated ETK_TRUE to enable decoration on the mdi_window, ETK_FALSE otherwise
 */
void etk_mdi_window_decorated_set(Etk_Mdi_Window *mdi_window, Etk_Bool decorated)
{
   if (!mdi_window || mdi_window->decorated == decorated)
      return;

   mdi_window->decorated = decorated;
   if (decorated)
      etk_widget_theme_group_set(ETK_WIDGET(mdi_window), "mdi_window");
   else
      etk_widget_theme_group_set(ETK_WIDGET(mdi_window), "mdi_window_borderless");
   etk_object_notify(ETK_OBJECT(mdi_window), "decorated");
}

/**
 * @brief Gets whether the mdi_window is decorated
 * @param mdi_window a mdi_window
 * @return Returns ETK_TRUE if the mdi_window is decorated, ETK_FALSE otherwise
 */
Etk_Bool etk_mdi_window_decorated_get(Etk_Mdi_Window *mdi_window)
{
   if (!mdi_window)
      return ETK_FALSE;

   return mdi_window->decorated;
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

   mdi_window->title = NULL;
   mdi_window->maximized = ETK_FALSE;
   mdi_window->draggable = ETK_TRUE;
   mdi_window->resizable = ETK_TRUE;
   mdi_window->decorated = ETK_TRUE;
   /* mdi_window->decorations = ETK_MDI_WINDOW_WM_DECORATION_ALL; */
   mdi_window->dragging = ETK_FALSE;
   mdi_window->resizing = ETK_FALSE;

   mdi_window->delete_event = _etk_mdi_window_delete_event_handler;

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(mdi_window), ETK_CALLBACK(_etk_mdi_window_realized_cb), NULL);
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
      case ETK_MDI_WINDOW_DRAGGABLE_PROPERTY:
         etk_mdi_window_draggable_set(mdi_window, etk_property_value_bool_get(value));
         break;
      case ETK_MDI_WINDOW_RESIZABLE_PROPERTY:
         etk_mdi_window_resizable_set(mdi_window, etk_property_value_bool_get(value));
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
         etk_property_value_string_set(value, mdi_window->title);
         break;
      case ETK_MDI_WINDOW_MAXIMIZED_PROPERTY:
         etk_property_value_bool_set(value, mdi_window->maximized);
         break;
      case ETK_MDI_WINDOW_DRAGGABLE_PROPERTY:
         etk_property_value_bool_set(value, mdi_window->draggable);
         break;
      case ETK_MDI_WINDOW_RESIZABLE_PROPERTY:
         etk_property_value_bool_set(value, mdi_window->resizable);
         break;
      case ETK_MDI_WINDOW_DECORATED_PROPERTY:
         etk_property_value_bool_set(value, mdi_window->decorated);
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
static Etk_Bool _etk_mdi_window_realized_cb(Etk_Object *object, void *data)
{
   Etk_Mdi_Window *mdi_window;
   Evas_Object *theme_object;
   Evas_Object *o;

   if (!(mdi_window = ETK_MDI_WINDOW(object)) || !(theme_object = ETK_WIDGET(mdi_window)->theme_object))
      return ETK_TRUE;

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

   return ETK_TRUE;
}

/* Called when the titlebar of the mdi_window is pressed */
static void _etk_mdi_window_titlebar_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Mdi_Window *mdi_window;
   Evas_Event_Mouse_Down *ev;

   if (!(mdi_window = ETK_MDI_WINDOW(data)) || !mdi_window->draggable)
      return;

   ev = event_info;

   if (!mdi_window->maximized)
   {
      int x, y;

      etk_widget_geometry_get(ETK_WIDGET(mdi_window), &x, &y, NULL, NULL);
      mdi_window->drag_offset_x = ev->canvas.x - x;
      mdi_window->drag_offset_y = ev->canvas.y - y;
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

   if (!(mdi_window = ETK_MDI_WINDOW(data)) || !mdi_window->dragging)
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

   if (!(mdi_window = ETK_MDI_WINDOW(data)) || mdi_window->maximized || !mdi_window->resizable)
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

   if (!(mdi_window = ETK_MDI_WINDOW(data)) || mdi_window->maximized || !mdi_window->resizable)
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

   if (!(mdi_window = ETK_MDI_WINDOW(data)) || !mdi_window->resizing)
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
   return ETK_TRUE;
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
 * \par
 * @prop_name "maximized": Whether or not the mdi_window is maximized
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 * \par
 * @prop_name "draggable": Whether or not the mdi_window is draggable
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_TRUE
 * @prop_name "resizable": Whether or not the mdi_window is resizable
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_TRUE
 * @prop_name "decorated": Whether or not the mdi_window is decorated
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_TRUE
 */
