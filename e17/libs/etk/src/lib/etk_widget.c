/** @file etk_widget.c */
#include "etk_widget.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <Evas.h>
#include <Edje.h>
#include "etk_theme.h"
#include "etk_main.h"
#include "etk_toplevel_widget.h"
#include "etk_container.h"
#include "etk_utils.h"
#include "etk_marshallers.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_drag.h"

/**
 * @addtogroup Etk_Widget
 * @{
 */

typedef struct Etk_Widget_Member_Object
{
   Evas_Object *object;
   Etk_Bool visible;
} Etk_Widget_Member_Object;

typedef struct Etk_Widget_Swallowed_Object
{
   char *part;
   Evas_Object *object;
   Etk_Widget *widget;
} Etk_Widget_Swallowed_Object;

enum Etk_Widget_Signal_Id
{
   ETK_WIDGET_SHOW_SIGNAL,
   ETK_WIDGET_HIDE_SIGNAL,
   ETK_WIDGET_REALIZE_SIGNAL,
   ETK_WIDGET_UNREALIZE_SIGNAL,
   ETK_WIDGET_SIZE_REQUEST_SIGNAL,
   ETK_WIDGET_MOUSE_IN_SIGNAL,
   ETK_WIDGET_MOUSE_OUT_SIGNAL,
   ETK_WIDGET_MOUSE_MOVE_SIGNAL,
   ETK_WIDGET_MOUSE_DOWN_SIGNAL,
   ETK_WIDGET_MOUSE_UP_SIGNAL,
   ETK_WIDGET_MOUSE_CLICK_SIGNAL,
   ETK_WIDGET_MOUSE_WHEEL_SIGNAL,
   ETK_WIDGET_KEY_DOWN_SIGNAL,
   ETK_WIDGET_KEY_UP_SIGNAL,
   ETK_WIDGET_ENTER_SIGNAL,
   ETK_WIDGET_LEAVE_SIGNAL,
   ETK_WIDGET_FOCUS_SIGNAL,
   ETK_WIDGET_UNFOCUS_SIGNAL,
   ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL,
   ETK_WIDGET_DRAG_DROP_SIGNAL,
   ETK_WIDGET_DRAG_ENTER_SIGNAL,     
   ETK_WIDGET_DRAG_MOTION_SIGNAL,
   ETK_WIDGET_DRAG_LEAVE_SIGNAL,
   ETK_WIDGET_DRAG_BEGIN_SIGNAL,     
   ETK_WIDGET_DRAG_END_SIGNAL,
   ETK_WIDGET_SELECTION_RECEIVED_SIGNAL,
   ETK_WIDGET_CLIPBOARD_RECEIVED_SIGNAL,
   ETK_WIDGET_NUM_SIGNALS
};

enum Etk_Widget_Property_Id
{
   ETK_WIDGET_NAME_PROPERTY,
   ETK_WIDGET_PARENT_PROPERTY,
   ETK_WIDGET_THEME_FILE_PROPERTY,
   ETK_WIDGET_THEME_GROUP_PROPERTY,
   ETK_WIDGET_THEME_PARENT_PROPERTY,
   ETK_WIDGET_GEOMETRY_PROPERTY,
   ETK_WIDGET_WIDTH_REQUEST_PROPERTY,
   ETK_WIDGET_HEIGHT_REQUEST_PROPERTY,
   ETK_WIDGET_VISIBLE_PROPERTY,
   ETK_WIDGET_VISIBILITY_LOCKED_PROPERTY,
   ETK_WIDGET_REPEAT_MOUSE_EVENTS_PROPERTY,
   ETK_WIDGET_PASS_MOUSE_EVENTS_PROPERTY,
   ETK_WIDGET_HAS_EVENT_OBJECT_PROPERTY,
   ETK_WIDGET_FOCUSABLE_PROPERTY,
   ETK_WIDGET_FOCUS_ON_PRESS_PROPERTY
};

static void _etk_widget_constructor(Etk_Widget *widget);
static void _etk_widget_destructor(Etk_Widget *widget);
static void _etk_widget_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_widget_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);

static void _etk_widget_show_handler(Etk_Widget *widget);
static void _etk_widget_enter_handler(Etk_Widget *widget);
static void _etk_widget_leave_handler(Etk_Widget *widget);
static void _etk_widget_focus_handler(Etk_Widget *widget);
static void _etk_widget_unfocus_handler(Etk_Widget *widget);

static void _etk_widget_drag_drop_handler(Etk_Widget *widget);
static void _etk_widget_drag_motion_handler(Etk_Widget *widget);
static void _etk_widget_drag_enter_handler(Etk_Widget *widget);
static void _etk_widget_drag_leave_handler(Etk_Widget *widget);
static void _etk_widget_drag_begin_handler(Etk_Widget *widget);
static void _etk_widget_drag_end_handler(Etk_Widget *widget);

static void _etk_widget_mouse_in_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_widget_signal_mouse_in_cb(Etk_Object *object, Etk_Event_Mouse_In_Out *event, void *data);
static void _etk_widget_mouse_out_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_widget_signal_mouse_out_cb(Etk_Object *object, Etk_Event_Mouse_In_Out *event, void *data);
static void _etk_widget_mouse_move_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_widget_mouse_down_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_widget_signal_mouse_down_cb(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data);
static void _etk_widget_mouse_up_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_widget_mouse_wheel_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_widget_signal_mouse_wheel_cb(Etk_Object *object, Etk_Event_Key_Up_Down *event, void *data);
static void _etk_widget_key_down_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_widget_signal_key_down_cb(Etk_Object *object, Etk_Event_Key_Up_Down *event, void *data);
static void _etk_widget_key_up_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_widget_signal_key_up_cb(Etk_Object *object, Etk_Event_Key_Up_Down *event, void *data);
static void _etk_widget_toplevel_evas_changed_cb(Etk_Object *object, const char *property_name, void *data);

static void _etk_widget_realize(Etk_Widget *widget);
static void _etk_widget_unrealize(Etk_Widget *widget);

static void _etk_widget_toplevel_parent_set(Etk_Widget *widget, Etk_Toplevel_Widget *toplevel_parent);
static void _etk_widget_realize_all(Etk_Widget *widget);
static void _etk_widget_unrealize_all(Etk_Widget *widget);
static void _etk_widget_realize_on_theme_file_change(Etk_Widget *widget, Etk_Bool force_realize);
static void _etk_widget_realize_on_theme_change(Etk_Widget *widget);

static void _etk_widget_theme_min_size_calc(Etk_Widget *widget, int *w, int *h);
static void _etk_widget_redraw_queue_recursive(Etk_Widget *widget);

static void _etk_widget_swallow_full(Etk_Widget *swallower, const char *part, Evas_Object *object, Etk_Widget *widget);
static void _etk_widget_unswallow_full(Etk_Widget *swallower, Evas_List *swo_node);

static void _etk_widget_child_remove(Etk_Widget *parent, Etk_Widget *child);
static void _etk_widget_object_add_to_smart(Etk_Widget *widget, Evas_Object *object, Etk_Bool clip);
static void _etk_widget_add_to_clip(Etk_Widget *widget, Evas_Object *clip);
static void _etk_widget_remove_from_clip(Etk_Widget *widget, Evas_Object *clip);

static Evas_List *_etk_widget_member_object_find(Etk_Widget *widget, Evas_Object *object);
static void _etk_widget_member_object_intercept_show_cb(void *data, Evas_Object *obj);
static void _etk_widget_member_object_intercept_hide_cb(void *data, Evas_Object *obj);

static void _etk_widget_member_object_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_widget_swallowed_object_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_widget_clip_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_widget_swallowed_widget_realize_cb(Etk_Object *object, void *data);

static Evas_Object *_etk_widget_smart_object_add(Evas *evas, Etk_Widget *widget);
static void _etk_widget_smart_object_move_cb(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _etk_widget_smart_object_resize_cb(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _etk_widget_smart_object_show_cb(Evas_Object *obj);
static void _etk_widget_smart_object_hide_cb(Evas_Object *obj);
static void _etk_widget_smart_object_clip_set_cb(Evas_Object *object, Evas_Object *clip);
static void _etk_widget_smart_object_clip_unset_cb(Evas_Object *object);
static void _etk_widget_dnd_drag_mouse_move_cb(Etk_Object *object, void *event, void *data);
static void _etk_widget_dnd_drag_end_cb(Etk_Object *object, void *data);  

static Etk_Signal *_etk_widget_signals[ETK_WIDGET_NUM_SIGNALS];
static Etk_Bool _etk_widget_intercept_show_hide = ETK_TRUE;
static Evas_Smart *_etk_widget_smart_object_smart = NULL;
static Etk_Widget_Swallow_Error _etk_widget_swallow_error = ETK_SWALLOW_ERROR_NONE;
static Evas_List *_etk_widget_dnd_dest_widgets = NULL;
static Evas_List *_etk_widget_dnd_source_widgets = NULL;
static Etk_Bool _etk_dnd_drag_start = ETK_TRUE;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Widget
 * @return Returns the type of an Etk_Widget
 */
Etk_Type *etk_widget_type_get()
{
   static Etk_Type *widget_type = NULL;

   if (!widget_type)
   {
      widget_type = etk_type_new("Etk_Widget", ETK_OBJECT_TYPE, sizeof(Etk_Widget),
         ETK_CONSTRUCTOR(_etk_widget_constructor), ETK_DESTRUCTOR(_etk_widget_destructor));

      _etk_widget_signals[ETK_WIDGET_SHOW_SIGNAL] = etk_signal_new("show",
         widget_type, ETK_MEMBER_OFFSET(Etk_Widget, show), etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_HIDE_SIGNAL] = etk_signal_new("hide",
         widget_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_REALIZE_SIGNAL] = etk_signal_new("realize",
         widget_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_UNREALIZE_SIGNAL] = etk_signal_new("unrealize",
         widget_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_SIZE_REQUEST_SIGNAL] = etk_signal_new("size_request",
         widget_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_MOUSE_IN_SIGNAL] = etk_signal_new("mouse_in",
         widget_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_MOUSE_OUT_SIGNAL] = etk_signal_new("mouse_out",
         widget_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_MOUSE_MOVE_SIGNAL] = etk_signal_new("mouse_move",
         widget_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_MOUSE_DOWN_SIGNAL] = etk_signal_new("mouse_down",
         widget_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_MOUSE_UP_SIGNAL] = etk_signal_new("mouse_up",
         widget_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_MOUSE_CLICK_SIGNAL] = etk_signal_new("mouse_click",
         widget_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_MOUSE_WHEEL_SIGNAL] = etk_signal_new("mouse_wheel",
         widget_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_KEY_DOWN_SIGNAL] = etk_signal_new("key_down",
         widget_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_KEY_UP_SIGNAL] = etk_signal_new("key_up",
         widget_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_ENTER_SIGNAL] = etk_signal_new("enter",
         widget_type, ETK_MEMBER_OFFSET(Etk_Widget, enter), etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_LEAVE_SIGNAL] = etk_signal_new("leave",
         widget_type, ETK_MEMBER_OFFSET(Etk_Widget, leave), etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_FOCUS_SIGNAL] = etk_signal_new("focus",
         widget_type, ETK_MEMBER_OFFSET(Etk_Widget, focus), etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_UNFOCUS_SIGNAL] = etk_signal_new("unfocus",
         widget_type, ETK_MEMBER_OFFSET(Etk_Widget, unfocus), etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL] = etk_signal_new("scroll_size_changed",
         widget_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_DRAG_DROP_SIGNAL] = etk_signal_new("drag_drop",
         widget_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_DRAG_MOTION_SIGNAL] = etk_signal_new("drag_motion",
         widget_type, ETK_MEMBER_OFFSET(Etk_Widget, drag_motion), etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_DRAG_ENTER_SIGNAL] = etk_signal_new("drag_enter",
         widget_type, ETK_MEMBER_OFFSET(Etk_Widget, drag_enter), etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_DRAG_LEAVE_SIGNAL] = etk_signal_new("drag_leave",
         widget_type, ETK_MEMBER_OFFSET(Etk_Widget, drag_leave), etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_DRAG_BEGIN_SIGNAL] = etk_signal_new("drag_begin",
         widget_type, ETK_MEMBER_OFFSET(Etk_Widget, drag_begin), etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_DRAG_END_SIGNAL] = etk_signal_new("drag_end",
         widget_type, ETK_MEMBER_OFFSET(Etk_Widget, drag_end), etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_SELECTION_RECEIVED_SIGNAL] = etk_signal_new("selection_received",
         widget_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_widget_signals[ETK_WIDGET_CLIPBOARD_RECEIVED_SIGNAL] = etk_signal_new("clipboard_received",
         widget_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      
      etk_type_property_add(widget_type, "name", ETK_WIDGET_NAME_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(widget_type, "parent", ETK_WIDGET_PARENT_PROPERTY,
         ETK_PROPERTY_POINTER, ETK_PROPERTY_READABLE, etk_property_value_pointer(NULL));
      etk_type_property_add(widget_type, "theme_file", ETK_WIDGET_THEME_FILE_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(widget_type, "theme_group", ETK_WIDGET_THEME_GROUP_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(widget_type, "theme_parent", ETK_WIDGET_THEME_PARENT_PROPERTY,
         ETK_PROPERTY_POINTER, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_pointer(NULL));
      etk_type_property_add(widget_type, "geometry", ETK_WIDGET_GEOMETRY_PROPERTY,
         ETK_PROPERTY_OTHER, ETK_PROPERTY_NO_ACCESS, NULL);
      etk_type_property_add(widget_type, "width_request", ETK_WIDGET_WIDTH_REQUEST_PROPERTY, 
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(-1));
      etk_type_property_add(widget_type, "height_request", ETK_WIDGET_HEIGHT_REQUEST_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(-1));
      etk_type_property_add(widget_type, "visible", ETK_WIDGET_VISIBLE_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(widget_type, "visibility_locked", ETK_WIDGET_VISIBILITY_LOCKED_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(widget_type, "focusable", ETK_WIDGET_FOCUSABLE_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(widget_type, "focus_on_press", ETK_WIDGET_FOCUS_ON_PRESS_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(widget_type, "repeat_mouse_events", ETK_WIDGET_REPEAT_MOUSE_EVENTS_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(widget_type, "pass_mouse_events", ETK_WIDGET_PASS_MOUSE_EVENTS_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(widget_type, "has_event_object", ETK_WIDGET_HAS_EVENT_OBJECT_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));

      widget_type->property_set = _etk_widget_property_set;
      widget_type->property_get = _etk_widget_property_get;
   }

   return widget_type;
}

/**
 * @brief Creates a new widget: it calls the corresponding constructors (from the constructor of the base class to the
 * constructor of the more derived class) and then sets the values of the given properties
 * @param widget_type the type of the widget to create
 * @param first_property the name of the first property to set
 * @param ... the value of the first property, followed by any number of property-name/property-value pairs,
 * terminated with NULL
 * @return Returns the new Etk_Widget of type @a widget_type
 * @note This function just call etk_object_new() and casts the result to an "Etk_Widget *"
 */
Etk_Widget *etk_widget_new(Etk_Type *widget_type, const char *first_property, ...)
{
   Etk_Widget *new_widget;
   va_list args;

   if (!widget_type)
      return NULL;

   va_start(args, first_property);
   new_widget = ETK_WIDGET(etk_object_new_valist(widget_type, first_property, args));
   va_end(args);

   return new_widget;
}

/**
 * @brief Sets the name of the widget
 * @param widget a widget
 * @param name the name to set
 */
void etk_widget_name_set(Etk_Widget *widget, const char *name)
{
   if (!widget || widget->name == name)
      return;

   free(widget->name);
   widget->name = strdup(name);
   etk_object_notify(ETK_OBJECT(widget), "name");
}

/**
 * @brief Gets the name of the widget
 * @param widget a widget
 * @return Returns the name of the widget
 */
const char *etk_widget_name_get(Etk_Widget *widget)
{
   if (!widget)
      return NULL;
   return widget->name;
}

/**
 * @brief Gets the geometry of the widget, relative to the top left corner of the Evas where it is drawn
 * @param widget a widget
 * @param x the location where to store the x position of the widget
 * @param y the location where to store the y position of the widget
 * @param w the location where to store the width of the widget
 * @param h the location where to store the height of the widget
 */
void etk_widget_geometry_get(Etk_Widget *widget, int *x, int *y, int *w, int *h)
{
   if (x)   *x = widget ? widget->geometry.x : 0;
   if (y)   *y = widget ? widget->geometry.y : 0;
   if (w)   *w = widget ? widget->geometry.w : 0;
   if (h)   *h = widget ? widget->geometry.h : 0;
}

/**
 * @brief Gets the inner geometry of the widget, relative to the top left corner of the Evas where it is drawn. @n
 * The inner geometry takes the inset values (horizontal and vertical paddings) into account (inset values are defined
 * by the theme)
 * @param widget a widget
 * @param x the location where to store the inner x position of the widget
 * @param y the location where to store the inner y position of the widget
 * @param w the location where to store the inner width of the widget
 * @param h the location where to store the inner height of the widget
 */
void etk_widget_inner_geometry_get(Etk_Widget *widget, int *x, int *y, int *w, int *h)
{
   if (x)   *x = widget ? widget->inner_geometry.x : 0;
   if (y)   *y = widget ? widget->inner_geometry.y : 0;
   if (w)   *w = widget ? widget->inner_geometry.w : 0;
   if (h)   *h = widget ? widget->inner_geometry.h : 0;
}

/**
 * @brief Gets the toplevel widget that contains @a widget
 * @param widget a widget
 * @return Returns the toplevel widget if @a widget is contained in a toplevel widget, NULL on failure
 */
Etk_Toplevel_Widget *etk_widget_toplevel_parent_get(Etk_Widget *widget)
{
   if (!widget)
      return NULL;
   return widget->toplevel_parent;
}

/**
 * @brief Gets the evas of the toplevel widget that contains @a widget
 * @param widget a widget
 * @return Returns the evas if @a widget is contained in a toplevel widget, or NULL on failure
 */
Evas *etk_widget_toplevel_evas_get(Etk_Widget *widget)
{
   if (!widget || !widget->toplevel_parent)
      return NULL;
   return etk_toplevel_widget_evas_get(widget->toplevel_parent);
}

/**
 * @brief Sets the theme-file of the widget
 * @param widget a widget
 * @param theme_file the path of the .edj theme-file. If NULL, it will use the theme-file of its theme-parent
 * (or parent if is has no theme-parent). If none of its theme-parents has a non-null theme file, it will use the
 * current theme-file of Etk. @n
 * Children of the widget will automatically inherit this theme-file if they have no theme-file already set
 */
void etk_widget_theme_file_set(Etk_Widget *widget, const char *theme_file)
{
   if (!widget)
      return;

   if (theme_file != widget->theme_file)
   {
      free(widget->theme_file);
      widget->theme_file = theme_file ? strdup(theme_file) : NULL;
      etk_object_notify(ETK_OBJECT(widget), "theme_file");
      /* TODO: _realize_on_theme_file_change(): this function name is too loooonggg! */
      _etk_widget_realize_on_theme_file_change(widget, ETK_TRUE);
   }
}

/**
 * @brief Gets the theme-file of the widget. See etk_widget_theme_file_set() for more infos about theme-file
 * @param widget a widget
 * @return Returns the theme-file used by the widget (if NULL, it uses the theme-file of its parent)
 * @note To get the theme-file really used by the widget, you have to walk through the theme-parents of the widget @n
 * until you find the first theme-parent that uses a non-null theme-file. This is the theme-file used of the widget. @n
 * If no theme-parent has a non-null theme-file, then it uses the current theme-file of Etk, that you can get with
 * etk_theme_widget_theme_get()
 */
const char *etk_widget_theme_file_get(Etk_Widget *widget)
{
   if (!widget)
      return NULL;
   return widget->theme_file;
}

/**
 * @brief Sets the theme-group of the widget. The theme-group regroups all the theme parts of a logical group of widgets. @n
 * For example, the theme-group for an Etk_Tree is "tree". It regroups the theme parts for the tree background,
 * for the tree rows, for the expanders... So changing the theme-group of a widget will change the whole appearance of
 * the widget including the appearance of the children of the widgets, if they use the same theme-group.
 * @param widget a widget
 * @param theme_group the theme-group to use
 * @note Widgets will automatically inherit the theme-group from their theme-parent
 */
void etk_widget_theme_group_set(Etk_Widget *widget, const char *theme_group)
{
   if (!widget)
      return;

   if (theme_group != widget->theme_group)
   {
      free(widget->theme_group);
      widget->theme_group = theme_group ? strdup(theme_group) : NULL;
      etk_object_notify(ETK_OBJECT(widget), "theme_group");
      _etk_widget_realize_on_theme_change(widget);
   }
}

/**
 * @brief Gets the theme-group of the widget. See etk_widget_theme_group_set() for more infos about theme-group
 * @param widget a widget
 * @return Returns the theme-group of the widget
 */
const char *etk_widget_theme_group_get(Etk_Widget *widget)
{
   if (!widget)
      return NULL;
   return widget->theme_group;
}

/**
 * @brief Sets the theme-parent of the widget. The widget will use the theme-file and theme-group of its theme-parent
 * @param widget a widget
 * @param theme_parent The theme-parent to use
 * @note By default, a widget uses its parent as its theme-parent
 * @widget_implementation
 */
void etk_widget_theme_parent_set(Etk_Widget *widget, Etk_Widget *theme_parent)
{
   if (!widget || widget->theme_parent == theme_parent)
      return;
   
   if (widget->theme_parent)
      widget->theme_parent->theme_children = evas_list_remove(widget->theme_parent->theme_children, widget);
   
   if (theme_parent)
      theme_parent->theme_children = evas_list_append(theme_parent->theme_children, widget);
   widget->theme_parent = theme_parent;
   
   _etk_widget_realize_on_theme_change(widget);
}

/**
 * @brief Gets the theme-parent of the widget
 * @param widget a widget
 * @return Returns the theme-parent of the widget. If it returns NULL, it means the widget inherits its theme-file and
 * its theme-group from its "normal" parent (i.e. the widget that contains it)
 */
Etk_Widget *etk_widget_theme_parent_get(Etk_Widget *widget)
{
   if (!widget)
      return NULL;
   return widget->theme_parent;
}

/**
 * @brief Sets the parent of the widget
 * @param widget a widget
 * @param parent the new parent
 * @note etk_widget_parent_set(widget, parent) is equivalent to etk_widget_parent_set_full(widget, parent, ETK_TRUE)
 * @widget_implementation
 * @note If you want to add a widget to a container, use etk_container_add() instead!
 */
void etk_widget_parent_set(Etk_Widget *widget, Etk_Widget *parent)
{
   etk_widget_parent_set_full(widget, parent, ETK_TRUE);
}

/**
 * @brief Sets the parent of the widget
 * @param widget a widget
 * @param parent the new parent
 * @param remove_from_container if @a remove_from_container is ETK_TRUE and if the parent of the widget is a container,
 * the remove_child() function of the container parent will be called. So @a remove_from_container should most of the
 * time be set to ETK_TRUE, except when etk_widget_parent_set_full() is called from the remove_child() function of a
 * container, in order to avoid an infinite loop.
 * @widget_implementation
 * @note If you want to add a widget to a container, use etk_container_add() instead!
 */
void etk_widget_parent_set_full(Etk_Widget *widget, Etk_Widget *parent, Etk_Bool remove_from_container)
{
   Etk_Toplevel_Widget *previous_toplevel;
   Evas *previous_evas, *new_evas;
   Etk_Widget *toplevel;

   if (!widget || widget->parent == parent)
      return;
   
   if (widget->parent)
   {
      if (remove_from_container && ETK_IS_CONTAINER(widget->parent))
         etk_container_remove(ETK_CONTAINER(widget->parent), widget);
      _etk_widget_child_remove(widget->parent, widget);
   }
   if (parent)
      parent->children = evas_list_append(parent->children, widget);
   widget->parent = parent;

   previous_toplevel = widget->toplevel_parent;
   for (toplevel = widget; toplevel->parent; toplevel = toplevel->parent);
   if (ETK_IS_TOPLEVEL_WIDGET(toplevel))
      widget->toplevel_parent = ETK_TOPLEVEL_WIDGET(toplevel);
   else
      widget->toplevel_parent = NULL;

   if ((widget->toplevel_parent != previous_toplevel))
      _etk_widget_toplevel_parent_set(widget, widget->toplevel_parent);

   previous_evas = etk_toplevel_widget_evas_get(previous_toplevel);
   new_evas = etk_widget_toplevel_evas_get(widget);
   
   /* Realize/unrealize the widget and its children */
   /* TODO: optimize this? */
   if (new_evas)
      _etk_widget_realize_on_theme_change(widget);
   else if (widget->realized)
      _etk_widget_unrealize_all(widget);

   etk_object_notify(ETK_OBJECT(widget), "parent");
}

/**
 * @brief Gets the parent of the widget
 * @param widget a widget
 * @return Returns the parent of the widget
 */
Etk_Widget *etk_widget_parent_get(Etk_Widget *widget)
{
   if (!widget)
      return NULL;
   return widget->parent;
}

/**
 * @brief Sets whether the widget should have an event object. An event object is a simple invisible rectangle that
 * grabs the mouse events. It can be useful for example if you want to know when a container with no theme object
 * (a table, a box, ...) is clicked. @n
 * If a widget already has a theme object, then this function has no effect since the theme object is already used to
 * grab the mouse events
 * @param widget a widget
 * @param has_event_object ETK_TRUE to add an event object to the widget, ETK_FALSE to remove it
 */
void etk_widget_has_event_object_set(Etk_Widget *widget, Etk_Bool has_event_object)
{
   Evas *evas;
   
   if (!widget || widget->has_event_object == has_event_object)
      return;
   
   widget->has_event_object = has_event_object;
   
   if (has_event_object)
   {
      if (!widget->theme_object && (evas = etk_widget_toplevel_evas_get(widget)))
      {
         widget->event_object = evas_object_rectangle_add(evas);
         evas_object_color_set(widget->event_object, 255, 255, 255, 0);
         evas_object_show(widget->event_object);
         _etk_widget_object_add_to_smart(widget, widget->event_object, ETK_TRUE);
         evas_object_lower(widget->event_object);
         etk_widget_redraw_queue(widget);
      }
   }
   else if (widget->event_object)
   {
      evas_object_del(widget->event_object);
      widget->event_object = NULL;
   }
   
   etk_object_notify(ETK_OBJECT(widget), "has_event_object");
}

/**
 * @brief Gets whether the widget has an event object
 * @param widget a widget
 * @return Returns ETK_TRUE if the widget has an event object
 */
Etk_Bool etk_widget_has_event_object_get(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->has_event_object;
}

/**
 * @brief Sets whether the widget should repeat the mouse events it receives
 * @param widget a widget
 * @param repeat_mouse_events if @a repeat_mouse_events is ETK_TRUE, the parent widget will also receive the mouse events
 */
void etk_widget_repeat_mouse_events_set(Etk_Widget *widget, Etk_Bool repeat_mouse_events)
{
   if (!widget)
      return;

   /* TODO: Should it affect mouse wheel events? */
   widget->repeat_mouse_events = repeat_mouse_events;
   if (widget->smart_object)
      evas_object_repeat_events_set(widget->smart_object, repeat_mouse_events);
   etk_object_notify(ETK_OBJECT(widget), "repeat_mouse_events");
}

/**
 * @brief Checks whether the widget repeats the mouse events it receives
 * @param widget a widget
 * @return Returns ETK_TRUE if the widget repeats the mouse events it receives
 */
Etk_Bool etk_widget_repeat_mouse_events_get(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->repeat_mouse_events;
}

/**
 * @brief Sets whether the widget should pass the mouse events it receives to its parent
 * @param widget a widget
 * @param pass_mouse_events if @a pass_mouse_events is ETK_TRUE, the mouse events will be directly sent to the parent.
 * @a widget won't receive the mouse events anymore
 */
void etk_widget_pass_mouse_events_set(Etk_Widget *widget, Etk_Bool pass_mouse_events)
{
   if (!widget)
      return;

   /* TODO: Should it affect mouse wheel events? */
   widget->pass_mouse_events = pass_mouse_events;
   if (widget->smart_object)
      evas_object_pass_events_set(widget->smart_object, pass_mouse_events);
   etk_object_notify(ETK_OBJECT(widget), "pass_mouse_events");
}

/**
 * @brief Checks whether the widget passes the mouse events it receives to its parent
 * @param widget a widget
 * @return Returns ETK_TRUE if the widget passes the mouse events it receives
 */
Etk_Bool etk_widget_pass_mouse_events_get(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->pass_mouse_events;
}

/**
 * @brief Shows the widget
 * @param widget a widget
 * @note The widget will effectively be shown if all its parents are also shown
 */
void etk_widget_show(Etk_Widget *widget)
{
   Etk_Widget *parent;
   
   if (!widget || widget->visible)
      return;

   parent = widget->parent;
   widget->visible = ETK_TRUE;
   if (widget->smart_object && (!parent || (parent->smart_object && evas_object_visible_get(parent->smart_object))))
      evas_object_show(widget->smart_object);
   etk_widget_size_recalc_queue(widget);
   etk_object_notify(ETK_OBJECT(widget), "visible");
   
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_SHOW_SIGNAL], ETK_OBJECT(widget), NULL);
}

/**
 * @brief Recursively shows the widget and its children
 * @param widget a widget
 */
void etk_widget_show_all(Etk_Widget *widget)
{
   Evas_List *l;
   
   if (!widget)
      return;

   if (!widget->visibility_locked)
     etk_widget_show(widget);
   for (l = widget->children; l; l = l->next)
      etk_widget_show_all(ETK_WIDGET(l->data));
}

/**
 * @brief Hides the widget
 * @param widget a widget
 */
void etk_widget_hide(Etk_Widget *widget)
{
   if (!widget || !widget->visible)
      return;

   widget->visible = ETK_FALSE;
   if (widget->smart_object)
      evas_object_hide(widget->smart_object);
   etk_widget_size_recalc_queue(widget);
   etk_object_notify(ETK_OBJECT(widget), "visible");
   
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_HIDE_SIGNAL], ETK_OBJECT(widget), NULL);
}

/**
 * @brief Recursively hides the widget and its children
 * @param widget a widget
 */
void etk_widget_hide_all(Etk_Widget *widget)
{
   Evas_List *l;
   
   if (!widget)
      return;

   if (!widget->visibility_locked)
      etk_widget_hide(widget);
   for (l = widget->children; l; l = l->next)
      etk_widget_hide_all(ETK_WIDGET(l->data));
}

/**
 * @brief Checks if the object is visible
 * @param widget a widget
 * @return Returns ETK_TRUE if the widget is visible, ETK_FALSE otherwise
 */
Etk_Bool etk_widget_is_visible(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->visible;
}

/**
 * @brief Sets whether the widget should be affected by etk_widget_show_all() and etk_widget_hide_all()
 * @param widget a widget
 * @param visibility_locked has to be set to ETK_TRUE to prevent @a widget to be affected by
 * etk_widget_show_all() and etk_widget_hide_all()
 * @widget_implementation
 */
void etk_widget_visibility_locked_set(Etk_Widget *widget, Etk_Bool visibility_locked)
{
   if (!widget || widget->visibility_locked == visibility_locked)
      return;
   widget->visibility_locked = visibility_locked;
   etk_object_notify(ETK_OBJECT(widget), "visibility_locked");
}

/**
 * @brief Gets whether the widget is affected by etk_widget_show_all() and etk_widget_hide_all()
 * @param widget a widget
 * @return Returns ETK_TRUE if etk_widget_show_all() and etk_widget_hide_all() does not affect the visibility of
 * @a widget, ETK_FALSE otherwise
 */
Etk_Bool etk_widget_visibility_locked_get(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->visibility_locked;
}

/**
 * @brief Raises a widget: it will be moved above all the other widgets that have the same parent as @a widget
 * @param widget the widget to raise
 */
void etk_widget_raise(Etk_Widget *widget)
{
   Evas_List *l;
   
   if (!widget || !widget->parent)
      return;
   
   if ((l = evas_list_find_list(widget->parent->children, widget)))
   {
      widget->parent->children = evas_list_remove_list(widget->parent->children, l);
      widget->parent->children = evas_list_append(widget->parent->children, widget);
   }
   
   if (widget->smart_object)
      evas_object_raise(widget->smart_object);
}

/**
 * @brief Lowers a widget: it will be moved below all the other widgets that have the same parent as @a widget
 * @param widget the widget to lower
 */
void etk_widget_lower(Etk_Widget *widget)
{
   Evas_List *l;
   
   if (!widget || !widget->parent)
      return;
   
   if ((l = evas_list_find_list(widget->parent->children, widget)))
   {
      widget->parent->children = evas_list_remove_list(widget->parent->children, l);
      widget->parent->children = evas_list_prepend(widget->parent->children, widget);
   }
   
   if (widget->smart_object)
      evas_object_lower(widget->smart_object);
}

/**
 * @brief Queues a size recalculation request: during the next main-loop iteration, the widget size will be
 * recalculated
 * @param widget the widget to queue
 * @widget_implementation
 */
void etk_widget_size_recalc_queue(Etk_Widget *widget)
{
   Etk_Widget *w;

   if (!widget)
      return;

   /* TODO: should etk_widget_size_recalc_queue() calls etk_widget_redraw_queue() ? */
   for (w = widget; w; w = ETK_WIDGET(w->parent))
   {
      if (w->swallowed && w->parent)
         w->parent->need_theme_min_size_recalc = ETK_TRUE;
      w->need_size_recalc = ETK_TRUE;
      w->need_redraw = ETK_TRUE;
   }

   if (widget->toplevel_parent)
      etk_main_iteration_queue();
}

/**
 * @brief Queues a redraw request: during the next mainloop iteration, the widget and its children will be redrawn
 * (i.e. etk_widget_size_allocate() will be called)
 * @param widget the widget to queue
 * @widget_implementation
 */
void etk_widget_redraw_queue(Etk_Widget *widget)
{
   if (!widget)
      return;

   _etk_widget_redraw_queue_recursive(widget);
   if (widget->toplevel_parent)
      etk_main_iteration_queue();
}

/**
 * @brief Set the ideal size of the widget. Use this function to ask Etk to resize the widget to the given size.
 * It does not mean the widget will have this size: it can still be smaller or bigger. @a
 * The given size will be returned by etk_widget_size_request(), instead of calculating it
 * @param widget a widget
 * @param w the ideal width (-1 will make Etk calculate it automatically)
 * @param h the ideal height (-1 will make Etk calculate it automatically)
 */
void etk_widget_size_request_set(Etk_Widget *widget, int w, int h)
{
   if (!widget)
      return;

   widget->requested_size.w = w;
   widget->requested_size.h = h;
   
   etk_object_notify(ETK_OBJECT(widget), "width_request");
   etk_object_notify(ETK_OBJECT(widget), "height_request");
   etk_widget_size_recalc_queue(widget);
}

/**
 * @brief Gets the ideal size of the widget
 * @param widget a widget
 * @param size_requisition the location where to store the ideal size
 * @note etk_widget_size_request_full(widget, size_req) is equivalent to
 * etk_widget_size_request_full(widget, size_req, ETK_TRUE)
 * @widget_implementation
 */
void etk_widget_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   etk_widget_size_request_full(widget, size_requisition, ETK_TRUE);
}

/**
 * @brief Gets the ideal size of the widget
 * @param widget a widget
 * @param size_requisition the location where to store the ideal size
 * @param hidden_has_no_size if @a hidden_has_no_size is ETK_TRUE, then if the widget is hidden,
 * the returned ideal size will be 0x0
 * @widget_implementation
 */
void etk_widget_size_request_full(Etk_Widget *widget, Etk_Size *size_requisition, Etk_Bool hidden_has_no_size)
{
   if (!widget || !size_requisition)
      return;

   size_requisition->w = -1;
   size_requisition->h = -1;

   if (!widget->visible && hidden_has_no_size)
      size_requisition->w = 0;
   else if (!widget->need_size_recalc && widget->last_size_requisition.w >= 0)
      size_requisition->w = widget->last_size_requisition.w;
   else if (widget->requested_size.w >= 0)
      size_requisition->w = widget->requested_size.w;
   
   if (!widget->visible && hidden_has_no_size)
      size_requisition->h = 0;
   else if (!widget->need_size_recalc && widget->last_size_requisition.h >= 0)
      size_requisition->h = widget->last_size_requisition.h;
   else if (widget->requested_size.h >= 0)
      size_requisition->h = widget->requested_size.h;

   /* We need to recalc it */
   if (size_requisition->w < 0 || size_requisition->h < 0)
   {
      int min_w, min_h;

      _etk_widget_theme_min_size_calc(widget, &min_w, &min_h);
      if (widget->size_request && widget->realized)
      {
         Etk_Size widget_requisition;

         widget->size_request(widget, &widget_requisition);
         if (size_requisition->w < 0)
            size_requisition->w = ETK_MAX(min_w, widget_requisition.w + widget->left_inset + widget->right_inset);
         if (size_requisition->h < 0)
            size_requisition->h = ETK_MAX(min_h, widget_requisition.h + widget->top_inset + widget->bottom_inset);
      }
      else
      {
         if (size_requisition->w < 0)
            size_requisition->w = min_w;
         if (size_requisition->h < 0)
            size_requisition->h = min_h;
      }
   }

   if (widget->visible || !hidden_has_no_size)
      widget->last_size_requisition = *size_requisition;
   widget->need_size_recalc = ETK_FALSE;

   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_SIZE_REQUEST_SIGNAL], ETK_OBJECT(widget), NULL, size_requisition);
}

/**
 * @brief Moves and resizes the widget to the given geometry
 * @param widget a widget
 * @param geometry the geometry the widget should occupy.
 * The position should be relative to the Evas where the widget is drawn.
 * @widget_implementation
 */
void etk_widget_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   if (!widget || widget->swallowed || !widget->smart_object)
      return;

   if (geometry.x != widget->geometry.x || geometry.y != widget->geometry.y || widget->need_redraw)
      evas_object_move(widget->smart_object, geometry.x, geometry.y);
   if (geometry.w != widget->geometry.w || geometry.h != widget->geometry.h || widget->need_redraw)
      evas_object_resize(widget->smart_object, geometry.w, geometry.h);
}

/**
 * @brief Emits the "enter" signal on the widget
 * @param widget a widget
 */
void etk_widget_enter(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_ENTER_SIGNAL], ETK_OBJECT(widget), NULL);
}

/**
 * @brief Emits the "leave" signal on the widget
 * @param widget a widget
 */
void etk_widget_leave(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_LEAVE_SIGNAL], ETK_OBJECT(widget), NULL);
}

/**
 * @brief Focuses the widget. The widget has to be attached to a toplevel widget, otherwise it will have no effect!
 * @param widget a widget
 */
void etk_widget_focus(Etk_Widget *widget)
{
   Etk_Widget *focused;

   if (!widget || !widget->toplevel_parent || !widget->focusable)
      return;
   if ((focused = etk_toplevel_widget_focused_widget_get(widget->toplevel_parent)) && (widget == focused))
      return;

   if (focused)
      etk_widget_unfocus(focused);

   etk_toplevel_widget_focused_widget_set(widget->toplevel_parent, widget);
   /* TODO: Should we test if the toplevel widget is focused too before doing this? */
   if (widget->smart_object)
      evas_object_focus_set(widget->smart_object, 1);
   
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_FOCUS_SIGNAL], ETK_OBJECT(widget), NULL);
}

/**
 * @brief Unfocuses the widget
 * @param widget a widget
 */
void etk_widget_unfocus(Etk_Widget *widget)
{
   Etk_Widget *focused;

   if (!widget || !widget->toplevel_parent || !(focused = etk_toplevel_widget_focused_widget_get(widget->toplevel_parent)) || (focused != widget))
      return;

   etk_toplevel_widget_focused_widget_set(widget->toplevel_parent, NULL);
   /* TODO: make sure a widget is always focused (or focus the toplevel widget) (on delete too) */
   if (widget->smart_object)
      evas_object_focus_set(widget->smart_object, 0);
   
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_UNFOCUS_SIGNAL], ETK_OBJECT(widget), NULL);
}

/**
 * @brief Makes the widget swallow another widget in a part of its theme object
 * @param swallower the widget that will swallow the widget @a to_swallow. @a swallower has to
 * be realized and to have a theme object
 * @param part the name of the part of the theme object in which @a to_swallow should be swallowed
 * @param to_swallow the widget to swallow. @a to_swallow has to be a child of @a swallower
 * @return Returns ETK_TRUE on success, ETK_FALSE on failure (it may occur if @a swallower is not realized, if the part
 * doesn't exist or if @a to swallow is not a child of @a swallower)
 * @widget_implementation
 */
Etk_Bool etk_widget_swallow_widget(Etk_Widget *swallower, const char *part, Etk_Widget *to_swallow)
{
   if (!swallower || !part || !to_swallow)
      return ETK_FALSE;
   if (!(swallower->theme_object))
   {
      _etk_widget_swallow_error = ETK_SWALLOW_ERROR_NOT_REALIZED;
      return ETK_FALSE;
   }
   if (to_swallow->parent != swallower)
   {
      _etk_widget_swallow_error = ETK_SWALLOW_ERROR_INCOMPATIBLE_PARENT;
      return ETK_FALSE;
   }
   if (!edje_object_part_exists(swallower->theme_object, part))
   {
      _etk_widget_swallow_error = ETK_SWALLOW_ERROR_NO_PART;
      return ETK_FALSE;
   }

   if (to_swallow->swallowed)
      etk_widget_unswallow_widget(to_swallow->parent, to_swallow);

   to_swallow->swallowed = ETK_TRUE;
   _etk_widget_swallow_full(swallower, part, to_swallow->smart_object, to_swallow);
   
   _etk_widget_swallow_error = ETK_SWALLOW_ERROR_NONE;
   return ETK_TRUE;
}

/**
 * @brief Makes the widget unswallow another widget
 * @param swallower the widget that currently swallow @a swallowed
 * @param swallowed the widget currently swallowed by @a swallower, and that you want to be unswallowed
 * @widget_implementation
 */
void etk_widget_unswallow_widget(Etk_Widget *swallower, Etk_Widget *swallowed)
{
   Evas_List *l;
   Etk_Widget_Swallowed_Object *swo;
   
   if (!swallower || !swallowed)
      return;
   
   for (l = swallower->swallowed_objects; l; l = l->next)
   {
      swo = l->data;
      if (swo->widget == swallowed)
      {
         _etk_widget_unswallow_full(swallower, l);
         break;
      }
   }
}

/**
 * @brief Checks if @a swallower is currently swallowing @a swallowed
 * @param swallower a widget
 * @param swallowed the widget to check if it is swallowed by @a swallower
 * @return Returns ETK_TRUE if @a swallower is swallowing @a swallowed
 * @widget_implementation
 */
Etk_Bool etk_widget_is_swallowing_widget(Etk_Widget *swallower, Etk_Widget *swallowed)
{
   Evas_List *l;
   Etk_Widget_Swallowed_Object *swo;
   
   if (!swallower || !swallowed)
      return ETK_FALSE;
   
   for (l = swallower->swallowed_objects; l; l = l->next)
   {
      swo = l->data;
      if (swo->widget == swallowed)
         return ETK_TRUE;
   }
   return ETK_FALSE;
}

/**
 * @brief Checks if the widget is swallowed by its parent
 * @return Returns ETK_TRUE if the widget is swallowed by its parent
 * @widget_implementation
 */
Etk_Bool etk_widget_is_swallowed(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->swallowed;
}

/**
 * @brief Makes the widget swallow an Evas_Object in a part of its theme object
 * @param swallower the widget that will swallow the object @a to_swallow. @a swallower has to
 * be realized and to have a theme object
 * @param part the name of the part of the theme object in which @a to_swallow should be swallowed
 * @param to_swallow the Evas_Object to swallow
 * @return Returns ETK_TRUE on success, ETK_FALSE on failure (it may occur if @a swallower is not realized
 * or if the part doesn't exist)
 * @widget_implementation
 */
Etk_Bool etk_widget_swallow_object(Etk_Widget *swallower, const char *part, Evas_Object *to_swallow)
{
   if (!swallower || !part || !to_swallow)
      return ETK_FALSE;
   if (!(swallower->theme_object))
   {
      _etk_widget_swallow_error = ETK_SWALLOW_ERROR_NOT_REALIZED;
      return ETK_FALSE;
   }
   if (!edje_object_part_exists(swallower->theme_object, part))
   {
      _etk_widget_swallow_error = ETK_SWALLOW_ERROR_NO_PART;
      return ETK_FALSE;
   }
   
   _etk_widget_swallow_full(swallower, part, to_swallow, NULL);
   _etk_widget_swallow_error = ETK_SWALLOW_ERROR_NONE;
   return ETK_TRUE;
}

/**
 * @brief Makes the widget unswallow an Evas_Object
 * @param swallower the widget that currently swallow @a object
 * @param object the Evas_Object currently swallowed by @a swallower, and that you want to be unswallowed
 * @widget_implementation
 */
void etk_widget_unswallow_object(Etk_Widget *swallower, Evas_Object *object)
{
   Evas_List *l;
   Etk_Widget_Swallowed_Object *swo;
   
   if (!swallower || !object)
      return;
   
   for (l = swallower->swallowed_objects; l; l = l->next)
   {
      swo = l->data;
      if (swo->object == object)
      {
         _etk_widget_unswallow_full(swallower, l);
         break;
      }
   }
}

/**
 * @brief Checks if the widget swallows the object
 * @param widget a widget
 * @param object an evas object
 * @return Returns ETK_TRUE if @a widget is swallowing @a object
 * @widget_implementation
 */
Etk_Bool etk_widget_is_swallowing_object(Etk_Widget *widget, Evas_Object *object)
{
   Evas_List *l;
   Etk_Widget_Swallowed_Object *swallowed_object;

   if (!widget || !object)
      return ETK_FALSE;

   for (l = widget->swallowed_objects; l; l = l->next)
   {
      swallowed_object = l->data;
      if (swallowed_object->object == object)
         return ETK_TRUE;
   }
   return ETK_FALSE;
}

/**
 * @brief Gets the code corresponding to the error that occurs during the last call of
 * etk_widget_swallow_widget() or etk_widget_swallow_object()
 * @return Returns the error code
 * @widget_implementation
 */
Etk_Widget_Swallow_Error etk_widget_swallow_error_get()
{
   return _etk_widget_swallow_error;
}

/**
 * @brief Sends a signal to the theme object of the widget
 * @param widget a widget
 * @param signal_name the name of the signal to send
 * @widget_implementation
 */
void etk_widget_theme_object_signal_emit(Etk_Widget *widget, const char *signal_name)
{
   if (!widget || !widget->theme_object)
      return;
   edje_object_signal_emit(widget->theme_object, signal_name, "");
   widget->need_theme_min_size_recalc = ETK_TRUE;
}

/**
 * @brief Sets a the text of a text part of the theme object of the widget
 * @param widget a widget
 * @param part_name the name of the text part
 * @param text the text to set
 * @widget_implementation
 */
void etk_widget_theme_object_part_text_set(Etk_Widget *widget, const char *part_name, const char *text)
{
   if (!widget || !widget->theme_object)
      return;
   edje_object_part_text_set(widget->theme_object, part_name, text);
   widget->need_theme_min_size_recalc = ETK_TRUE;
}

/**
 * @brief Gets the value of data from the theme of the widget. The widget has to be realized
 * @param widget a widget
 * @param data_name the name of the data you want to get the value
 * @param format the format of the data. Same format than the format argument of sscanf
 * @param ... the location of the variables where to set the values
 * @return Returns the number of the input items successfully matched and assigned, same as sscanf
 * @widget_implementation
 */
int etk_widget_theme_object_data_get(Etk_Widget *widget, const char *data_name, const char *format, ...)
{
   const char *data_string;
   int result;
   va_list args;
   
   if (!widget || !data_name || !format || !widget->theme_object)
      return 0;
   if (!(data_string = edje_object_data_get(widget->theme_object, data_name)))
      return 0;
   
   /* TODO: remove that vsscanf compiler warning */
   va_start(args, format);
   result = vsscanf(data_string, format, args);
   va_end(args);
   
   return result;
}

/**
 * @brief Adds an evas object to the list of member objects of the widget. @n
 * A member object is automatically shown/hidden when the widget is shown/hidden, it is automatically clipped when the
 * widget is, and it is used by the widget to receive mouse events. @n
 * The object will be automatically deleted when the object will be unrealized, unless you remove it before the widget
 * is deleted, with etk_widget_member_object_del().
 * @param widget a widget
 * @param object the evas object to add
 * @return Returns ETK_TRUE on success. ETK_FALSE on failure, probably because the widget and the object do not
 * belong to the same evas, or because the widget is not realized yet
 * @note The object has to belong to the same evas than the widget
 * @widget_implementation
 */
Etk_Bool etk_widget_member_object_add(Etk_Widget *widget, Evas_Object *object)
{
   Etk_Widget_Member_Object *member_object;
   Etk_Widget *prev_widget;
   
   if (!widget || !object || !widget->smart_object || (evas_object_evas_get(object) != etk_widget_toplevel_evas_get(widget)))
      return ETK_FALSE;
   if (_etk_widget_member_object_find(widget, object))
      return ETK_TRUE;
   
   if ((prev_widget = ETK_WIDGET(evas_object_data_get(object, "_Etk_Widget::Parent"))))
      etk_widget_member_object_del(prev_widget, object);
   else if ((prev_widget = ETK_WIDGET(evas_object_data_get(object, "_Etk_Widget::Swallower"))))
      etk_widget_unswallow_object(prev_widget, object);
   
   member_object = malloc(sizeof(Etk_Widget_Member_Object));
   member_object->object = object;
   member_object->visible = evas_object_visible_get(object);
   
   _etk_widget_object_add_to_smart(widget, object, !widget->clip_set);
   evas_object_intercept_show_callback_add(object, _etk_widget_member_object_intercept_show_cb, widget);
   evas_object_intercept_hide_callback_add(object, _etk_widget_member_object_intercept_hide_cb, widget);
   evas_object_event_callback_add(object, EVAS_CALLBACK_FREE, _etk_widget_member_object_deleted_cb, widget);
   evas_object_data_set(object, "_Etk_Widget::Parent", widget);
   
   widget->member_objects = evas_list_append(widget->member_objects, member_object);

   return ETK_TRUE;
}

/**
 * @brief Removes an evas object from the list of member objects of the widget. After that, the object will still be
 * displayed on the Evas.
 * @param widget a widget
 * @param object the evas object to remove
 * @widget_implementation
 */
void etk_widget_member_object_del(Etk_Widget *widget, Evas_Object *object)
{
   Evas_List *l;
   Etk_Widget_Member_Object *member_object;

   if (!widget || !object)
      return;

   if ((l = _etk_widget_member_object_find(widget, object)))
   {
      member_object = l->data;
      
      evas_object_smart_member_del(object);
      if (widget->clip)
         evas_object_clip_unset(member_object->object);
      
      evas_object_intercept_show_callback_del(member_object->object, _etk_widget_member_object_intercept_show_cb);
      evas_object_intercept_hide_callback_del(member_object->object, _etk_widget_member_object_intercept_hide_cb);
      evas_object_event_callback_del(object, EVAS_CALLBACK_FREE, _etk_widget_member_object_deleted_cb);
      evas_object_data_del(object, "_Etk_Widget::Parent");
      
      free(member_object);
      widget->member_objects = evas_list_remove_list(widget->member_objects, l);
   }
}

/**
 * @brief Stacks @a object above all the other member objects of the widget
 * @param widget a widget
 * @param object the object to raise
 * @widget_implementation
 */
void etk_widget_member_object_raise(Etk_Widget *widget, Evas_Object *object)
{
   if (!widget || !object)
      return;

   if (_etk_widget_member_object_find(widget, object))
      evas_object_raise(object);
}

/**
 * @brief Stacks @a object below all the other member objects of the widget
 * @param widget a widget
 * @param object the object to lower
 * @widget_implementation
 */
void etk_widget_member_object_lower(Etk_Widget *widget, Evas_Object *object)
{
   if (!widget || !object)
      return;

   if (_etk_widget_member_object_find(widget, object))
   {
      if (widget->theme_object)
         evas_object_stack_above(object, widget->theme_object);
      else if (widget->event_object)
         evas_object_stack_above(object, widget->event_object);
      else
         evas_object_lower(object);
   }
}

/**
 * @brief Stacks the member object @a object above the member object @a above
 * @param widget a widget
 * @param object the object to restack
 * @param above the object above which @a object will be stacked
 * @widget_implementation
 */
void etk_widget_member_object_stack_above(Etk_Widget *widget, Evas_Object *object, Evas_Object *above)
{
   if (!widget || !above || (object == above))
      return;

   if (_etk_widget_member_object_find(widget, object) && _etk_widget_member_object_find(widget, above))
      evas_object_stack_above(object, above);
}

/**
 * @brief Stacks the member object @a object below the member object @a below
 * @param widget a widget
 * @param object the object to restack
 * @param below the object below which @a object will be stacked
 * @widget_implementation
 */
void etk_widget_member_object_stack_below(Etk_Widget *widget, Evas_Object *object, Evas_Object *below)
{
   if (!widget || !below || (object == below))
      return;
   
   if (_etk_widget_member_object_find(widget, object) && _etk_widget_member_object_find(widget, below))
      evas_object_stack_below(object, below);
}

/**
 * @brief Sets the clip object of the widget. The theme object and the member objects of the widget will be
 * clipped against @a clip
 * @param widget a widget
 * @param clip the clip object to set
 * @widget_implementation
 */
void etk_widget_clip_set(Etk_Widget *widget, Evas_Object *clip)
{
   if (!widget)
      return;
   
   if (widget->clip)
      etk_widget_clip_unset(widget);
   
   if (clip)
   {
      if (widget->smart_object)
         evas_object_clip_set(widget->smart_object, clip);
      else
         _etk_widget_add_to_clip(widget, clip);
      widget->clip = clip;
   }
}

/**
 * @brief Unsets the clip object of the widget. The theme object and the member objects won't be clipped anymore
 * @param widget a widget
 * @return Returns the clip object of the widget
 * @widget_implementation
 */
void etk_widget_clip_unset(Etk_Widget *widget)
{
   if (!widget || !widget->clip)
      return;

   if (widget->smart_object)
      evas_object_clip_unset(widget->smart_object);
   else
      _etk_widget_remove_from_clip(widget, widget->clip);
   widget->clip = NULL;
}

/**
 * @brief Gets the clip object of the widget
 * @param widget a widget
 * @return Returns the clip object of the widget
 * @widget_implementation
 */
Evas_Object *etk_widget_clip_get(Etk_Widget *widget)
{
   if (!widget)
      return NULL;
   return widget->clip;
}

/**
 * @brief Sets whether the widget is dnd destination
 * @param widget a widget
 * @param on ETK_TRUE to enable this widget as a dnd destination, ETK_FALSE to disable it
 */
void etk_widget_dnd_dest_set(Etk_Widget *widget, Etk_Bool on)
{
   if (!widget)
      return;
   
   if (on)
   {
      widget->accepts_dnd = ETK_TRUE;
      widget->dnd_dest = ETK_TRUE;
      _etk_widget_dnd_dest_widgets = evas_list_append(_etk_widget_dnd_dest_widgets, widget);
   }
   else
   {
      widget->accepts_dnd = ETK_FALSE;
      widget->dnd_dest = ETK_FALSE;
      _etk_widget_dnd_dest_widgets = evas_list_remove(_etk_widget_dnd_dest_widgets, widget);
   }
}

/**
 * @brief Checks whether the widget is a dnd destination
 * @param widget a widget
 * @return Returns ETK_TRUE if the widget is a dnd destination, ETK_FALSE otherwise
 */
Etk_Bool etk_widget_dnd_dest_get(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return (widget->accepts_dnd && widget->dnd_dest);
}

/**
 * @brief Sets whether the widget is dnd source
 * @param widget a widget
 * @param on ETK_TRUE to enable this widget as a dnd source, ETK_FALSE to disable it
 */
void etk_widget_dnd_source_set(Etk_Widget *widget, Etk_Bool on)
{
   if (!widget)
      return;
   
   if (on)
   {      
      
      widget->accepts_dnd = ETK_TRUE;
      widget->dnd_source = ETK_TRUE;      
      _etk_widget_dnd_source_widgets = evas_list_append(_etk_widget_dnd_source_widgets, widget);
      
      if(!widget->dnd_internal)
      {
	 widget->drag = etk_drag_new(widget);
	 etk_signal_connect("mouse_move", ETK_OBJECT(widget), ETK_CALLBACK(_etk_widget_dnd_drag_mouse_move_cb), NULL);
	 etk_signal_connect("drag_end", ETK_OBJECT(widget->drag), ETK_CALLBACK(_etk_widget_dnd_drag_end_cb), NULL);
      }
   }
   else
   {
      widget->accepts_dnd = ETK_FALSE;
      widget->dnd_source = ETK_FALSE;
      _etk_widget_dnd_source_widgets = evas_list_remove(_etk_widget_dnd_source_widgets, widget);
      
      if(!widget->dnd_internal)
      {
	 etk_signal_disconnect("mouse_move", ETK_OBJECT(widget), ETK_CALLBACK(_etk_widget_dnd_drag_mouse_move_cb));
	 etk_signal_disconnect("drag_end", ETK_OBJECT(widget->drag), ETK_CALLBACK(_etk_widget_dnd_drag_end_cb));
      }
   }
}

/**
 * @brief Checks whether the widget is a dnd source
 * @param widget a widget
 * @return Returns ETK_TRUE if the widget is a dnd source, ETK_FALSE otherwise
 */
Etk_Bool etk_widget_dnd_source_get(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return (widget->accepts_dnd && widget->dnd_source);
}

/**
 * @brief Set whether the widget wants to handle its own dnd or not
 * @param widget a widget
 * @param on ETK_TRUE if the widget handles its own dnd, ETK_FALSE otherwise
 */
void etk_widget_dnd_internal_set(Etk_Widget *widget, Etk_Bool on)
{
   if (!widget)
      return;
   widget->dnd_internal = on;
}

/**
 * @brief Checks whether the widget wants to handle its own dnd or not
 * @param widget a widget
 * @return Returns ETK_TRUE if the widget handles its own dnd, ETK_FALSE otherwise
 */
Etk_Bool etk_widget_dnd_internal_get(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return (widget->dnd_internal);
}

/**
 * @brief Sets the visual data for the drag (the widget to be displayed)
 * @param widget a widget
 * @param drag_widget the widget that will appear in the drag window
 */
void etk_widget_dnd_drag_widget_set(Etk_Widget *widget, Etk_Widget *drag_widget)
{
   if(!widget || !drag_widget)
     return;
   
   etk_container_add(ETK_CONTAINER(widget->drag), drag_widget);
}

/**
 * @brief Gets the visual data for the drag (the widget to be displayed)
 * @param widget a widget
 * @return Returns the widget that will appear in the drag window
 */
Etk_Widget *etk_widget_dnd_drag_widget_get(Etk_Widget *widget)
{
   Evas_List *children = NULL;
   
   if(!widget)
     return NULL;
   
   children = etk_container_children_get(ETK_CONTAINER(widget->drag));
   
   if(!children)
     return NULL;
   
   if(evas_list_count(children) < 1)
     return NULL;
   
   if(ETK_IS_WIDGET(children->data))
     return (Etk_Widget*)children->data;
   
   return NULL;
}

/* TODO: doc */
void etk_widget_dnd_drag_data_set(Etk_Widget *widget, const char **types, int num_types, void *data, int data_size)
{
   if(!widget)
     return;
   
   if(!widget->drag || !widget->dnd_source)
     return;
   
   etk_drag_types_set(ETK_DRAG(widget->drag), types, num_types);
   etk_drag_data_set(ETK_DRAG(widget->drag), data, data_size);   
}

/**
 * @brief Sets the possible types for dnd
 * @param widget a widget
 * @param types list of acceptable types
 * @param num number of types
 */
void etk_widget_dnd_types_set(Etk_Widget *widget, const char **types, int num)
{
   int i;
   
   if (!widget)
      return;
   
   /* free old data */
   if(num <= 0 || types == NULL || 
     (widget->dnd_types_num > 0 && widget->dnd_types != NULL))
   {
      
      for(i = 0; i < widget->dnd_types_num; i++)
	if(widget->dnd_types[i]) free(widget->dnd_types[i]);
      
      if(widget->dnd_types) free(widget->dnd_types);
      
      widget->dnd_types = NULL;
      widget->dnd_types_num = 0;
      return;
   }
   
   widget->dnd_types = calloc(num, sizeof(char*));
   for(i = 0; i < num; i++)
     widget->dnd_types[i] = strdup(types[i]);
   
   widget->dnd_types_num = num;
}

/**
 * @brief Gets the possible types for dnd
 * @param widget a widget
 * @param num number of types that the widget has
 * @return returns the dnd types this widget supports
 */
const char **etk_widget_dnd_types_get(Etk_Widget *widget, int *num)
{
   if (!widget)
      return ETK_FALSE;
   
   if(num)
     *num = widget->dnd_types_num;
   return (const char **)(widget->dnd_types);
}

/**
 * @brief Gets the list of the widgets that are dnd destinations
 * @return Returns the list of the dnd destination widgets
 */
Evas_List *etk_widget_dnd_dest_widgets_get()
{
   return _etk_widget_dnd_dest_widgets;
}

/**
 * @brief Sends the "drag_drop" signal
 * @param widget a widget
 */
void etk_widget_drag_drop(Etk_Widget *widget, Etk_Event_Selection_Request *event)
{
   if (!widget)
      return;
   
   /* TODO: FIXME: why isnt this being emitted automatically?!? */
   etk_widget_theme_object_signal_emit(widget, "drag_drop");   
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_DRAG_DROP_SIGNAL], ETK_OBJECT(widget), NULL, event);
}

/**
 * @brief Sends the "drag_motion" signal
 * @param widget a widget
 */
void etk_widget_drag_motion(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_DRAG_MOTION_SIGNAL], ETK_OBJECT(widget), NULL);
}

/**
 * @brief Sends the "drag_enter" signal
 * @param widget a widget
 */
void etk_widget_drag_enter(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_DRAG_ENTER_SIGNAL], ETK_OBJECT(widget), NULL);
}

/**
 * @brief Sends the "drag_leave" signal
 * @param widget a widget
 */
void etk_widget_drag_leave(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_DRAG_LEAVE_SIGNAL], ETK_OBJECT(widget), NULL);
}

/**
 * @brief Sends the "drag_begin" signal
 * @param widget a widget
 */
void etk_widget_drag_begin(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_DRAG_BEGIN_SIGNAL], ETK_OBJECT(widget), NULL);
}

/**
 * @brief Sends the "drag_end" signal
 * @param widget a widget
 */
void etk_widget_drag_end(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_DRAG_END_SIGNAL], ETK_OBJECT(widget), NULL);
}

/**
 * @brief Sends the "selection_received" signal
 * @param widget a widget
 */
void etk_widget_selection_received(Etk_Widget *widget, Etk_Event_Selection_Request *event)
{
   if (!widget)
      return;
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_SELECTION_RECEIVED_SIGNAL], ETK_OBJECT(widget), NULL, event);
}

/**
 * @brief Sends the "clipboard_received" signal
 * @param widget a widget
 */
void etk_widget_clipboard_received(Etk_Widget *widget, Etk_Event_Selection_Request *event)
{
   if (!widget)
      return;
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_CLIPBOARD_RECEIVED_SIGNAL], ETK_OBJECT(widget), NULL, event);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the widget */
static void _etk_widget_constructor(Etk_Widget *widget)
{
   if (!widget)
      return;

   widget->name = NULL;
   widget->parent = NULL;
   widget->toplevel_parent = NULL;
   widget->child_properties = NULL;
   widget->children = NULL;
   widget->focus_order = NULL;

   widget->theme_object = NULL;
   widget->theme_min_width = 0;
   widget->theme_min_height = 0;
   widget->theme_file = NULL;
   widget->theme_group = NULL;
   widget->theme_parent = NULL;
   widget->theme_children = NULL;

   widget->smart_object = NULL;
   widget->event_object = NULL;
   widget->clip = NULL;
   widget->swallowed_objects = NULL;
   widget->member_objects = NULL;

   widget->show = _etk_widget_show_handler;
   widget->enter = _etk_widget_enter_handler;
   widget->leave = _etk_widget_leave_handler;
   widget->focus = _etk_widget_focus_handler;
   widget->unfocus = _etk_widget_unfocus_handler;
   widget->drag_drop = _etk_widget_drag_drop_handler;
   widget->drag_motion = _etk_widget_drag_motion_handler;
   widget->drag_enter = _etk_widget_drag_enter_handler;   
   widget->drag_leave = _etk_widget_drag_leave_handler;
   widget->drag_begin = _etk_widget_drag_begin_handler;   
   widget->drag_end = _etk_widget_drag_end_handler;

   widget->left_inset = 0;
   widget->right_inset = 0;
   widget->top_inset = 0;
   widget->bottom_inset = 0;

   widget->geometry.x = -1;
   widget->geometry.y = -1;
   widget->geometry.w = -1;
   widget->geometry.h = -1;
   widget->inner_geometry.x = -1;
   widget->inner_geometry.y = -1;
   widget->inner_geometry.w = -1;
   widget->inner_geometry.h = -1;
   widget->size_request = NULL;
   widget->size_allocate = NULL;
   widget->requested_size.w = -1;
   widget->requested_size.h = -1;
   widget->last_size_requisition.w = 0;
   widget->last_size_requisition.h = 0;

   widget->scroll_size_get = NULL;
   widget->scroll_margins_get = NULL;
   widget->scroll = NULL;
   widget->clip_set = NULL;
   widget->clip_unset = NULL;

   widget->realized = ETK_FALSE;
   widget->swallowed = ETK_FALSE;
   widget->visible = ETK_FALSE;
   widget->visibility_locked = ETK_FALSE;
   widget->focusable = ETK_FALSE;
   widget->focus_on_press = ETK_FALSE;
   widget->use_focus_order = ETK_FALSE;
   widget->has_event_object = ETK_FALSE;
   widget->repeat_mouse_events = ETK_FALSE;
   widget->pass_mouse_events = ETK_FALSE;
   widget->need_size_recalc = ETK_FALSE;
   widget->need_redraw = ETK_FALSE;
   widget->need_theme_min_size_recalc = ETK_FALSE;
   widget->swallowed = ETK_FALSE;
   widget->accepts_dnd = ETK_FALSE;
   widget->dnd_internal = ETK_FALSE;
   widget->dnd_source = ETK_FALSE;
   widget->dnd_dest = ETK_FALSE;
   widget->dnd_types = NULL;
   widget->dnd_types_num = 0;
   
   etk_signal_connect("mouse_in", ETK_OBJECT(widget), ETK_CALLBACK(_etk_widget_signal_mouse_in_cb), NULL);
   etk_signal_connect("mouse_out", ETK_OBJECT(widget), ETK_CALLBACK(_etk_widget_signal_mouse_out_cb), NULL);
   etk_signal_connect("mouse_down", ETK_OBJECT(widget), ETK_CALLBACK(_etk_widget_signal_mouse_down_cb), NULL);
   etk_signal_connect("mouse_wheel", ETK_OBJECT(widget), ETK_CALLBACK(_etk_widget_signal_mouse_wheel_cb), NULL);
   etk_signal_connect("key_down", ETK_OBJECT(widget), ETK_CALLBACK(_etk_widget_signal_key_down_cb), NULL);
   etk_signal_connect("key_up", ETK_OBJECT(widget), ETK_CALLBACK(_etk_widget_signal_key_up_cb), NULL);
   
   if (ETK_IS_TOPLEVEL_WIDGET(widget))
      etk_object_notification_callback_add(ETK_OBJECT(widget), "evas", _etk_widget_toplevel_evas_changed_cb, NULL);
}

/* Destroys the widget */
/* TODO: can we unparent safely the focused widget? */
static void _etk_widget_destructor(Etk_Widget *widget)
{
   if (!widget)
      return;

   _etk_widget_unrealize(widget);
   
   /* Remove the children */
   while (widget->children)
      etk_object_destroy(ETK_OBJECT(widget->children->data));
   evas_list_free(widget->focus_order);
   etk_widget_parent_set(widget, NULL);
   
   while (widget->theme_children)
   {
      ETK_WIDGET(widget->theme_children->data)->theme_parent = NULL;
      widget->theme_children = evas_list_remove_list(widget->theme_children, widget->theme_children);
   }
   if (widget->theme_parent)
      widget->theme_parent->theme_children = evas_list_remove(widget->theme_parent->theme_children, widget);
   
   if (widget->clip)
      _etk_widget_remove_from_clip(widget, widget->clip);
   
   /* Remove the widget from the dnd lists */
   if (widget->accepts_dnd && widget->dnd_dest)
      _etk_widget_dnd_dest_widgets = evas_list_remove(_etk_widget_dnd_dest_widgets, widget);
   if (widget->accepts_dnd && widget->dnd_source)
     _etk_widget_dnd_source_widgets = evas_list_remove(_etk_widget_dnd_source_widgets, widget);
   
   free(widget->theme_file);
   free(widget->theme_group);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_widget_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_WIDGET_THEME_FILE_PROPERTY:
         etk_widget_theme_file_set(widget, etk_property_value_string_get(value));
         break;
      case ETK_WIDGET_THEME_GROUP_PROPERTY:
         etk_widget_theme_group_set(widget, etk_property_value_string_get(value));
         break;
      case ETK_WIDGET_THEME_PARENT_PROPERTY:
         etk_widget_theme_parent_set(widget, ETK_WIDGET(etk_property_value_pointer_get(value)));
         break;
      case ETK_WIDGET_VISIBLE_PROPERTY:
         if (etk_property_value_bool_get(value))
            etk_widget_show(widget);
         else
            etk_widget_hide(widget);
         break;
      case ETK_WIDGET_VISIBILITY_LOCKED_PROPERTY:
         etk_widget_visibility_locked_set(widget, etk_property_value_bool_get(value));
         break;
      case ETK_WIDGET_WIDTH_REQUEST_PROPERTY:
         etk_widget_size_request_set(widget, etk_property_value_int_get(value), widget->requested_size.h);
         break;
      case ETK_WIDGET_HEIGHT_REQUEST_PROPERTY:
         etk_widget_size_request_set(widget, widget->requested_size.w, etk_property_value_int_get(value));
         break;
      case ETK_WIDGET_NAME_PROPERTY:
         etk_widget_name_set(widget, etk_property_value_string_get(value));
         break;
      case ETK_WIDGET_HAS_EVENT_OBJECT_PROPERTY:
         etk_widget_has_event_object_set(widget, etk_property_value_bool_get(value));
         break;
      case ETK_WIDGET_REPEAT_MOUSE_EVENTS_PROPERTY:
         etk_widget_repeat_mouse_events_set(widget, etk_property_value_bool_get(value));
         break;
      case ETK_WIDGET_PASS_MOUSE_EVENTS_PROPERTY:
         etk_widget_pass_mouse_events_set(widget, etk_property_value_bool_get(value));
         break;
      case ETK_WIDGET_FOCUSABLE_PROPERTY:
         /* TODO: is the widget is the one which is focused, we should change the focused widget */
         widget->focusable = etk_property_value_bool_get(value);
         etk_object_notify(object, "focusable");
      /* TODO: rename it to "focus_on_click" */
      case ETK_WIDGET_FOCUS_ON_PRESS_PROPERTY:
         widget->focus_on_press = etk_property_value_bool_get(value);
         etk_object_notify(object, "focus_on_press");
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_widget_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_WIDGET_NAME_PROPERTY:
         etk_property_value_string_set(value, widget->name);
         break;
      case ETK_WIDGET_PARENT_PROPERTY:
         etk_property_value_pointer_set(value, ETK_OBJECT(widget->parent));
         break;
      case ETK_WIDGET_THEME_FILE_PROPERTY:
         etk_property_value_string_set(value, widget->theme_file);
         break;
      case ETK_WIDGET_THEME_GROUP_PROPERTY:
         etk_property_value_string_set(value, widget->theme_group);
         break;
      case ETK_WIDGET_THEME_PARENT_PROPERTY:
         etk_property_value_pointer_set(value, widget->theme_parent);
         break;
      case ETK_WIDGET_WIDTH_REQUEST_PROPERTY:
         etk_property_value_int_set(value, widget->requested_size.w);
         break;
      case ETK_WIDGET_HEIGHT_REQUEST_PROPERTY:
         etk_property_value_int_set(value, widget->requested_size.h);
         break;
      case ETK_WIDGET_VISIBLE_PROPERTY:
         etk_property_value_bool_set(value, widget->visible);
         break;
      case ETK_WIDGET_VISIBILITY_LOCKED_PROPERTY:
         etk_property_value_bool_set(value, widget->visibility_locked);
         break;
      case ETK_WIDGET_HAS_EVENT_OBJECT_PROPERTY:
         etk_property_value_bool_set(value, widget->has_event_object);
         break;
      case ETK_WIDGET_REPEAT_MOUSE_EVENTS_PROPERTY:
         etk_property_value_bool_set(value, widget->repeat_mouse_events);
         break;
      case ETK_WIDGET_PASS_MOUSE_EVENTS_PROPERTY:
         etk_property_value_bool_set(value, widget->pass_mouse_events);
         break;
      case ETK_WIDGET_FOCUSABLE_PROPERTY:
         etk_property_value_bool_set(value, widget->focusable);
         break;
      case ETK_WIDGET_FOCUS_ON_PRESS_PROPERTY:
         etk_property_value_bool_set(value, widget->focus_on_press);
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

/* Default handler for the "enter" signal */
static void _etk_widget_enter_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_object_signal_emit(widget, "enter");
}

/* Default handler for the "leave" signal */
static void _etk_widget_leave_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_object_signal_emit(widget, "leave");
}

/* Default handler for the "focus" signal */
static void _etk_widget_focus_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_object_signal_emit(widget, "focus");
}

/* Default handler for the "unfocus" signal */
static void _etk_widget_unfocus_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_object_signal_emit(widget, "unfocus");
}

/* Sets the widget as visible and queues a visibility update */
static void _etk_widget_show_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_object_signal_emit(widget, "show");
}

/* Default handler for the "drag_drop" signal */
static void _etk_widget_drag_drop_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_object_signal_emit(widget, "drag_drop");
}

/* Default handler for the "drag_motion" signal */
static void _etk_widget_drag_motion_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_object_signal_emit(widget, "drag_motion");
}

/* Default handler for the "drag_enter" signal */
static void _etk_widget_drag_enter_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_object_signal_emit(widget, "drag_enter");
}

/* Default handler for the "drag_leave" signal */
static void _etk_widget_drag_leave_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_object_signal_emit(widget, "drag_leave");
}

/* Default handler for the "drag_begin" signal */
static void _etk_widget_drag_begin_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_object_signal_emit(widget, "drag_begin");
}

/* Default handler for the "drag_end" signal */
static void _etk_widget_drag_end_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_object_signal_emit(widget, "drag_end");
}

/* Evas Callback: Called when the mouse pointer enters the widget */
static void _etk_widget_mouse_in_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Evas_Event_Mouse_In *evas_event = event_info;
   Etk_Event_Mouse_In_Out event;

   if (!(widget = ETK_WIDGET(data)))
      return;
   
   event.buttons = evas_event->buttons;
   event.canvas.x = evas_event->canvas.x;
   event.canvas.y = evas_event->canvas.y;
   event.widget.x = evas_event->canvas.x - widget->inner_geometry.x;
   event.widget.y = evas_event->canvas.y - widget->inner_geometry.y;
   event.modifiers = evas_event->modifiers;
   event.locks = evas_event->locks;
   event.timestamp = evas_event->timestamp;

   if (!etk_signal_emit(_etk_widget_signals[ETK_WIDGET_MOUSE_IN_SIGNAL], ETK_OBJECT(widget), NULL, &event))
      return;
   
   /* TODO: should we really propagate the mouse in event? */
   if (widget->parent)
      _etk_widget_mouse_in_cb(widget->parent, evas, NULL, event_info);
}

/* Signal Callback: Called when the mouse pointer enters the widget */
static void _etk_widget_signal_mouse_in_cb(Etk_Object *object, Etk_Event_Mouse_In_Out *event, void *data)
{
   if (!object)
      return;
   etk_widget_enter(ETK_WIDGET(object));
}

/* Evas Callback: Called when the mouse pointer leaves the widget */
static void _etk_widget_mouse_out_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Evas_Event_Mouse_Out *evas_event = event_info;
   Etk_Event_Mouse_In_Out event;

   if (!(widget = ETK_WIDGET(data)))
      return;
   
   event.buttons = evas_event->buttons;
   event.canvas.x = evas_event->canvas.x;
   event.canvas.y = evas_event->canvas.y;
   event.widget.x = evas_event->canvas.x - widget->inner_geometry.x;
   event.widget.y = evas_event->canvas.y - widget->inner_geometry.y;
   event.modifiers = evas_event->modifiers;
   event.locks = evas_event->locks;
   event.timestamp = evas_event->timestamp;

   if (!etk_signal_emit(_etk_widget_signals[ETK_WIDGET_MOUSE_OUT_SIGNAL], ETK_OBJECT(widget), NULL, &event))
      return;
   
   /* TODO: should we really propagate the mouse out event? */
   if (widget->parent)
      _etk_widget_mouse_out_cb(widget->parent, evas, NULL, event_info);
}

/* Signal Callback: Called when the mouse pointer leaves the widget */
static void _etk_widget_signal_mouse_out_cb(Etk_Object *object, Etk_Event_Mouse_In_Out *event, void *data)
{
   if (!object)
      return;
   etk_widget_leave(ETK_WIDGET(object));
}

/* Evas Callback: Called when the mouse pointer moves over the widget */
static void _etk_widget_mouse_move_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Evas_Event_Mouse_Move *evas_event = event_info;
   Etk_Event_Mouse_Move event;

   if (!(widget = ETK_WIDGET(data)))
      return;
   
   event.buttons = evas_event->buttons;
   event.cur.canvas.x = evas_event->cur.canvas.x;
   event.cur.canvas.y = evas_event->cur.canvas.y;
   event.cur.widget.x = evas_event->cur.canvas.x - widget->inner_geometry.x;
   event.cur.widget.y = evas_event->cur.canvas.y - widget->inner_geometry.y;
   event.prev.canvas.x = evas_event->prev.canvas.x;
   event.prev.canvas.y = evas_event->prev.canvas.y;
   event.prev.widget.x = evas_event->prev.canvas.x - widget->inner_geometry.x;
   event.prev.widget.y = evas_event->prev.canvas.y - widget->inner_geometry.y;
   event.modifiers = evas_event->modifiers;
   event.locks = evas_event->locks;
   event.timestamp = evas_event->timestamp;

   if (!etk_signal_emit(_etk_widget_signals[ETK_WIDGET_MOUSE_MOVE_SIGNAL], ETK_OBJECT(widget), NULL, &event))
      return;
   
   if (widget->parent)
      _etk_widget_mouse_move_cb(widget->parent, evas, NULL, event_info);
}

/* Evas Callback: Called when the widget is pressed by the mouse */
static void _etk_widget_mouse_down_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Evas_Event_Mouse_Down *evas_event = event_info;
   Etk_Event_Mouse_Up_Down event;

   if (!(widget = ETK_WIDGET(data)))
      return;
   
   event.button = evas_event->button;
   event.canvas.x = evas_event->canvas.x;
   event.canvas.y = evas_event->canvas.y;
   event.widget.x = evas_event->canvas.x - widget->inner_geometry.x;
   event.widget.y = evas_event->canvas.y - widget->inner_geometry.y;
   event.modifiers = evas_event->modifiers;
   event.locks = evas_event->locks;
   event.flags = evas_event->flags;
   event.timestamp = evas_event->timestamp;

   if (!etk_signal_emit(_etk_widget_signals[ETK_WIDGET_MOUSE_DOWN_SIGNAL], ETK_OBJECT(widget), NULL, &event))
      return;
   
   if (widget->repeat_mouse_events && widget->parent)
      _etk_widget_mouse_down_cb(widget->parent, evas, NULL, event_info);
}

/* Signal Callback: Called when the widget is pressed by the mouse */
static void _etk_widget_signal_mouse_down_cb(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data)
{
   Etk_Widget *widget;
 
   if (!(widget = ETK_WIDGET(object)) || !widget->focus_on_press)
      return;
   etk_widget_focus(widget);
}

/* Evas Callback: Called when the widget is released by the mouse */
static void _etk_widget_mouse_up_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Evas_Event_Mouse_Up *evas_event = event_info;
   Etk_Event_Mouse_Up_Down event;

   if (!(widget = ETK_WIDGET(data)))
      return;
   
   event.button = evas_event->button;
   event.canvas.x = evas_event->canvas.x;
   event.canvas.y = evas_event->canvas.y;
   event.widget.x = evas_event->canvas.x - widget->inner_geometry.x;
   event.widget.y = evas_event->canvas.y - widget->inner_geometry.y;
   event.modifiers = evas_event->modifiers;
   event.locks = evas_event->locks;
   event.flags = evas_event->flags;
   event.timestamp = evas_event->timestamp;

   if (!etk_signal_emit(_etk_widget_signals[ETK_WIDGET_MOUSE_UP_SIGNAL], ETK_OBJECT(widget), NULL, &event))
      return;
   
   if (evas_event->canvas.x >= widget->geometry.x && evas_event->canvas.x <= widget->geometry.x + widget->geometry.w &&
      evas_event->canvas.y >= widget->geometry.y && evas_event->canvas.y <= widget->geometry.y + widget->geometry.h)
   {
      if (!etk_signal_emit(_etk_widget_signals[ETK_WIDGET_MOUSE_CLICK_SIGNAL], ETK_OBJECT(widget), NULL, &event))
         return;
   }
   
   /* TODO: what if the widget is destroyed by one of the callbacks (recurrent problem!!!) ?? */
   if (widget->repeat_mouse_events && widget->parent)
      _etk_widget_mouse_up_cb(widget->parent, evas, NULL, event_info);
}

/* Evas Callback: Called when the mouse wheel is used over the widget */
static void _etk_widget_mouse_wheel_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Evas_Event_Mouse_Wheel *evas_event = event_info;
   Etk_Event_Mouse_Wheel event;

   if (!(widget = ETK_WIDGET(data)))
      return;
   
   event.direction = evas_event->direction;
   event.z = evas_event->z;
   event.canvas.x = evas_event->canvas.x;
   event.canvas.y = evas_event->canvas.y;
   event.widget.x = evas_event->canvas.x - widget->inner_geometry.x;
   event.widget.y = evas_event->canvas.y - widget->inner_geometry.y;
   event.modifiers = evas_event->modifiers;
   event.locks = evas_event->locks;
   event.timestamp = evas_event->timestamp;

   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_MOUSE_WHEEL_SIGNAL], ETK_OBJECT(widget), NULL, &event);
}

/* Signal Callback: Called when the mouse wheel is used over the widget */
static void _etk_widget_signal_mouse_wheel_cb(Etk_Object *object, Etk_Event_Key_Up_Down *event, void *data)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(object)) || !widget->parent)
      return;
   
   /* Propagates the event to the parent widget */
   /* TODO: we should recalc the coords of the event according to the parent geometry! */
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_MOUSE_WHEEL_SIGNAL], ETK_OBJECT(widget->parent), NULL, event);
}

/* Evas Callback: Called when the user presses a key and if the widget is focused */
static void _etk_widget_key_down_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Evas_Event_Key_Down *evas_event = event_info;
   Etk_Event_Key_Up_Down event;

   if (!(widget = ETK_WIDGET(data)))
      return;
   
   event.keyname = evas_event->keyname;
   event.modifiers = evas_event->modifiers;
   event.locks = evas_event->locks;
   event.key = evas_event->key;
   event.string = evas_event->string;
   event.compose = evas_event->compose;
   event.timestamp = evas_event->timestamp;

   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_KEY_DOWN_SIGNAL], ETK_OBJECT(widget), NULL, &event);
}

/* Signal Callback: Called when the user presses a key and if the widget is focused */
static void _etk_widget_signal_key_down_cb(Etk_Object *object, Etk_Event_Key_Up_Down *event, void *data)
{
   Etk_Widget *widget;
   Etk_Toplevel_Widget *toplevel;

   if (!(widget = ETK_WIDGET(object)) || !event || !(toplevel = (widget->toplevel_parent)) || !event->key)
      return;

   if (strcmp(event->key, "Tab") == 0)
      etk_widget_focus(etk_toplevel_widget_focused_widget_next_get(toplevel));
   else if (strcmp(event->key, "ISO_Left_Tab") == 0)
      etk_widget_focus(etk_toplevel_widget_focused_widget_prev_get(toplevel));
   else
   {
      /* Propagates the event to the parent widget */
      etk_signal_emit(_etk_widget_signals[ETK_WIDGET_KEY_DOWN_SIGNAL], ETK_OBJECT(widget->parent), NULL, event);
   }
}

/* Evas Callback: Called when the user releases a key and if the widget is focused */
static void _etk_widget_key_up_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Evas_Event_Key_Up *evas_event = event_info;
   Etk_Event_Key_Up_Down event;

   if (!(widget = ETK_WIDGET(data)))
      return;
   
   event.keyname = evas_event->keyname;
   event.modifiers = evas_event->modifiers;
   event.locks = evas_event->locks;
   event.key = evas_event->key;
   event.string = evas_event->string;
   event.compose = evas_event->compose;
   event.timestamp = evas_event->timestamp;

   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_KEY_UP_SIGNAL], ETK_OBJECT(widget), NULL, &event);
}

/* Signal Callback: Called when the user releases a key and if the widget is focused */
static void _etk_widget_signal_key_up_cb(Etk_Object *object, Etk_Event_Key_Up_Down *event, void *data)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(object)) || !widget->parent)
      return;
   
   /* Propagates the event to the parent widget */
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_KEY_UP_SIGNAL], ETK_OBJECT(widget->parent), NULL, event);
}

/* Called when the widget is a toplevel widget and when its evas is changed */
/* TODO: do we need that? Maybe a better way to do that? */
static void _etk_widget_toplevel_evas_changed_cb(Etk_Object *object, const char *property_name, void *data)
{
   Etk_Widget *widget;
   Evas *evas;
   
   if (!(widget = ETK_WIDGET(object)))
      return;
   
   evas = etk_toplevel_widget_evas_get(ETK_TOPLEVEL_WIDGET(widget));
   if (evas && (!widget->smart_object || evas_object_evas_get(widget->smart_object) != evas))
      _etk_widget_realize_all(widget);
   else if (!evas && widget->smart_object)
      _etk_widget_unrealize_all(widget);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Realizes the widget: it will load the theme and allocate the graphical ressources */
static void _etk_widget_realize(Etk_Widget *widget)
{
   Evas *evas = NULL;
   Evas_List *l;
   Etk_Widget *child;
   Etk_Widget *theme_parent;
   
   if (!widget || !(evas = etk_widget_toplevel_evas_get(widget)))
      return;
   
   if (widget->realized)
      _etk_widget_unrealize(widget);
   
   /* First, we create the smart object */
   widget->smart_object = _etk_widget_smart_object_add(evas, widget);
   evas_object_event_callback_add(widget->smart_object, EVAS_CALLBACK_MOUSE_IN, _etk_widget_mouse_in_cb, widget);
   evas_object_event_callback_add(widget->smart_object, EVAS_CALLBACK_MOUSE_OUT, _etk_widget_mouse_out_cb, widget);
   evas_object_event_callback_add(widget->smart_object, EVAS_CALLBACK_MOUSE_MOVE, _etk_widget_mouse_move_cb, widget);
   evas_object_event_callback_add(widget->smart_object, EVAS_CALLBACK_MOUSE_DOWN, _etk_widget_mouse_down_cb, widget);
   evas_object_event_callback_add(widget->smart_object, EVAS_CALLBACK_MOUSE_UP, _etk_widget_mouse_up_cb, widget);
   evas_object_event_callback_add(widget->smart_object, EVAS_CALLBACK_MOUSE_WHEEL, _etk_widget_mouse_wheel_cb, widget);
   evas_object_event_callback_add(widget->smart_object, EVAS_CALLBACK_KEY_DOWN, _etk_widget_key_down_cb, widget);
   evas_object_event_callback_add(widget->smart_object, EVAS_CALLBACK_KEY_UP, _etk_widget_key_up_cb, widget);
   
   if (widget->parent && widget->parent->smart_object)
      _etk_widget_object_add_to_smart(widget->parent, widget->smart_object, /*!widget->parent->clip_set && */!widget->clip);
   for (l = widget->children; l; l = l->next)
   {
      child = ETK_WIDGET(l->data);
      if (child->smart_object)
         _etk_widget_object_add_to_smart(widget, child->smart_object, !widget->clip_set/* && !child->clip*/);
   }
   
   if (widget->visible && (ETK_IS_TOPLEVEL_WIDGET(widget) ||
      (widget->parent && widget->parent->smart_object && evas_object_visible_get(widget->parent->smart_object))))
   {
      evas_object_show(widget->smart_object);
   }
   else
      evas_object_hide(widget->smart_object);

   evas_object_propagate_events_set(widget->smart_object, 0);
   evas_object_repeat_events_set(widget->smart_object, widget->repeat_mouse_events);
   evas_object_pass_events_set(widget->smart_object, widget->pass_mouse_events);
   
   /* TODO: focus: we should probably check if the toplevel widget is focused too! */
   if (widget->toplevel_parent && (widget == etk_toplevel_widget_focused_widget_get(widget->toplevel_parent)))
      evas_object_focus_set(widget->smart_object, 1);
   
   /* Then, we create the theme object */
   theme_parent = widget->theme_parent ? widget->theme_parent : widget->parent;
   if ((widget->theme_object = etk_theme_object_load_from_parent(evas, theme_parent, widget->theme_file, widget->theme_group)))
   {
      if (etk_widget_theme_object_data_get(widget, "inset", "%d %d %d %d",
         &widget->left_inset, &widget->right_inset, &widget->top_inset, &widget->bottom_inset) != 4)
      {
         widget->left_inset = 0;
         widget->right_inset = 0;
         widget->top_inset = 0;
         widget->bottom_inset = 0;
      }
      evas_object_show(widget->theme_object);
      _etk_widget_object_add_to_smart(widget, widget->theme_object, ETK_TRUE);
      evas_object_lower(widget->theme_object);
   }
   /* And we create the event object if necessary */
   else if (widget->has_event_object)
   {
      widget->event_object = evas_object_rectangle_add(evas);
      evas_object_color_set(widget->event_object, 255, 255, 255, 0);
      evas_object_show(widget->event_object);
      _etk_widget_object_add_to_smart(widget, widget->event_object, ETK_TRUE);
      evas_object_lower(widget->event_object);
   }

   widget->need_theme_min_size_recalc = ETK_TRUE;
   widget->realized = ETK_TRUE;

   if (!etk_signal_emit(_etk_widget_signals[ETK_WIDGET_REALIZE_SIGNAL], ETK_OBJECT(widget), NULL))
      return;
   
   /* Finally, we clip the widget */
   if (widget->clip)
      evas_object_clip_set(widget->smart_object, widget->clip);
   etk_widget_size_recalc_queue(widget);
}

/* Unrealizes the widget: it will unload the theme and free the graphical ressources */
static void _etk_widget_unrealize(Etk_Widget *widget)
{
   Etk_Widget_Member_Object *m;
   Etk_Widget_Swallowed_Object *swo;
   Evas_Object *object;
   Etk_Bool delete_obj;

   if (!widget || !widget->realized)
      return;

   while (widget->swallowed_objects)
   {
      swo = widget->swallowed_objects->data;
      object = swo->object;
      delete_obj = (swo->widget == NULL);
      _etk_widget_unswallow_full(widget, widget->swallowed_objects);
      if (delete_obj)
         evas_object_del(object);
   }
   while (widget->member_objects)
   {
      m = widget->member_objects->data;
      object = m->object;
      etk_widget_member_object_del(widget, object);
      evas_object_del(object);
   }
   if (widget->theme_object)
   {
      evas_object_del(widget->theme_object);
      widget->theme_object = NULL;
   }
   if (widget->event_object)
   {
      evas_object_del(widget->event_object);
      widget->event_object = NULL;
   }
   if (widget->smart_object)
   {
      evas_object_del(widget->smart_object);
      widget->smart_object = NULL;
   }
   evas_object_del(widget->smart_object);
   widget->smart_object = NULL;
   
   widget->left_inset = 0;
   widget->right_inset = 0;
   widget->top_inset = 0;
   widget->bottom_inset = 0;
   
   widget->realized = ETK_FALSE;
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_UNREALIZE_SIGNAL], ETK_OBJECT(widget), NULL);
}

/* Used by etk_widget_parent_set */
static void _etk_widget_toplevel_parent_set(Etk_Widget *widget, Etk_Toplevel_Widget *toplevel_parent)
{
   Evas_List *l;
   
   if (!widget)
      return;

   widget->toplevel_parent = toplevel_parent;
   for (l = widget->children; l; l = l->next)
      _etk_widget_toplevel_parent_set(ETK_WIDGET(l->data), toplevel_parent);
}

/* Realizes the widget and all its children */
static void _etk_widget_realize_all(Etk_Widget *widget)
{
   Evas_List *l;
   
   if (!widget)
      return;

   _etk_widget_realize(widget);
   for (l = widget->children; l; l = l->next)
      _etk_widget_realize_all(ETK_WIDGET(l->data));
}

/* Unrealizes the widget and all its children */
static void _etk_widget_unrealize_all(Etk_Widget *widget)
{
   Evas_List *l;
   
   if (!widget)
      return;

   _etk_widget_unrealize(widget);
   for (l = widget->children; l; l = l->next)
      _etk_widget_unrealize_all(ETK_WIDGET(l->data));
}

/* Realizes the children and theme children of the widget when the theme file is changed */
static void _etk_widget_realize_on_theme_file_change(Etk_Widget *widget, Etk_Bool force_realize)
{
   Evas_List *l;
   
   if (!widget)
      return;
   
   if (force_realize || !widget->theme_file)
   {
      _etk_widget_realize(widget);
      
      for (l = widget->children; l; l = l->next)
         _etk_widget_realize_on_theme_file_change(ETK_WIDGET(l->data), ETK_FALSE);
      for (l = widget->theme_children; l; l = l->next)
         _etk_widget_realize_on_theme_file_change(ETK_WIDGET(l->data), ETK_FALSE);
   }
}

/* Realizes the children and the theme children of the widget when the theme group or theme parent is changed */
static void _etk_widget_realize_on_theme_change(Etk_Widget *widget)
{
   Evas_List *l;
   
   if (!widget)
      return;
   
   _etk_widget_realize(widget);
   
   for (l = widget->children; l; l = l->next)
      _etk_widget_realize_on_theme_change(ETK_WIDGET(l->data));
   for (l = widget->theme_children; l; l = l->next)
      _etk_widget_realize_on_theme_change(ETK_WIDGET(l->data));
}

/* Calculates the minimum size of the theme object of "widget" */
static void _etk_widget_theme_min_size_calc(Etk_Widget *widget, int *w, int *h)
{
   if (!widget)
      return;

   if (widget->need_theme_min_size_recalc)
   {
      if (widget->theme_object)
      {
         int min_calc_width, min_calc_height;
         int min_get_width, min_get_height;
         Evas_List *l;
         Etk_Widget_Swallowed_Object *swallowed_object;

         for (l = widget->swallowed_objects; l; l = l->next)
         {
            swallowed_object = l->data;
            if (swallowed_object->widget)
            {
               Etk_Size swallow_size;

               etk_widget_size_request(swallowed_object->widget, &swallow_size);
               edje_extern_object_min_size_set(swallowed_object->object, swallow_size.w, swallow_size.h);
               edje_object_part_swallow(widget->theme_object, swallowed_object->part, swallowed_object->object);
            }
         }
         edje_object_size_min_calc(widget->theme_object, &min_calc_width, &min_calc_height);
         edje_object_size_min_get(widget->theme_object, &min_get_width, &min_get_height);
         widget->theme_min_width = ETK_MAX(min_calc_width, min_get_width);
         widget->theme_min_height = ETK_MAX(min_calc_height, min_get_height);

         for (l = widget->swallowed_objects; l; l = l->next)
         {
            swallowed_object = l->data;
            if (swallowed_object->widget)
            {
               edje_extern_object_min_size_set(swallowed_object->object, 0, 0);
               edje_object_part_swallow(widget->theme_object, swallowed_object->part, swallowed_object->object);
            }
         }
      }
      else
      {
         widget->theme_min_width = 0;
         widget->theme_min_height = 0;
      }
      
      widget->theme_min_width = ETK_MAX(widget->theme_min_width, widget->left_inset + widget->right_inset);
      widget->theme_min_height = ETK_MAX(widget->theme_min_height, widget->top_inset + widget->bottom_inset);
      widget->need_theme_min_size_recalc = ETK_FALSE;
   }

   if (w)
      *w = widget->theme_min_width;
   if (h)
      *h = widget->theme_min_height;
}

/* Used by etk_widget_redraw_queue() */
static void _etk_widget_redraw_queue_recursive(Etk_Widget *widget)
{
   Evas_List *l;
   
   if (!widget)
      return;

   widget->need_redraw = ETK_TRUE;
   for (l = widget->children; l; l = l->next)
      _etk_widget_redraw_queue_recursive(ETK_WIDGET(l->data));
}

/* Makes the theme object of the widget swallow a widget or an object into one of its part */
static void _etk_widget_swallow_full(Etk_Widget *swallower, const char *part, Evas_Object *object, Etk_Widget *widget)
{
   Etk_Widget *prev_widget;
   Evas_Object *prev_swallowed;
   Etk_Widget_Swallowed_Object *swo;

   if (!swallower || !part || !swallower->theme_object || (!widget && !object))
      return;
   
   if ((prev_swallowed = edje_object_part_swallow_get(swallower->theme_object, part)))
      etk_widget_unswallow_object(swallower, prev_swallowed);
   
   if (object)
   {
      if ((prev_widget = ETK_WIDGET(evas_object_data_get(object, "_Etk_Widget::Parent"))))
         etk_widget_member_object_del(prev_widget, object);
      else if ((prev_widget = ETK_WIDGET(evas_object_data_get(object, "_Etk_Widget::Swallower"))))
         etk_widget_unswallow_object(prev_widget, object);
   }
   
   swo = malloc(sizeof(Etk_Widget_Swallowed_Object));
   swo->object = object;
   swo->part = strdup(part);
   swo->widget = widget;
   swallower->swallowed_objects = evas_list_append(swallower->swallowed_objects, swo);
   
   if (object)
   {
      edje_object_part_swallow(swallower->theme_object, part, object);
      evas_object_event_callback_add(object, EVAS_CALLBACK_FREE, _etk_widget_swallowed_object_deleted_cb, swallower);
      evas_object_data_set(object, "_Etk_Widget::Swallower", swallower);
      etk_widget_size_recalc_queue(swallower);
   }
   else
      etk_signal_connect("realize", ETK_OBJECT(widget), ETK_CALLBACK(_etk_widget_swallowed_widget_realize_cb), NULL);
}

/* Makes the theme object of the widget unswallow an object */
static void _etk_widget_unswallow_full(Etk_Widget *swallower, Evas_List *swo_node)
{
   Evas_Object *object;
   Etk_Widget_Swallowed_Object *swo;
   
   if (!swallower || !swo_node)
      return;
   
   swo = swo_node->data;
   if ((object = swo->object))
   {
      edje_object_part_unswallow(swallower->theme_object, object);
      evas_object_event_callback_del(object, EVAS_CALLBACK_FREE, _etk_widget_swallowed_object_deleted_cb);
      evas_object_data_del(object, "_Etk_Widget::Swallower");
      etk_widget_size_recalc_queue(swallower);
   }
   
   free(swo->part);
   free(swo);
   swallower->swallowed_objects = evas_list_remove_list(swallower->swallowed_objects, swo_node);
}

/* Adds a child from the widget */
static void _etk_widget_child_remove(Etk_Widget *parent, Etk_Widget *child)
{
   Evas_List *l;
   
   if (!parent || !child)
      return;

   if ((l = evas_list_find_list(parent->children, child)))
   {
      if (child->swallowed)
         etk_widget_unswallow_widget(parent, child);
      /* TODO: Unset the clip on child remove ? */
      if (child->clip && parent->clip == child->clip)
         etk_widget_clip_unset(child);
      
      parent->children = evas_list_remove_list(parent->children, l);
      etk_widget_size_recalc_queue(parent);
   }
}

/* Adds an object to the widget smart object */
static void _etk_widget_object_add_to_smart(Etk_Widget *widget, Evas_Object *object, Etk_Bool clip)
{
   if (!widget || !widget->smart_object || !object)
      return;
   
   if (!evas_object_visible_get(widget->smart_object))
      evas_object_hide(object);
   
   if (widget->clip/* && clip*/)
      evas_object_clip_set(object, widget->clip);
   evas_object_smart_member_add(object, widget->smart_object);
   evas_object_raise(object);
}

/* Adds "widget" to the list of widgets clipped by "clip" */
static void _etk_widget_add_to_clip(Etk_Widget *widget, Evas_Object *clip)
{
   Evas_List *clipped_widgets;
   
   if (!widget || !clip)
      return;
   
   if (!(clipped_widgets = evas_object_data_get(clip, "_Etk_Widget::Clipped_Widgets")))
   {
      clipped_widgets = evas_list_append(NULL, widget);
      evas_object_event_callback_add(clip, EVAS_CALLBACK_FREE, _etk_widget_clip_deleted_cb, NULL);
      evas_object_data_set(clip, "_Etk_Widget::Clipped_Widgets", clipped_widgets);
   }
   else if (!evas_list_find(clipped_widgets, widget))
      evas_list_append(clipped_widgets, widget);
}

/* Removes "widget" from the list of widgets clipped by "clip" */
static void _etk_widget_remove_from_clip(Etk_Widget *widget, Evas_Object *clip)
{
   Evas_List *clipped_widgets;
   
   if (!widget || !clip)
      return;
   
   if ((clipped_widgets = evas_object_data_get(widget->clip, "_Etk_Widget::Clipped_Widgets")))
   {
      Evas_List *widget_node;
      Etk_Bool need_update;
      
      if ((widget_node = evas_list_find_list(clipped_widgets, widget)))
      {
         need_update = (clipped_widgets == widget_node);
         clipped_widgets = evas_list_remove_list(clipped_widgets, widget_node);
         
         if (!clipped_widgets)
         {
            evas_object_data_del(widget->clip, "_Etk_Widget::Clipped_Widgets");
            evas_object_event_callback_del(widget->clip, EVAS_CALLBACK_FREE, _etk_widget_clip_deleted_cb);
         }
         else if (need_update)
         {
            evas_object_data_del(widget->clip, "_Etk_Widget::Clipped_Widgets");
            evas_object_data_set(widget->clip, "_Etk_Widget::Clipped_Widgets", clipped_widgets);
         }
      }
   }
}

/* Finds if an evas object is a member object of the widget */
static Evas_List *_etk_widget_member_object_find(Etk_Widget *widget, Evas_Object *object)
{
   Evas_List *l;
   Etk_Widget_Member_Object *m;
   
   if (!widget || !object)
      return NULL;
   
   for (l = widget->member_objects; l; l = l->next)
   {
      m = l->data;
      if (m->object == object)
         return l;
   }
   return NULL;
}

/* Called when a member object requests to be shown. Shows it only if all the parents are visible */
static void _etk_widget_member_object_intercept_show_cb(void *data, Evas_Object *obj)
{
   Etk_Widget *widget;
   Evas_List *l;
   Etk_Widget_Member_Object *member_object;
   
   if (!_etk_widget_intercept_show_hide)
   {
      evas_object_show(obj);
      return;
   }
   
   if (!(widget = ETK_WIDGET(data)))
      return;
   
   if ((l = _etk_widget_member_object_find(widget, obj)))
   {
      member_object = l->data;
      member_object->visible = ETK_TRUE;
   }
   
   if (widget->smart_object && evas_object_visible_get(widget->smart_object))
      evas_object_show(obj);
}


/* Called when a member object of the widget requests to be hidden */
static void _etk_widget_member_object_intercept_hide_cb(void *data, Evas_Object *obj)
{
   Evas_List *l;
   Etk_Widget *widget;
   Etk_Widget_Member_Object *member_object;

   evas_object_hide(obj);
   
   if (!_etk_widget_intercept_show_hide || !(widget = ETK_WIDGET(data)))
      return;
   
   if ((l = _etk_widget_member_object_find(widget, obj)))
   {
      member_object = l->data;
      member_object->visible = ETK_FALSE;
   }
}

/* Called when a member object of the widget is deleted */
static void _etk_widget_member_object_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Widget *widget;
   
   if (!(widget = ETK_WIDGET(data)))
      return;
   etk_widget_member_object_del(widget, obj);
}

/* Called when an object swallowed by the widget is deleted */
static void _etk_widget_swallowed_object_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Widget *widget;
   Evas_List *l;
   Etk_Widget_Swallowed_Object *swo;
   
   if (!(widget = ETK_WIDGET(data)))
      return;
   
   for (l = widget->swallowed_objects; l; l = l->next)
   {
      swo = l->data;
      if (swo->object == obj)
      {
         if (swo->widget)
         {
            swo->object = NULL;
            etk_signal_connect("realize", ETK_OBJECT(swo->widget),
               ETK_CALLBACK(_etk_widget_swallowed_widget_realize_cb), NULL);
         }
         else
            _etk_widget_unswallow_full(widget, l);
         
         break;
      }
   }
}

/* Called when the clip of the widget is deleted */
static void _etk_widget_clip_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_List *clipped_widgets, *l;
   Etk_Widget *widget;
   
   if (!obj || !(clipped_widgets = evas_object_data_get(obj, "_Etk_Widget::Clipped_Widgets")))
      return;
   
   for (l = clipped_widgets; l; l = l->next)
   {
      widget = ETK_WIDGET(l->data);
      widget->clip = NULL;
   }
}

/* Called when a swallowed widget is realized */
static void _etk_widget_swallowed_widget_realize_cb(Etk_Object *object, void *data)
{
   Etk_Widget *swallower, *swallowed;
   Evas_List *l;
   Etk_Widget_Swallowed_Object *swo;
   Evas_Object *obj;
   
   if (!(swallowed = ETK_WIDGET(object)) || !(swallower = swallowed->parent))
      return;
   
   for (l = swallower->swallowed_objects; l; l = l->next)
   {
      swo = l->data;
      if (swo->widget == swallowed)
      {
         if ((obj = swallowed->smart_object))
         {
            swo->object = obj;
            edje_object_part_swallow(swallower->theme_object, swo->part, obj);
            evas_object_event_callback_add(obj, EVAS_CALLBACK_FREE, _etk_widget_swallowed_object_deleted_cb, swallower);
            etk_widget_size_recalc_queue(swallower);
         }
         break;
      }
   }
   
   /* TODO: etk_signal may not like that?!!!! IMPORTANT TO FIX!!! */
   etk_signal_disconnect("realize", ETK_OBJECT(swallowed), ETK_CALLBACK(_etk_widget_swallowed_widget_realize_cb));
}

/* Creates a new smart object for the widget */
static Evas_Object *_etk_widget_smart_object_add(Evas *evas, Etk_Widget *widget)
{
   Evas_Object *new_object;
   
   if (!evas || !widget)
      return NULL;

   if (!_etk_widget_smart_object_smart)
   {
      _etk_widget_smart_object_smart = evas_smart_new("etk_widget_smart_object",
         NULL, /* add */
         NULL, /* del */
         NULL, /* layer_set */
         NULL, /* raise */
         NULL, /* lower */
         NULL, /* stack_above */
         NULL, /* stack_below */
         _etk_widget_smart_object_move_cb, /* move */
         _etk_widget_smart_object_resize_cb, /* resize */
         _etk_widget_smart_object_show_cb, /* show */
         _etk_widget_smart_object_hide_cb, /* hide */
         NULL, /* color_set */
         _etk_widget_smart_object_clip_set_cb, /* clip_set */
         _etk_widget_smart_object_clip_unset_cb, /* clip_unset */
         NULL); /* data*/
   }

   new_object = evas_object_smart_add(evas, _etk_widget_smart_object_smart);
   evas_object_smart_data_set(new_object, widget);
   return new_object;
}

/* Called when the smart object is moved */
static void _etk_widget_smart_object_move_cb(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   Etk_Widget *widget, *child;

   if (!obj || !(widget = ETK_WIDGET(evas_object_smart_data_get(obj))))
      return;

   if (x != widget->geometry.x || y != widget->geometry.y || widget->need_redraw)
   {
      Evas_List *l;
      Etk_Widget_Member_Object *m;
      Evas_Coord child_x, child_y;
      int x_offset, y_offset;

      x_offset = x - widget->geometry.x;
      y_offset = y - widget->geometry.y;
      widget->geometry.x = x;
      widget->geometry.y = y;
      widget->inner_geometry.x = widget->geometry.x + widget->left_inset;
      widget->inner_geometry.y = widget->geometry.y + widget->top_inset;

      if (widget->theme_object)
         evas_object_move(widget->theme_object, widget->geometry.x, widget->geometry.y);
      if (widget->event_object)
         evas_object_resize(widget->event_object, widget->geometry.w, widget->geometry.h);
      for (l = widget->member_objects; l; l = l->next)
      {
         m = l->data;
         evas_object_geometry_get(m->object, &child_x, &child_y, NULL, NULL);
         evas_object_move(m->object, child_x + x_offset, child_y + y_offset);
      }
      for (l = widget->children; l; l = l->next)
      {
         child = l->data;
         if (!child->swallowed)
         {
            evas_object_geometry_get(child->smart_object, &child_x, &child_y, NULL, NULL);
            evas_object_move(child->smart_object, child_x + x_offset, child_y + y_offset);
         }
      }
      
      etk_object_notify(ETK_OBJECT(widget), "geometry");
   }
}

/* Called when the smart object is resized */
static void _etk_widget_smart_object_resize_cb(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   Etk_Widget *widget;
   
   if (!obj || !(widget = ETK_WIDGET(evas_object_smart_data_get(obj))))
      return;

   if (w != widget->geometry.w || h != widget->geometry.h || widget->need_redraw)
   {
      widget->geometry.w = w;
      widget->geometry.h = h;
      widget->inner_geometry.w = widget->geometry.w - widget->left_inset - widget->right_inset;
      widget->inner_geometry.h = widget->geometry.h - widget->top_inset - widget->bottom_inset;

      if (widget->theme_object)
      {
         /* TODO: high: do we need the move here? */
         evas_object_move(widget->theme_object, widget->geometry.x, widget->geometry.y);
         evas_object_resize(widget->theme_object, widget->geometry.w, widget->geometry.h);
      }
      if (widget->event_object)
      {
         /* TODO: high: do we need the move here? */
         evas_object_move(widget->event_object, widget->geometry.x, widget->geometry.y);
         evas_object_resize(widget->event_object, widget->geometry.w, widget->geometry.h);
      }
      if (widget->size_allocate)
         widget->size_allocate(widget, widget->inner_geometry);
      
      widget->need_redraw = ETK_FALSE;
      etk_object_notify(ETK_OBJECT(widget), "geometry");
   }
}

/* Called when the smart object is shown */
static void _etk_widget_smart_object_show_cb(Evas_Object *obj)
{
   Evas_List *l;
   Etk_Widget_Member_Object *m;
   Etk_Widget *widget, *child;
   
   if (!obj || !(widget = ETK_WIDGET(evas_object_smart_data_get(obj))))
      return;
   
   if (widget->theme_object)
      evas_object_show(widget->theme_object);
   if (widget->event_object)
      evas_object_show(widget->event_object);
   for (l = widget->member_objects; l; l = l->next)
   {
      m = l->data;
      if (m->visible)
      {
         _etk_widget_intercept_show_hide = ETK_FALSE;
         evas_object_show(m->object);
         _etk_widget_intercept_show_hide = ETK_TRUE;
      }
   }
   for (l = widget->children; l; l = l->next)
   {
      child = l->data;
      if (child->visible)
         evas_object_show(child->smart_object);
   }
}

/* Called when the smart object is hidden */
static void _etk_widget_smart_object_hide_cb(Evas_Object *obj)
{
   Evas_List *l;
   Etk_Widget_Member_Object *m;
   Etk_Widget *widget, *child;
   
   if (!obj || !(widget = ETK_WIDGET(evas_object_smart_data_get(obj))))
      return;
   
   if (widget->theme_object)
      evas_object_hide(widget->theme_object);
   if (widget->event_object)
      evas_object_hide(widget->event_object);
   for (l = widget->member_objects; l; l = l->next)
   {
      m = l->data;
      _etk_widget_intercept_show_hide = ETK_FALSE;
      evas_object_hide(m->object);
      _etk_widget_intercept_show_hide = ETK_TRUE;
   }
   for (l = widget->children; l; l = l->next)
   {
      child = l->data;
      evas_object_hide(child->smart_object);
   }
}

/* Called when a clip is set to the smart object */
static void _etk_widget_smart_object_clip_set_cb(Evas_Object *object, Evas_Object *clip)
{
   Etk_Widget *widget;

   if (!object || !clip || !(widget = ETK_WIDGET(evas_object_smart_data_get(object))))
      return;

   if (widget->clip)
      etk_widget_clip_unset(widget);

   if (widget->theme_object)
      evas_object_clip_set(widget->theme_object, clip);
   if (widget->event_object)
      evas_object_clip_set(widget->event_object, clip);
   
   if (widget->clip_set)
      widget->clip_set(widget, clip);
   else
   {
      Etk_Widget *child;
      Etk_Widget_Member_Object *member_object;
      Evas_List *l;
      
      for (l = widget->member_objects; l; l = l->next)
      {
         member_object = l->data;
         if (member_object->object != clip)
            evas_object_clip_set(member_object->object, clip);
      }
      for (l = widget->children; l; l = l->next)
      {
         child = ETK_WIDGET(l->data);
         if (!child->swallowed)
            etk_widget_clip_set(child, clip);
      }
   }
   
   _etk_widget_add_to_clip(widget, clip);
   widget->clip = clip;
}

/* Called when the clip of the smart object is unset */
static void _etk_widget_smart_object_clip_unset_cb(Evas_Object *object)
{
   Etk_Widget *widget;

   if (!object || !(widget = ETK_WIDGET(evas_object_smart_data_get(object))) || !widget->clip)
      return;

   if (widget->theme_object)
      evas_object_clip_unset(widget->theme_object);
   if (widget->event_object)
      evas_object_clip_unset(widget->event_object);
   
   if (widget->clip_unset)
      widget->clip_unset(widget);
   else
   {
      Etk_Widget *child;
      Etk_Widget_Member_Object *member_object;
      Evas_List *l;
      
      for (l = widget->member_objects; l; l = l->next)
      {
         member_object = l->data;
         evas_object_clip_unset(member_object->object);
      }
      for (l = widget->children; l; l = l->next)
      {
         child = ETK_WIDGET(l->data);
         if (!child->swallowed)
            etk_widget_clip_unset(child);
      }
   }
   
   _etk_widget_remove_from_clip(widget, widget->clip);
   widget->clip = NULL;
}

static void _etk_widget_dnd_drag_mouse_move_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Mouse_Move *ev;
   
   ev = event;

   if(ev->buttons & 0x001 && _etk_dnd_drag_start)
   {
      const char **types;
      unsigned int num_types;
      char *data;
      Etk_Widget *drag;
      
      drag = (ETK_WIDGET(object))->drag;
      
      _etk_dnd_drag_start = ETK_FALSE;
      types = calloc(1, sizeof(char*));
      num_types = 1;
      types[0] = strdup("text/plain");
      data = strdup("This is the drag data!");
      
      //etk_drag_types_set(drag, types, num_types);
      //etk_drag_data_set(drag, data, strlen(data) + 1);
      etk_drag_begin(ETK_DRAG(drag));
   }
}

static void _etk_widget_dnd_drag_end_cb(Etk_Object *object, void *data)
{
   _etk_dnd_drag_start = ETK_TRUE;   
}

/** @} */
