/** @file etk_widget.c */
#include "etk_widget.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <Evas.h>
#include <Edje.h>
#include "etk_theme.h"
#include "etk_container.h"
#include "etk_toplevel.h"
#include "etk_drag.h"
#include "etk_event.h"
#include "etk_marshallers.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Widget
 * @{
 */

#define ETK_WIDGET_CONTENT_PART      "etk.swallow.content"

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
   ETK_WIDGET_SELECTION_RECEIVED_SIGNAL,
   ETK_WIDGET_DRAG_DROP_SIGNAL,
   ETK_WIDGET_DRAG_ENTER_SIGNAL,     
   ETK_WIDGET_DRAG_MOTION_SIGNAL,
   ETK_WIDGET_DRAG_LEAVE_SIGNAL,
   ETK_WIDGET_DRAG_BEGIN_SIGNAL,     
   ETK_WIDGET_DRAG_END_SIGNAL,
   ETK_WIDGET_NUM_SIGNALS
};

enum Etk_Widget_Property_Id
{
   ETK_WIDGET_PARENT_PROPERTY,
   ETK_WIDGET_THEME_FILE_PROPERTY,
   ETK_WIDGET_THEME_GROUP_PROPERTY,
   ETK_WIDGET_THEME_PARENT_PROPERTY,
   ETK_WIDGET_GEOMETRY_PROPERTY,
   ETK_WIDGET_WIDTH_REQUEST_PROPERTY,
   ETK_WIDGET_HEIGHT_REQUEST_PROPERTY,
   ETK_WIDGET_VISIBLE_PROPERTY,
   ETK_WIDGET_INTERNAL_PROPERTY,
   ETK_WIDGET_REPEAT_MOUSE_EVENTS_PROPERTY,
   ETK_WIDGET_PASS_MOUSE_EVENTS_PROPERTY,
   ETK_WIDGET_HAS_EVENT_OBJECT_PROPERTY,
   ETK_WIDGET_FOCUSABLE_PROPERTY,
   ETK_WIDGET_FOCUS_ON_CLICK_PROPERTY
};

static void _etk_widget_constructor(Etk_Widget *widget);
static void _etk_widget_destructor(Etk_Widget *widget);
static void _etk_widget_destroyed_cb(Etk_Object *object, void *data);
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
static void _etk_widget_mouse_out_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_widget_mouse_move_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_widget_mouse_down_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_widget_mouse_up_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_widget_mouse_wheel_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_widget_signal_mouse_down_cb(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data);
static void _etk_widget_signal_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);

static void _etk_widget_toplevel_evas_changed_cb(Etk_Object *object, const char *property_name, void *data);

static void _etk_widget_realize(Etk_Widget *widget);
static void _etk_widget_unrealize(Etk_Widget *widget);
static void _etk_widget_theme_group_full_update(Etk_Widget *widget);

static void _etk_widget_toplevel_parent_set(Etk_Widget *widget, Etk_Toplevel *toplevel_parent);
static void _etk_widget_realize_children(Etk_Widget *widget, Etk_Bool realize, Etk_Bool evas_changed);
static void _etk_widget_realize_theme_children(Etk_Widget *widget, Etk_Bool realize, Etk_Bool theme_group_changed);
static void _etk_widget_unrealize_all(Etk_Widget *widget);

static void _etk_widget_theme_min_size_calc(Etk_Widget *widget, int *w, int *h, Etk_Bool hidden_has_no_size);
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

static void _etk_widget_dnd_drag_mouse_move_cb(Etk_Object *object, void *event, void *data);
static void _etk_widget_dnd_drag_end_cb(Etk_Object *object, void *data);

static Evas_Object *_etk_widget_smart_object_add(Evas *evas, Etk_Widget *widget);
static void _etk_widget_smart_object_move_cb(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _etk_widget_smart_object_resize_cb(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _etk_widget_smart_object_show_cb(Evas_Object *obj);
static void _etk_widget_smart_object_hide_cb(Evas_Object *obj);
static void _etk_widget_smart_object_clip_set_cb(Evas_Object *object, Evas_Object *clip);
static void _etk_widget_smart_object_clip_unset_cb(Evas_Object *object);

static Evas_Object *_etk_widget_content_object_add(Evas *evas, Etk_Widget *widget);
static void _etk_widget_content_object_move_cb(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _etk_widget_content_object_resize_cb(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _etk_widget_content_object_clip_set_cb(Evas_Object *obj, Evas_Object *clip);
static void _etk_widget_content_object_clip_unset_cb(Evas_Object *obj);  


static Etk_Signal *_etk_widget_signals[ETK_WIDGET_NUM_SIGNALS];
static Etk_Bool _etk_widget_intercept_show_hide = ETK_TRUE;
static Evas_Smart *_etk_widget_smart_object_smart = NULL;
static Evas_Smart *_etk_widget_content_object_smart = NULL;
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
 * @internal
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
      _etk_widget_signals[ETK_WIDGET_SELECTION_RECEIVED_SIGNAL] = etk_signal_new("selection_received",
         widget_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
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
      etk_type_property_add(widget_type, "internal", ETK_WIDGET_INTERNAL_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(widget_type, "focusable", ETK_WIDGET_FOCUSABLE_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(widget_type, "focus_on_click", ETK_WIDGET_FOCUS_ON_CLICK_PROPERTY,
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
 * @param widget_type the type of widget to create
 * @param first_property the name of the first property to set
 * @param ... the value of the first property, followed by any number of property-name/property-value pairs,
 * terminated with NULL
 * @return Returns the new Etk_Widget of type @a widget_type
 * @note This function just calls etk_object_new() and casts the result to an "Etk_Widget *"
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
 * @brief Gets the geometry of the widget, relative to the top-left corner of the Evas where it is drawn
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
 * @brief Gets the inner geometry of the widget, relative to the top-left corner of the Evas where it is drawn. @n
 * The inner geometry takes the inset values (horizontal and vertical paddings) into account
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
 * @return Returns the toplevel widget if @a widget is contained by a toplevel widget, or NULL otherwise
 */
Etk_Toplevel *etk_widget_toplevel_parent_get(Etk_Widget *widget)
{
   if (!widget)
      return NULL;
   return widget->toplevel_parent;
}

/**
 * @brief Gets the evas of the toplevel widget that contains @a widget
 * @param widget a widget
 * @return Returns the evas if @a widget is contained by a toplevel widget, or NULL otherwise
 */
Evas *etk_widget_toplevel_evas_get(Etk_Widget *widget)
{
   if (!widget || !widget->toplevel_parent)
      return NULL;
   return etk_toplevel_evas_get(widget->toplevel_parent);
}

/**
 * @brief Changes the theme of the widget: it sets both the theme-file and the theme-group in a more optimized way
 * than just calling etk_widget_theme_file_set() and then etk_widget_theme_group_set(). But if you only want to change
 * the theme-file OR the theme-group, you should better call directly the corresponding function
 * (etk_widget_theme_file_set() or etk_widget_theme_group_set())
 * @param widget a widget
 * @param theme_file the path of the theme-file. If NULL, it will use the first non-NULL theme-file of its parents.
 * If none of its parents has a non-NULL theme-file, the current theme-file of Etk. @n
 * The new theme-file will also be automatically used by all the children of @widget, as long as they have a
 * theme-file set to NULL
 * @param theme_group the theme-group to use
 */
void etk_widget_theme_set(Etk_Widget *widget, const char *theme_file, const char *theme_group)
{
   if (!widget)
      return;

   if (theme_file != widget->theme_file)
   {
      free(widget->theme_file);
      widget->theme_file = theme_file ? strdup(theme_file) : NULL;
   }
   if (theme_group != widget->theme_group)
   {
      free(widget->theme_group);
      widget->theme_group = theme_group ? strdup(theme_group) : NULL;
      _etk_widget_theme_group_full_update(widget);
   }
   
   _etk_widget_realize_children(widget, ETK_TRUE, ETK_FALSE);
   _etk_widget_realize_theme_children(widget, ETK_FALSE, ETK_TRUE);
   etk_object_notify(ETK_OBJECT(widget), "theme_file");
   etk_object_notify(ETK_OBJECT(widget), "theme_group");
}

/**
 * @brief Sets the theme-file used by the widget. The widget will be automatically re-realized
 * @param widget a widget
 * @param theme_file the path of the theme-file. If NULL, it will use the first non-NULL theme-file of its parents.
 * If none of its parents has a non-NULL theme-file, the current theme-file of Etk. @n
 * The new theme-file will also be automatically used by all the children of @widget, as long as they have a
 * theme-file set to NULL
 */
void etk_widget_theme_file_set(Etk_Widget *widget, const char *theme_file)
{
   if (!widget)
      return;

   if (theme_file != widget->theme_file)
   {
      free(widget->theme_file);
      widget->theme_file = theme_file ? strdup(theme_file) : NULL;
      
      _etk_widget_realize_children(widget, ETK_TRUE, ETK_FALSE);
      _etk_widget_realize_theme_children(widget, ETK_FALSE, ETK_FALSE);
      etk_object_notify(ETK_OBJECT(widget), "theme_file");
   }
}

/**
 * @brief Gets the theme-file used by the widget
 * @param widget a widget
 * @return Returns the path to the theme-file used by the widget (NULL if the widget uses the current theme of Etk)
 * @note The returned value may be different from the theme-file set with etk_widget_theme_file_set(): if
 * @a widget->theme_file is NULL, etk_widget_theme_file_get() will look recursively for the first non-NULL theme-file of
 * its parents. If none of its parents have a non-NULL theme-file, etk_widget_theme_file_get() will return NULL
 * (meaning that @a widget uses the current theme of Etk). @n
 * To get the value set with etk_widget_theme_file_set(), you can use
 * etk_object_properties_get(ETK_OBJECT(widget), "theme_file", &theme_file);
 */
const char *etk_widget_theme_file_get(Etk_Widget *widget)
{
   Etk_Widget *parent;
   
   for (parent = widget; parent; parent = parent->theme_parent ? parent->theme_parent : parent->parent)
   {
      if (parent->theme_file)
         return parent->theme_file;
   }
   
   return NULL;
}

/**
 * @brief Sets the theme-group of the widget. The theme-group of a widget is also used by the theme-children of the
 * widget. For example, the theme-group of a tree is "tree", and the theme-group of its rows is "row". Since the rows
 * are theme-children of the tree, the edje-group really loaded for the rows is "etk/tree/rows". It also means
 * that if you change the theme-group of the tree later to "new_tree" in order to change its appearance, the appearance
 * of the rows will also be updated (the rows will use automatically the edje-group called "etk/new_tree/rows").
 * @param widget a widget
 * @param theme_group the theme-group to use
 */
void etk_widget_theme_group_set(Etk_Widget *widget, const char *theme_group)
{
   if (!widget)
      return;

   if (theme_group != widget->theme_group)
   {
      free(widget->theme_group);
      widget->theme_group = theme_group ? strdup(theme_group) : NULL;
      
      _etk_widget_theme_group_full_update(widget);
      _etk_widget_realize_theme_children(widget, ETK_TRUE, ETK_TRUE);
      etk_object_notify(ETK_OBJECT(widget), "theme_group");
   }
}

/**
 * @brief Gets the full theme-group of the widget
 * @param widget a widget
 * @return Returns the full theme-group of the widget
 * @note The returned value may be different from the theme-group set with etk_widget_theme_group_set(): if
 * @a widget has a theme-parent, this will be taken into account. For example, if @a widget is a row, and
 * the theme-parent of the widget is a tree, the returned value will be "tree/row", and not just "row". @n
 * To get the value set with etk_widget_theme_group_set(), you can use
 * etk_object_properties_get(ETK_OBJECT(widget), "theme_group", &theme_group);
 */
const char *etk_widget_theme_group_get(Etk_Widget *widget)
{
   if (!widget)
      return NULL;
   return widget->theme_group_full;
}

/**
 * @brief Sets the theme-parent of the widget. The widget will automatically inherit the theme-group from the
 * theme-parent. See etk_widget_theme_group_set() for more info.
 * @param widget a widget
 * @param theme_parent The theme-parent to use
 * @widget_implementation
 * @see etk_widget_theme_group_set()
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
   
   _etk_widget_theme_group_full_update(widget);
   _etk_widget_realize_children(widget, ETK_TRUE, ETK_FALSE);
   _etk_widget_realize_theme_children(widget, ETK_FALSE, ETK_TRUE);
   etk_object_notify(ETK_OBJECT(widget), "theme_parent");
}

/**
 * @brief Gets the theme-parent of the widget
 * @param widget a widget
 * @return Returns the theme-parent of the widget
 * @widget_implementation
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
 * @note If you want to add a widget to a container, use etk_container_add() instead
 */
void etk_widget_parent_set(Etk_Widget *widget, Etk_Widget *parent)
{
   etk_widget_parent_set_full(widget, parent, ETK_TRUE);
}

/**
 * @brief Sets the parent of the widget
 * @param widget a widget
 * @param parent the new parent
 * @param remove_from_container if @a remove_from_container is ETK_TRUE and if the current parent of the widget is a
 * container, the child_remove() function of the parent container will be called. So @a remove_from_container should
 * most of the time be set to ETK_TRUE, except when etk_widget_parent_set_full() is called from the child_remove()
 * function of a container, in order to avoid an infinite loop.
 * @widget_implementation
 * @note If you want to add a widget to a container, use etk_container_add() instead
 */
void etk_widget_parent_set_full(Etk_Widget *widget, Etk_Widget *parent, Etk_Bool remove_from_container)
{
   Etk_Toplevel *prev_toplevel;
   Evas *new_evas, *prev_evas;
   Etk_Widget *toplevel;
   const char *prev_theme_file, *new_theme_file;

   if (!widget || widget->parent == parent || ETK_IS_TOPLEVEL(widget))
      return;
   
   prev_evas = etk_widget_toplevel_evas_get(widget);
   prev_theme_file = etk_widget_theme_file_get(widget);
   
   if (widget->parent)
   {
      if (remove_from_container && ETK_IS_CONTAINER(widget->parent))
         etk_container_remove(ETK_CONTAINER(widget->parent), widget);
      _etk_widget_child_remove(widget->parent, widget);
   }
   if (parent)
      parent->children = evas_list_append(parent->children, widget);
   widget->parent = parent;
   
   prev_toplevel = widget->toplevel_parent;
   for (toplevel = widget; toplevel->parent; toplevel = toplevel->parent);
   if (ETK_IS_TOPLEVEL(toplevel))
      widget->toplevel_parent = ETK_TOPLEVEL(toplevel);
   else
      widget->toplevel_parent = NULL;
   
   if ((widget->toplevel_parent != prev_toplevel))
      _etk_widget_toplevel_parent_set(widget, widget->toplevel_parent);
   
   new_evas = etk_widget_toplevel_evas_get(widget);
   new_theme_file = etk_widget_theme_file_get(widget);;
   
   /* Realize/unrealize the widget and its children */
   if (new_evas)
   {
      Etk_Bool same_theme_file;
      
      same_theme_file = ((prev_theme_file == new_theme_file)
         || (prev_theme_file && new_theme_file && strcmp(prev_theme_file, new_theme_file) == 0));
      
      if ((new_evas != prev_evas))
         _etk_widget_realize_children(widget, ETK_TRUE, ETK_TRUE);
      if (!same_theme_file)
         _etk_widget_realize_theme_children(widget, (new_evas == prev_evas), ETK_FALSE);
   }
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
 * @brief Sets whether the widget should use an event-object. An event-object is an invisible rectangle that
 * is used to grab the mouse events on the widget. It can be useful for example if you want a container
 * with no theme-object (a table, a box, ...) to receive the mouse events. @n
 * If @a widget already has a theme-object, this function has no effect (the theme-object is already used to
 * grab the mouse events)
 * @param widget a widget
 * @param has_event_object ETK_TRUE to make the widget use an event-object, ETK_FALSE otherwise
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
         evas_object_color_set(widget->event_object, 0, 0, 0, 0);
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
 * @brief Gets whether the widget uses an event-object
 * @param widget a widget
 * @return Returns ETK_TRUE if the widget uses an event-object
 */
Etk_Bool etk_widget_has_event_object_get(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->has_event_object;
}

/**
 * @brief Sets whether the mouse-events received by the widget should be propagated to its parent
 * @param widget a widget
 * @param repeat_mouse_events if @a repeat_mouse_events is ETK_TRUE, the parent
 * widget will also receive the mouse-events
 */
void etk_widget_repeat_mouse_events_set(Etk_Widget *widget, Etk_Bool repeat_mouse_events)
{
   if (!widget || widget->repeat_mouse_events == repeat_mouse_events)
      return;

   widget->repeat_mouse_events = repeat_mouse_events;
   etk_object_notify(ETK_OBJECT(widget), "repeat_mouse_events");
}

/**
 * @brief Checks whether the mouse-events received by the widget are propagated to its parent
 * @param widget a widget
 * @return Returns ETK_TRUE if the mouse-events received by the widget are propagated to its parent
 */
Etk_Bool etk_widget_repeat_mouse_events_get(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->repeat_mouse_events;
}

/**
 * @brief Sets whether the widget should pass directly to its parent the mouse-events it receives
 * @param widget a widget
 * @param pass_mouse_events if @a pass_mouse_events is ETK_TRUE, the mouse-events will be directly sent to the parent.
 * @a widget won't receive mouse-events anymore
 */
void etk_widget_pass_mouse_events_set(Etk_Widget *widget, Etk_Bool pass_mouse_events)
{
   if (!widget || widget->pass_mouse_events == pass_mouse_events)
      return;

   widget->pass_mouse_events = pass_mouse_events;
   if (widget->smart_object)
      evas_object_pass_events_set(widget->smart_object, pass_mouse_events);
   etk_object_notify(ETK_OBJECT(widget), "pass_mouse_events");
}

/**
 * @brief Checks whether the widget passes directly to its parent the mouse-events it receives
 * @param widget a widget
 * @return Returns ETK_TRUE if the widget passes directly to its parent the mouse-events it receives
 */
Etk_Bool etk_widget_pass_mouse_events_get(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->pass_mouse_events;
}

/**
 * @brief Sets whether or not the widget is an internal widget: an internal widget is not affected by
 * etk_widget_show_all() and etk_widget_hide_all(). It prevents visibility issues when one calls
 * etk_widget_hide_all(widget) and then etk_widget_show(widget). @n
 * For example, if the label of a button wasn't an internal widget, calling etk_widget_hide_all(button) and then
 * etk_widget_show(button) would make the label disappear since the label is a child of the button.
 * @param widget a widget
 * @param internal ETK_TRUE to prevent the widget to be affected by etk_widget_show_all() and etk_widget_hide_all()
 * @widget_implementation
 */
void etk_widget_internal_set(Etk_Widget *widget, Etk_Bool internal)
{
   if (!widget || widget->internal == internal)
      return;
   
   widget->internal = internal;
   etk_object_notify(ETK_OBJECT(widget), "internal");
}

/**
 * @brief Gets whether the widget is an internal widget. See etk_widget_internal_set() for more info
 * about internal widgets
 * @param widget a widget
 * @return Returns ETK_TRUE if the widget is an internal widget, ETK_FALSE otherwise
 * @see etk_widget_internal_get()
 */
Etk_Bool etk_widget_internal_get(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->internal;
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
   
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_SHOW_SIGNAL], ETK_OBJECT(widget), NULL);
   etk_object_notify(ETK_OBJECT(widget), "visible");
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

   if (!widget->internal)
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
   
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_HIDE_SIGNAL], ETK_OBJECT(widget), NULL);
   etk_object_notify(ETK_OBJECT(widget), "visible");
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

   if (!widget->internal)
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
   
   /* Lower the smart-object */
   if (widget->smart_object)
   {
      if (widget->parent->content_object)
         evas_object_lower(widget->smart_object);
      else if (widget->parent->theme_object)
         evas_object_stack_above(widget->smart_object, widget->theme_object);
      else if (widget->parent->event_object)
         evas_object_stack_above(widget->smart_object, widget->event_object);
      else
         evas_object_lower(widget->smart_object);
   }
}

/**
 * @brief Queues a size recalculation request: during the next main-loop iteration, the size of the widget will be
 * recalculated, and the widget will be redrawn
 * @param widget the widget to queue
 * @widget_implementation
 */
void etk_widget_size_recalc_queue(Etk_Widget *widget)
{
   Etk_Widget *w;

   if (!widget)
      return;

   for (w = widget; w; w = ETK_WIDGET(w->parent))
   {
      if ((w->swallowed || w->content_object) && w->parent)
         w->parent->need_theme_size_recalc = ETK_TRUE;
      w->need_size_recalc = ETK_TRUE;
      w->need_redraw = ETK_TRUE;
   }
   
   if (widget->toplevel_parent)
      widget->toplevel_parent->need_update = ETK_TRUE;
}

/**
 * @brief Queues a redraw request: during the next main-loop iteration, the widget and its children will be redrawn
 * (i.e. etk_widget_size_allocate() will be called on them)
 * @param widget the widget to queue
 * @widget_implementation
 */
void etk_widget_redraw_queue(Etk_Widget *widget)
{
   if (!widget)
      return;

   _etk_widget_redraw_queue_recursive(widget);
   if (widget->toplevel_parent)
      widget->toplevel_parent->need_update = ETK_TRUE;
}

/**
 * @brief Sets the size-request of the widget. Use this function to request Etk to resize the widget to the given size. @n
 * It does not mean the widget will be allocated this size: in some cases, the widget may be allocated less than its
 * size-request, and often the allocated size can be larger than the size-request (if the widget expands for example). @n
 * You can use this function to force a widget to be smaller or larger than its default size
 * @param widget a widget
 * @param w the width to request (-1 will make Etk calculate it automatically)
 * @param h the height to request (-1 will make Etk calculate it automatically)
 */
void etk_widget_size_request_set(Etk_Widget *widget, int w, int h)
{
   if (!widget)
      return;

   widget->requested_size.w = w;
   widget->requested_size.h = h;
   
   etk_widget_size_recalc_queue(widget);
   etk_object_notify(ETK_OBJECT(widget), "width_request");
   etk_object_notify(ETK_OBJECT(widget), "height_request");
}

/**
 * @brief Calculates the size-request of the widget
 * @param widget a widget
 * @param size_requisition the location where to store the requested size
 * @note etk_widget_size_request_full(widget, size_request) is equivalent to
 * etk_widget_size_request_full(widget, size_request, ETK_TRUE)
 * @widget_implementation
 */
void etk_widget_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   etk_widget_size_request_full(widget, size_requisition, ETK_TRUE);
}

/**
 * @brief Calculates the size-request of the widget
 * @param widget a widget
 * @param size_requisition the location where to store the ideal size
 * @param hidden_has_no_size if @a hidden_has_no_size is ETK_TRUE, then if the widget is hidden,
 * the returned size will be 0x0. If it is ETK_FALSE, the size-request will be calculated even if the widget is hidden
 * @widget_implementation
 */
void etk_widget_size_request_full(Etk_Widget *widget, Etk_Size *size_requisition, Etk_Bool hidden_has_no_size)
{
   if (!widget || !size_requisition)
      return;

   size_requisition->w = -1;
   size_requisition->h = -1;
   
   if (!widget->realized)
   {
      size_requisition->w = 0;
      size_requisition->h = 0;
      return;
   }

   if (!widget->visible && hidden_has_no_size)
      size_requisition->w = 0;
   else if (widget->requested_size.w >= 0)
      size_requisition->w = widget->requested_size.w;
   else if (widget->last_calced_size.w >= 0
      && !widget->need_size_recalc && (widget->visible || hidden_has_no_size))
   {
      size_requisition->w = widget->last_calced_size.w;
   }
   
   if (!widget->visible && hidden_has_no_size)
      size_requisition->h = 0;
   else if (widget->requested_size.h >= 0)
      size_requisition->h = widget->requested_size.h;
   else if (widget->last_calced_size.h >= 0
      && !widget->need_size_recalc && (widget->visible || hidden_has_no_size))
   {
      size_requisition->h = widget->last_calced_size.h;
   }

   /* We need to recalc it */
   if (size_requisition->w < 0 || size_requisition->h < 0)
   {
      int min_w, min_h;

      _etk_widget_theme_min_size_calc(widget, &min_w, &min_h, hidden_has_no_size);
      if (!widget->content_object && widget->size_request)
      {
         Etk_Size widget_requisition;

         widget->size_request(widget, &widget_requisition);
         if (size_requisition->w < 0)
            size_requisition->w = ETK_MAX(min_w, widget_requisition.w + widget->inset.left + widget->inset.right);
         if (size_requisition->h < 0)
            size_requisition->h = ETK_MAX(min_h, widget_requisition.h + widget->inset.top + widget->inset.bottom);
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
      widget->last_calced_size = *size_requisition;
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
 * @brief Sets whether or not the widget can be focused
 * @param widget a widget
 * @param focusable ETK_TRUE to make @a widget focusable, ETK_FALSE otherwise
 */
void etk_widget_focusable_set(Etk_Widget *widget, Etk_Bool focusable)
{
   if (!widget || widget->focusable == focusable)
      return;
   
   if (!focusable && etk_widget_is_focused(widget))
      etk_widget_unfocus(widget);
   widget->focusable = focusable;
   etk_object_notify(ETK_OBJECT(widget), "focusable");
}

/**
 * @brief Gets whether the widget can be focused or not
 * @param widget a widget
 * @return Returns ETK_TRUE if the widget is focusable, ETK_FALSE otherwise
 */
Etk_Bool etk_widget_focusable_get(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->focusable;
}

/**
 * @brief Focuses the widget. The focused widget is the one which receives the keyboard events
 * @param widget a widget
 * @note The widget has to be attached to a toplevel widget, otherwise it will have no effect
 */
void etk_widget_focus(Etk_Widget *widget)
{
   Etk_Widget *focused;

   if (!widget || !widget->toplevel_parent || !widget->focusable)
      return;
   if ((focused = etk_toplevel_focused_widget_get(widget->toplevel_parent)) && (widget == focused))
      return;

   if (focused)
      etk_widget_unfocus(focused);

   etk_toplevel_focused_widget_set(widget->toplevel_parent, widget);
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_FOCUS_SIGNAL], ETK_OBJECT(widget), NULL);
}

/**
 * @brief Unfocuses the widget
 * @param widget a widget
 */
void etk_widget_unfocus(Etk_Widget *widget)
{
   if (!widget || !widget->toplevel_parent || !etk_widget_is_focused(widget))
      return;

   etk_toplevel_focused_widget_set(widget->toplevel_parent, NULL);
   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_UNFOCUS_SIGNAL], ETK_OBJECT(widget), NULL);
}

/**
 * @brief Gets whether the widget is focused
 * @param widget a widget
 * @return Returns ETK_TRUE if @a widget is focused, ETK_FALSE otherwise
 */
Etk_Bool etk_widget_is_focused(Etk_Widget *widget)
{
   if (!widget || !widget->toplevel_parent)
      return ETK_FALSE;
   return (etk_toplevel_focused_widget_get(widget->toplevel_parent) == widget);
}

/**
 * @brief Sends a signal to the theme-object of the widget
 * @param widget a widget
 * @param signal_name the signal to send
 * @param size_recalc if @a size_recalc is ETK_TRUE, etk_widget_size_recalc_queue() will be called on the widget.
 * So, if the emitted signal is likely to change the size of the widget, @a size_recalc has to be set to ETK_TRUE.
 * Otherwise it should be set to ETK_FALSE
 * @note The widget has to be realized, otherwise it will have no effect
 * @widget_implementation
 * @see edje_object_signal_emit()
 */
void etk_widget_theme_signal_emit(Etk_Widget *widget, const char *signal_name, Etk_Bool size_recalc)
{
   if (!widget || !widget->theme_object)
      return;
   edje_object_signal_emit(widget->theme_object, signal_name, "etk");
   if (size_recalc)
   {
      widget->need_theme_size_recalc = ETK_TRUE;
      etk_widget_size_recalc_queue(widget);
   }
}

/**
 * @brief Sets the text of a text-part of the widget's theme-object.
 * @param widget a widget
 * @param part_name the name of the text-part
 * @param text the text to set
 * @note The widget has to be realized, otherwise it will have no effect
 * @note etk_widget_size_recalc_queue() is automatically called on the widget
 * @widget_implementation
 * @see edje_object_part_text_set()
 */
void etk_widget_theme_part_text_set(Etk_Widget *widget, const char *part_name, const char *text)
{
   if (!widget || !widget->theme_object)
      return;
   edje_object_part_text_set(widget->theme_object, part_name, text);
   widget->need_theme_size_recalc = ETK_TRUE;
   etk_widget_size_recalc_queue(widget);
}

/**
 * @brief Gets the data called @a data_name from the theme of the widget
 * @param widget a widget
 * @param data_name the name of the data you want to get
 * @param format the format of the data. Same format as the format of sscanf()
 * @param ... the location of the variables where to store the values
 * @return Returns the number of the input items successfully matched and assigned, same as sscanf
 * @note The widget has to be realized, otherwise it will have no effect
 * @widget_implementation
 * @see edje_object_data_get()
 */
int etk_widget_theme_data_get(Etk_Widget *widget, const char *data_name, const char *format, ...)
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
 * @brief Makes the widget swallow another widget in a part of its theme-object
 * @param swallower the widget that will swallow the widget @a to_swallow. @a swallower has to
 * be realized and its theme-object should have a part called @a part
 * @param part the name of the part of the theme-object in which @a to_swallow should be swallowed
 * @param to_swallow the widget to swallow. @a to_swallow has to be a child of @a swallower
 * @return Returns ETK_TRUE on success, ETK_FALSE on failure (it may occur if @a swallower is not realized, if the part
 * doesn't exist or if @a to_swallow is not a child of @a swallower)
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
 * @brief Makes the widget @a swallower unswallow another widget
 * @param swallower the widget that is currently swallowing @a swallowed
 * @param swallowed the widget currently swallowed by @a swallower, and which should be unswallowed
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
 * @brief Makes the widget swallow an Evas_Object in a part of its theme-object
 * @param swallower the widget that will swallow the object @a to_swallow. @a swallower has to
 * be realized and to have a theme-object
 * @param part the name of the part of the theme-object in which @a to_swallow should be swallowed
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
 * @brief Gets the code corresponding to the error that occurs during the last call of
 * etk_widget_swallow_widget() or etk_widget_swallow_object()
 * @return Returns the error code
 * @widget_implementation
 */
Etk_Widget_Swallow_Error etk_widget_swallow_error_get(void)
{
   return _etk_widget_swallow_error;
}

/**
 * @brief Adds an evas object to the list of member-objects of the widget. @n
 * A member-object is an object which is automatically shown/hidden when the widget is shown/hidden,
 * which is clipped against the clip-object of the widget, and which is used by the widget to receive the mouse
 * events. A member-object is also stacked on the same level as the widget. @n
 * The object will be automatically deleted when the object is unrealized, unless you remove it from the list
 * with etk_widget_member_object_del() before the widget is unrealized.
 * @param widget a widget
 * @param object the evas object to add
 * @return Returns ETK_TRUE on success. ETK_FALSE on failure, probably because the widget and the object do not
 * belong to the same evas, or because the widget is not realized yet
 * @note The object has to belong to the same evas as the widget
 * @widget_implementation
 */
Etk_Bool etk_widget_member_object_add(Etk_Widget *widget, Evas_Object *object)
{
   Etk_Widget_Member_Object *member_object;
   Etk_Widget *prev_widget;
   
   if (!widget || !object || !widget->realized
      || (evas_object_evas_get(object) != etk_widget_toplevel_evas_get(widget)))
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
   
   _etk_widget_object_add_to_smart(widget, object, (evas_object_clip_get(object) == NULL));
   evas_object_intercept_show_callback_add(object, _etk_widget_member_object_intercept_show_cb, widget);
   evas_object_intercept_hide_callback_add(object, _etk_widget_member_object_intercept_hide_cb, widget);
   evas_object_event_callback_add(object, EVAS_CALLBACK_FREE, _etk_widget_member_object_deleted_cb, widget);
   evas_object_data_set(object, "_Etk_Widget::Parent", widget);
   
   widget->member_objects = evas_list_append(widget->member_objects, member_object);

   return ETK_TRUE;
}

/**
 * @brief Removes an evas object from the list of member-objects of the widget. See etk_widget_member_object_add() for
 * more info about member-objects.
 * @param widget a widget
 * @param object the evas object to remove
 * @note The object will still be displayed on the Evas after this call
 * @widget_implementation
 * @see etk_widget_member_object_add()
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
 * @brief Stacks @a object above all the other member-objects of the widget
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
 * @brief Stacks @a object below all the other member-objects of the widget
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
      if (widget->content_object)
         evas_object_lower(object);
      else if (widget->theme_object)
         evas_object_stack_above(object, widget->theme_object);
      else if (widget->event_object)
         evas_object_stack_above(object, widget->event_object);
      else
         evas_object_lower(object);
   }
}

/**
 * @brief Stacks the member-object @a object above the member-object @a above
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
 * @brief Stacks the member-object @a object below the member-object @a below
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
 * @brief Sets the clip-object of the widget. The theme-object and the member-objects of the widget will be
 * automatically clipped against @a clip
 * @param widget a widget
 * @param clip the clip-object to set
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
 * @brief Unsets the clip-object of the widget. The theme-object and the member-objects won't be clipped anymore
 * @param widget a widget
 * @return Returns the clip-object of the widget
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
 * @brief Gets the clip-object of the widget
 * @param widget a widget
 * @return Returns the clip-object of the widget
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
   etk_widget_theme_signal_emit(widget, "drag_drop", ETK_FALSE);   
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

   widget->parent = NULL;
   widget->toplevel_parent = NULL;
   widget->child_properties = NULL;
   widget->children = NULL;
   widget->focus_order = NULL;

   widget->theme_object = NULL;
   widget->theme_min_size.w = 0;
   widget->theme_min_size.h = 0;
   widget->theme_file = NULL;
   widget->theme_group = NULL;
   widget->theme_group_full = NULL;
   widget->theme_parent = NULL;
   widget->theme_children = NULL;

   widget->smart_object = NULL;
   widget->event_object = NULL;
   widget->content_object = NULL;
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

   widget->inset.left = 0;
   widget->inset.right = 0;
   widget->inset.top = 0;
   widget->inset.bottom = 0;

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
   widget->last_calced_size.w = 0;
   widget->last_calced_size.h = 0;

   widget->scroll_size_get = NULL;
   widget->scroll_margins_get = NULL;
   widget->scroll = NULL;

   widget->realized = ETK_FALSE;
   widget->swallowed = ETK_FALSE;
   widget->visible = ETK_FALSE;
   widget->internal = ETK_FALSE;
   widget->focusable = ETK_FALSE;
   widget->focus_on_click = ETK_FALSE;
   widget->use_focus_order = ETK_FALSE;
   widget->has_event_object = ETK_FALSE;
   widget->repeat_mouse_events = ETK_FALSE;
   widget->pass_mouse_events = ETK_FALSE;
   widget->need_size_recalc = ETK_FALSE;
   widget->need_redraw = ETK_FALSE;
   widget->need_theme_size_recalc = ETK_FALSE;
   widget->swallowed = ETK_FALSE;
   widget->accepts_dnd = ETK_FALSE;
   widget->dnd_internal = ETK_FALSE;
   widget->dnd_source = ETK_FALSE;
   widget->dnd_dest = ETK_FALSE;
   widget->dnd_types = NULL;
   widget->dnd_types_num = 0;
   
   etk_signal_connect("destroyed", ETK_OBJECT(widget), ETK_CALLBACK(_etk_widget_destroyed_cb), NULL);
   etk_signal_connect_swapped("mouse_in", ETK_OBJECT(widget), etk_widget_enter, widget);
   etk_signal_connect_swapped("mouse_out", ETK_OBJECT(widget), etk_widget_leave, widget);
   etk_signal_connect("mouse_down", ETK_OBJECT(widget), ETK_CALLBACK(_etk_widget_signal_mouse_down_cb), NULL);
   etk_signal_connect("key_down", ETK_OBJECT(widget), ETK_CALLBACK(_etk_widget_signal_key_down_cb), NULL);
   
   if (ETK_IS_TOPLEVEL(widget))
      etk_object_notification_callback_add(ETK_OBJECT(widget), "evas", _etk_widget_toplevel_evas_changed_cb, NULL);
}

/* Destroys the widget */
static void _etk_widget_destructor(Etk_Widget *widget)
{
   if (!widget)
      return;

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
   free(widget->theme_group_full);
}

/* Called when etk_object_destroy() is called on the widget.
 * We use this function, and not the destructor, to destroy the children of the widget
 * because destroying a widget requires the widget's parent to be still usable */
void _etk_widget_destroyed_cb(Etk_Object *object, void *data)
{
   Etk_Widget *widget;
   Etk_Widget *child;
   
   if (!(widget = ETK_WIDGET(object)))
      return;
   
   _etk_widget_unrealize(widget);
   
   /* Remove the children */
   while (widget->children)
   {
      child = ETK_WIDGET(widget->children->data);
      
      etk_widget_parent_set(child, NULL);
      etk_object_destroy(ETK_OBJECT(child));
   }
   widget->focus_order = evas_list_free(widget->focus_order);
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
      case ETK_WIDGET_WIDTH_REQUEST_PROPERTY:
         etk_widget_size_request_set(widget, etk_property_value_int_get(value), widget->requested_size.h);
         break;
      case ETK_WIDGET_HEIGHT_REQUEST_PROPERTY:
         etk_widget_size_request_set(widget, widget->requested_size.w, etk_property_value_int_get(value));
         break;
      case ETK_WIDGET_VISIBLE_PROPERTY:
         if (etk_property_value_bool_get(value))
            etk_widget_show(widget);
         else
            etk_widget_hide(widget);
         break;
      case ETK_WIDGET_INTERNAL_PROPERTY:
         etk_widget_internal_set(widget, etk_property_value_bool_get(value));
         break;
      case ETK_WIDGET_REPEAT_MOUSE_EVENTS_PROPERTY:
         etk_widget_repeat_mouse_events_set(widget, etk_property_value_bool_get(value));
         break;
      case ETK_WIDGET_PASS_MOUSE_EVENTS_PROPERTY:
         etk_widget_pass_mouse_events_set(widget, etk_property_value_bool_get(value));
         break;
      case ETK_WIDGET_HAS_EVENT_OBJECT_PROPERTY:
         etk_widget_has_event_object_set(widget, etk_property_value_bool_get(value));
         break;
      case ETK_WIDGET_FOCUSABLE_PROPERTY:
         etk_widget_focusable_set(widget, etk_property_value_bool_get(value));
         break;
      case ETK_WIDGET_FOCUS_ON_CLICK_PROPERTY:
         widget->focus_on_click = etk_property_value_bool_get(value);
         etk_object_notify(object, "focus_on_click");
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
      case ETK_WIDGET_INTERNAL_PROPERTY:
         etk_property_value_bool_set(value, widget->internal);
         break;
      case ETK_WIDGET_REPEAT_MOUSE_EVENTS_PROPERTY:
         etk_property_value_bool_set(value, widget->repeat_mouse_events);
         break;
      case ETK_WIDGET_PASS_MOUSE_EVENTS_PROPERTY:
         etk_property_value_bool_set(value, widget->pass_mouse_events);
         break;
      case ETK_WIDGET_HAS_EVENT_OBJECT_PROPERTY:
         etk_property_value_bool_set(value, widget->has_event_object);
         break;
      case ETK_WIDGET_FOCUSABLE_PROPERTY:
         etk_property_value_bool_set(value, widget->focusable);
         break;
      case ETK_WIDGET_FOCUS_ON_CLICK_PROPERTY:
         etk_property_value_bool_set(value, widget->focus_on_click);
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
   etk_widget_theme_signal_emit(widget, "etk,state,enter", ETK_FALSE);
}

/* Default handler for the "leave" signal */
static void _etk_widget_leave_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_signal_emit(widget, "etk,state,leave", ETK_FALSE);
}

/* Default handler for the "focus" signal */
static void _etk_widget_focus_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_signal_emit(widget, "etk,state,focused", ETK_FALSE);
}

/* Default handler for the "unfocus" signal */
static void _etk_widget_unfocus_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_signal_emit(widget, "etk,state,unfocused", ETK_FALSE);
}

/* Sets the widget as visible and queues a visibility update */
static void _etk_widget_show_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_signal_emit(widget, "etk,state,shown", ETK_FALSE);
}

/* Default handler for the "drag_drop" signal */
static void _etk_widget_drag_drop_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_signal_emit(widget, "drag_drop", ETK_FALSE);
}

/* Default handler for the "drag_motion" signal */
static void _etk_widget_drag_motion_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_signal_emit(widget, "drag_motion", ETK_FALSE);
}

/* Default handler for the "drag_enter" signal */
static void _etk_widget_drag_enter_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_signal_emit(widget, "drag_enter", ETK_FALSE);
}

/* Default handler for the "drag_leave" signal */
static void _etk_widget_drag_leave_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_signal_emit(widget, "drag_leave", ETK_FALSE);
}

/* Default handler for the "drag_begin" signal */
static void _etk_widget_drag_begin_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_signal_emit(widget, "drag_begin", ETK_FALSE);
}

/* Default handler for the "drag_end" signal */
static void _etk_widget_drag_end_handler(Etk_Widget *widget)
{
   if (!widget)
      return;
   etk_widget_theme_signal_emit(widget, "drag_end", ETK_FALSE);
}

/* Evas Callback: Called when the mouse pointer enters the widget */
static void _etk_widget_mouse_in_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Etk_Event_Mouse_In event;

   if (!(widget = ETK_WIDGET(data)))
      return;
   
   if (!widget->pass_mouse_events)
   {
      etk_event_mouse_in_wrap(widget, event_info, &event);
      etk_signal_emit(_etk_widget_signals[ETK_WIDGET_MOUSE_IN_SIGNAL], ETK_OBJECT(widget), NULL, &event);
   }
}

/* Evas Callback: Called when the mouse pointer leaves the widget */
static void _etk_widget_mouse_out_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Etk_Event_Mouse_Out event;

   if (!(widget = ETK_WIDGET(data)))
      return;
   
   if (!widget->pass_mouse_events)
   {
      etk_event_mouse_out_wrap(widget, event_info, &event);
      etk_signal_emit(_etk_widget_signals[ETK_WIDGET_MOUSE_OUT_SIGNAL], ETK_OBJECT(widget), NULL, &event);
   }
}

/* Evas Callback: Called when the mouse pointer moves over the widget */
static void _etk_widget_mouse_move_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Etk_Event_Mouse_Move event;

   if (!(widget = ETK_WIDGET(data)))
      return;
   
   if (!widget->pass_mouse_events)
   {
      etk_event_mouse_move_wrap(widget, event_info, &event);
      etk_signal_emit(_etk_widget_signals[ETK_WIDGET_MOUSE_MOVE_SIGNAL], ETK_OBJECT(widget), NULL, &event);
   }
   
   if (widget->parent)
      _etk_widget_mouse_move_cb(widget->parent, evas, NULL, event_info);
}

/* Evas Callback: Called when the widget is pressed by the mouse */
static void _etk_widget_mouse_down_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Etk_Event_Mouse_Down event;

   if (!(widget = ETK_WIDGET(data)))
      return;
   
   if (!widget->pass_mouse_events)
   {
      etk_event_mouse_down_wrap(widget, event_info, &event);
      etk_signal_emit(_etk_widget_signals[ETK_WIDGET_MOUSE_DOWN_SIGNAL], ETK_OBJECT(widget), NULL, &event);
   }
   
   if ((widget->pass_mouse_events || widget->repeat_mouse_events) && widget->parent)
      _etk_widget_mouse_down_cb(widget->parent, evas, NULL, event_info);
}

/* Evas Callback: Called when the widget is released by the mouse */
static void _etk_widget_mouse_up_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Etk_Event_Mouse_Up event;

   if (!(widget = ETK_WIDGET(data)))
      return;
   
   if (!widget->pass_mouse_events)
   {
      etk_event_mouse_up_wrap(widget, event_info, &event);
      etk_signal_emit(_etk_widget_signals[ETK_WIDGET_MOUSE_UP_SIGNAL], ETK_OBJECT(widget), NULL, &event);
      
      if (ETK_INSIDE(event.canvas.x, event.canvas.y,
         widget->geometry.x, widget->geometry.y, widget->geometry.w, widget->geometry.h))
      {
         etk_signal_emit(_etk_widget_signals[ETK_WIDGET_MOUSE_CLICK_SIGNAL], ETK_OBJECT(widget), NULL, &event);
      }
   }
   
   if ((widget->pass_mouse_events || widget->repeat_mouse_events) && widget->parent)
      _etk_widget_mouse_up_cb(widget->parent, evas, NULL, event_info);
}

/* Evas Callback: Called when the mouse wheel is used over the widget */
static void _etk_widget_mouse_wheel_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Etk_Event_Mouse_Wheel event;
   Etk_Bool propagate;

   if (!(widget = ETK_WIDGET(data)))
      return;
   
   etk_event_mouse_wheel_wrap(widget, event_info, &event);
   propagate = etk_signal_emit(_etk_widget_signals[ETK_WIDGET_MOUSE_WHEEL_SIGNAL], ETK_OBJECT(widget), NULL, &event);
   
   if (propagate && widget->parent)
      _etk_widget_mouse_wheel_cb(widget->parent, evas, NULL, event_info);
}

/* Signal Callback: Called when the user presses a key, if the widget is focused */
static void _etk_widget_signal_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Etk_Widget *widget;
   Etk_Toplevel *toplevel;

   if (!(widget = ETK_WIDGET(object)) || !event || !(toplevel = (widget->toplevel_parent)) || !event->keyname)
      return;

   if (strcmp(event->keyname, "Tab") == 0)
   {
      Etk_Widget *to_focus;
      if (event->modifiers & ETK_MODIFIER_SHIFT)
         to_focus = etk_toplevel_focused_widget_prev_get(toplevel);
      else
         to_focus = etk_toplevel_focused_widget_next_get(toplevel);
      
      if (to_focus)
         etk_widget_focus(to_focus);
      else
         etk_widget_unfocus(widget);
      
      etk_signal_stop();
   }
}

/* Signal Callback: Called when the widget is pressed by the mouse */
static void _etk_widget_signal_mouse_down_cb(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data)
{
   Etk_Widget *widget;
 
   if (!(widget = ETK_WIDGET(object)))
      return;
   if (widget->focus_on_click)
      etk_widget_focus(widget);
}

/* Called when the evas of a toplevel widget is changed */
static void _etk_widget_toplevel_evas_changed_cb(Etk_Object *object, const char *property_name, void *data)
{
   Etk_Widget *widget;
   Evas *evas;
   
   if (!(widget = ETK_WIDGET(object)))
      return;
   
   evas = etk_toplevel_evas_get(ETK_TOPLEVEL(widget));
   if (evas && (!widget->smart_object || evas_object_evas_get(widget->smart_object) != evas))
      _etk_widget_realize_children(widget, ETK_TRUE, ETK_TRUE);
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
   
   if (widget->parent && widget->parent->smart_object)
      _etk_widget_object_add_to_smart(widget->parent, widget->smart_object, (widget->clip == NULL));
   for (l = widget->children; l; l = l->next)
   {
      child = ETK_WIDGET(l->data);
      if (child->smart_object)
         _etk_widget_object_add_to_smart(widget, child->smart_object, (child->clip == NULL));
   }
   
   if (widget->visible
      && (ETK_IS_TOPLEVEL(widget)
         || (widget->parent && widget->parent->smart_object && evas_object_visible_get(widget->parent->smart_object))))
   {
      evas_object_show(widget->smart_object);
   }
   else
      evas_object_hide(widget->smart_object);

   evas_object_propagate_events_set(widget->smart_object, 0);
   evas_object_pass_events_set(widget->smart_object, widget->pass_mouse_events);
   
   /* Then, we create the theme-object */
   widget->theme_object = edje_object_add(evas);
   if (etk_theme_edje_object_set(widget->theme_object, etk_widget_theme_file_get(widget),
      widget->theme_group, etk_widget_theme_group_get(widget->theme_parent)))
   {
      if (etk_widget_theme_data_get(widget, "inset", "%d %d %d %d",
         &widget->inset.left, &widget->inset.right, &widget->inset.top, &widget->inset.bottom) != 4)
      {
         widget->inset.left = 0;
         widget->inset.right = 0;
         widget->inset.top = 0;
         widget->inset.bottom = 0;
      }
      evas_object_show(widget->theme_object);
      _etk_widget_object_add_to_smart(widget, widget->theme_object, ETK_TRUE);
      evas_object_lower(widget->theme_object);
      
      /* We create the content-object if the part ETK_WIDGET_CONTENT_PART exists:
       * the content-object is a smart object that will be swallowed in this part, and
       * whose member-objects are the member-objects and the children's smart-objects of the widget.
       * This way, we have a powerful and generic way to swallow children in any types of widgets */
      if (edje_object_part_exists(widget->theme_object, ETK_WIDGET_CONTENT_PART))
      {
         widget->content_object = _etk_widget_content_object_add(evas, widget);
         evas_object_color_set(widget->content_object, 0, 0, 0, 0);
         evas_object_show(widget->content_object);
         edje_object_part_swallow(widget->theme_object, ETK_WIDGET_CONTENT_PART, widget->content_object);
      }
   }
   else
   {
      evas_object_del(widget->theme_object);
      widget->theme_object = NULL;
      
      /* And we create the event-object if necessary */
      if (widget->has_event_object)
      {
         widget->event_object = evas_object_rectangle_add(evas);
         evas_object_color_set(widget->event_object, 0, 0, 0, 0);
         evas_object_show(widget->event_object);
         _etk_widget_object_add_to_smart(widget, widget->event_object, ETK_TRUE);
         evas_object_lower(widget->event_object);
      }
   }

   widget->need_theme_size_recalc = ETK_TRUE;
   widget->realized = ETK_TRUE;

   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_REALIZE_SIGNAL], ETK_OBJECT(widget), NULL);
   
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

   etk_signal_emit(_etk_widget_signals[ETK_WIDGET_UNREALIZE_SIGNAL], ETK_OBJECT(widget), NULL);
   
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
   if (widget->content_object)
   {
      evas_object_del(widget->content_object);
      widget->content_object = NULL;
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
   
   widget->inset.left = 0;
   widget->inset.right = 0;
   widget->inset.top = 0;
   widget->inset.bottom = 0;
   
   widget->realized = ETK_FALSE;
}

/* Updates the full theme-group string of the widget and of its theme-children */
static void _etk_widget_theme_group_full_update(Etk_Widget *widget)
{
   Evas_List *l;
   
   if (!widget)
      return;
   
   free(widget->theme_group_full);
   if (widget->theme_group && *widget->theme_group != '\0')
   {
      if (!widget->theme_parent)
         widget->theme_group_full = strdup(widget->theme_group);
      else
      {
         Evas_List *theme_parents = NULL, *l;
         Etk_Widget *theme_parent;
         int length;
         
         length = strlen(widget->theme_group);
         for (theme_parent = widget->theme_parent; theme_parent; theme_parent = theme_parent->theme_parent)
         {
            if (theme_parent->theme_group && *theme_parent->theme_group)
            {
               length += strlen(theme_parent->theme_group) + 1;
               theme_parents = evas_list_prepend(theme_parents, theme_parent);
            }
         }
            
         widget->theme_group_full = malloc(length + 1);
         widget->theme_group_full[0] = '\0';
         for (l = theme_parents; l; l = l->next)
         {
            theme_parent = ETK_WIDGET(l->data);
            strcat(widget->theme_group_full, theme_parent->theme_group);
            strcat(widget->theme_group_full, "/");
         }
         strcat(widget->theme_group_full, widget->theme_group);
         
         evas_list_free(theme_parents);
      }
   }
   else
      widget->theme_group_full = NULL;
   
   /* Updates the full theme-group of the theme-children */
   for (l = widget->theme_children; l; l = l->next)
      _etk_widget_theme_group_full_update(ETK_WIDGET(l->data));
}

/* Sets recursively the toplevel parent of the widget and of its children. Used by etk_widget_parent_set() */
static void _etk_widget_toplevel_parent_set(Etk_Widget *widget, Etk_Toplevel *toplevel_parent)
{
   Evas_List *l;
   
   if (!widget)
      return;

   if (etk_widget_is_focused(widget))
      etk_widget_unfocus(widget);
   widget->toplevel_parent = toplevel_parent;
   
   for (l = widget->children; l; l = l->next)
      _etk_widget_toplevel_parent_set(ETK_WIDGET(l->data), toplevel_parent);
}

/* Realizes the widget and all its children */
static void _etk_widget_realize_children(Etk_Widget *widget, Etk_Bool realize, Etk_Bool evas_changed)
{
   Evas_List *l;
   Etk_Widget *child;
   
   if (!widget || !etk_widget_toplevel_evas_get(widget))
      return;

   if (realize)
      _etk_widget_realize(widget);
   for (l = widget->children; l; l = l->next)
   {
      child = ETK_WIDGET(l->data);
      if (evas_changed || (!child->theme_parent && !child->theme_file))
         _etk_widget_realize_children(child, ETK_TRUE, evas_changed);
   }
}

/* Realizes the widget and all its theme-children */
static void _etk_widget_realize_theme_children(Etk_Widget *widget, Etk_Bool realize, Etk_Bool theme_group_changed)
{
   Evas_List *l;
   Etk_Widget *child;
   
   if (!widget || !etk_widget_toplevel_evas_get(widget))
      return;

   if (realize)
      _etk_widget_realize(widget);
   for (l = widget->theme_children; l; l = l->next)
   {
      child = ETK_WIDGET(l->data);
      if (theme_group_changed || (!child->theme_file))
         _etk_widget_realize_theme_children(child, ETK_TRUE, theme_group_changed);
   }
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

/* Calculates the minimum size of the theme-object of "widget" */
static void _etk_widget_theme_min_size_calc(Etk_Widget *widget, int *w, int *h, Etk_Bool hidden_has_no_size)
{
   if (!widget)
      return;

   if (!widget->visible && hidden_has_no_size)
   {
      if (w)   *w = 0;
      if (h)   *h = 0;
      return;
   }
   
   if (widget->need_theme_size_recalc)
   {
      if (widget->theme_object)
      {
         int min_calc_width, min_calc_height;
         int min_get_width, min_get_height;
         Evas_List *l;
         Etk_Widget_Swallowed_Object *swallowed_object;

         /* Calculate and set the min size of the swallowed objects */
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
         /* Calculate and set the min size of the content-object */
         if (widget->content_object)
         {
            Etk_Size size_request;
            
            size_request = widget->requested_size;
            if ((size_request.w < 0 || size_request.h < 0) && widget->size_request)
            {
               Etk_Size calced_size;
               
               widget->size_request(widget, &calced_size);
               if (size_request.w < 0)
                  size_request.w = calced_size.w;
               if (size_request.h < 0)
                  size_request.h = calced_size.h;
            }
            size_request.w = ETK_MAX(size_request.w, 0);
            size_request.w = ETK_MAX(size_request.w, 0);
            edje_extern_object_min_size_set(widget->content_object, size_request.w, size_request.h);
            edje_object_part_swallow(widget->theme_object, ETK_WIDGET_CONTENT_PART, widget->content_object);
         }
         
         /* Calculate the min size of the theme-object */
         edje_object_message_signal_process(widget->theme_object);
         edje_object_size_min_calc(widget->theme_object, &min_calc_width, &min_calc_height);
         edje_object_size_min_get(widget->theme_object, &min_get_width, &min_get_height);
         widget->theme_min_size.w = ETK_MAX(min_calc_width, min_get_width);
         widget->theme_min_size.h = ETK_MAX(min_calc_height, min_get_height);

         /* And finally, reset the min size of the swallowed objects and of the content-object to 0x0 */
         for (l = widget->swallowed_objects; l; l = l->next)
         {
            swallowed_object = l->data;
            if (swallowed_object->widget)
            {
               edje_extern_object_min_size_set(swallowed_object->object, 0, 0);
               edje_object_part_swallow(widget->theme_object, swallowed_object->part, swallowed_object->object);
            }
         }
         if (widget->content_object)
         {
            edje_extern_object_min_size_set(widget->content_object, 0, 0);
            edje_object_part_swallow(widget->theme_object, ETK_WIDGET_CONTENT_PART, widget->content_object);
         }
      }
      else
      {
         widget->theme_min_size.w = 0;
         widget->theme_min_size.h = 0;
      }
      
      widget->theme_min_size.w = ETK_MAX(widget->theme_min_size.w, widget->inset.left + widget->inset.right);
      widget->theme_min_size.h = ETK_MAX(widget->theme_min_size.h, widget->inset.top + widget->inset.bottom);
      widget->need_theme_size_recalc = ETK_FALSE;
   }

   if (w)   *w = widget->theme_min_size.w;
   if (h)   *h = widget->theme_min_size.h;
}

/* Marks recursively all the children of "widget" as needing a redraw. Used by etk_widget_redraw_queue() */
static void _etk_widget_redraw_queue_recursive(Etk_Widget *widget)
{
   Evas_List *l;
   
   if (!widget)
      return;

   widget->need_redraw = ETK_TRUE;
   for (l = widget->children; l; l = l->next)
      _etk_widget_redraw_queue_recursive(ETK_WIDGET(l->data));
}

/* Makes the theme-object of the widget swallow a widget or an object into one of its part */
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

/* Makes the theme-object of the widget unswallow an object */
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
   }
   
   free(swo->part);
   free(swo);
   swallower->swallowed_objects = evas_list_remove_list(swallower->swallowed_objects, swo_node);
   
   etk_widget_size_recalc_queue(swallower);
}

/* Removes a child from the widget */
static void _etk_widget_child_remove(Etk_Widget *parent, Etk_Widget *child)
{
   Evas_List *l;
   
   if (!parent || !child)
      return;

   if ((l = evas_list_find_list(parent->children, child)))
   {
      if (child->swallowed)
         etk_widget_unswallow_widget(parent, child);
      if (child->smart_object)
         evas_object_smart_member_del(child->smart_object);
      if (child->clip && parent->clip == child->clip)
         etk_widget_clip_unset(child);
      
      parent->children = evas_list_remove_list(parent->children, l);
      etk_widget_size_recalc_queue(parent);
   }
}

/* Adds an object to the widget smart object */
static void _etk_widget_object_add_to_smart(Etk_Widget *widget, Evas_Object *object, Etk_Bool clip)
{
   Evas_Object *parent;
   Evas_Object *clip_obj;
   
   if (!widget || !object)
      return;
   if (!(parent = widget->content_object ? widget->content_object : widget->smart_object))
      return;
   
   if (!evas_object_visible_get(parent))
      evas_object_hide(object);
   if (clip && (clip_obj = evas_object_clip_get(parent)))
      evas_object_clip_set(object, clip_obj);
   evas_object_smart_member_add(object, parent);
   
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

/* Finds if an evas object is a member-object of the widget */
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

/* Called when a member-object requests to be shown. Shows it only if all the parents are visible */
static void _etk_widget_member_object_intercept_show_cb(void *data, Evas_Object *obj)
{
   Etk_Widget *widget;
   Evas_List *l;
   Etk_Widget_Member_Object *member_object;
   Evas_Object *parent;
   
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
   
   parent = widget->content_object ? widget->content_object : widget->smart_object;
   if (parent && evas_object_visible_get(parent))
      evas_object_show(obj);
}


/* Called when a member-object of the widget requests to be hidden */
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

/* Called when a member-object of the widget is deleted */
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
         {
            swo->object = NULL;
            _etk_widget_unswallow_full(widget, l);
         }
         
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
   
   etk_signal_disconnect("realize", ETK_OBJECT(swallowed), ETK_CALLBACK(_etk_widget_swallowed_widget_realize_cb));
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

/**************************
 *
 * Etk_Widget's smart-object
 *
 **************************/

/* Creates a new smart object for the widget */
static Evas_Object *_etk_widget_smart_object_add(Evas *evas, Etk_Widget *widget)
{
   Evas_Object *new_object;
   
   if (!evas || !widget)
      return NULL;

   if (!_etk_widget_smart_object_smart)
   {
      _etk_widget_smart_object_smart = evas_smart_new("Widget_Object",
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

   if (x != widget->geometry.x || y != widget->geometry.y)
   {
      Evas_List *l;
      Etk_Widget_Member_Object *m;
      Evas_Coord child_x, child_y;
      int x_offset, y_offset;

      x_offset = x - widget->geometry.x;
      y_offset = y - widget->geometry.y;
      widget->geometry.x = x;
      widget->geometry.y = y;
      if (!widget->content_object)
      {
         widget->inner_geometry.x = widget->geometry.x + widget->inset.left;
         widget->inner_geometry.y = widget->geometry.y + widget->inset.top;
      }

      if (widget->theme_object)
         evas_object_move(widget->theme_object, widget->geometry.x, widget->geometry.y);
      if (widget->event_object)
         evas_object_move(widget->event_object, widget->geometry.x, widget->geometry.y);
      /* Move the member-objects and the children to the right place */
      if (!widget->content_object)
      {
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
      if (!widget->content_object)
      {
         widget->inner_geometry.w = widget->geometry.w - widget->inset.left - widget->inset.right;
         widget->inner_geometry.h = widget->geometry.h - widget->inset.top - widget->inset.bottom;
      }

      if (widget->theme_object)
      {
         evas_object_move(widget->theme_object, widget->geometry.x, widget->geometry.y);
         evas_object_resize(widget->theme_object, widget->geometry.w, widget->geometry.h);
      }
      if (widget->event_object)
      {
         evas_object_move(widget->event_object, widget->geometry.x, widget->geometry.y);
         evas_object_resize(widget->event_object, widget->geometry.w, widget->geometry.h);
      }
      if ((!widget->content_object || widget->need_redraw) && widget->size_allocate)
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
      if (!child->swallowed)
         evas_object_hide(child->smart_object);
   }
}

/* Called when a clip is set to the smart object */
static void _etk_widget_smart_object_clip_set_cb(Evas_Object *object, Evas_Object *clip)
{
   Etk_Widget *widget;
   Etk_Widget *child;
   Etk_Widget_Member_Object *member_object;
   Evas_List *l;

   if (!object || !clip || !(widget = ETK_WIDGET(evas_object_smart_data_get(object))))
      return;

   if (widget->clip)
      etk_widget_clip_unset(widget);

   if (widget->theme_object)
      evas_object_clip_set(widget->theme_object, clip);
   if (widget->event_object)
      evas_object_clip_set(widget->event_object, clip);
   
   /* Clip the member-objects and the children */
   if (!widget->content_object)
   {
      for (l = widget->member_objects; l; l = l->next)
      {
         member_object = l->data;
         if (!evas_object_clip_get(member_object->object))
            evas_object_clip_set(member_object->object, clip);
      }
      for (l = widget->children; l; l = l->next)
      {
         child = ETK_WIDGET(l->data);
         if (!child->swallowed && !etk_widget_clip_get(child))
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
   Etk_Widget *child;
   Etk_Widget_Member_Object *member_object;
   Evas_List *l;

   if (!object || !(widget = ETK_WIDGET(evas_object_smart_data_get(object))) || !widget->clip)
      return;

   if (widget->theme_object)
      evas_object_clip_unset(widget->theme_object);
   if (widget->event_object)
      evas_object_clip_unset(widget->event_object);
   
   /* Unclip the member-objects and the children */
   if (!widget->content_object)
   {
      for (l = widget->member_objects; l; l = l->next)
      {
         member_object = l->data;
         if (evas_object_clip_get(member_object->object) == widget->clip)
            evas_object_clip_unset(member_object->object);
      }
      for (l = widget->children; l; l = l->next)
      {
         child = ETK_WIDGET(l->data);
         if (!child->swallowed && etk_widget_clip_get(child) == widget->clip)
            etk_widget_clip_unset(child);
      }
   }
   
   _etk_widget_remove_from_clip(widget, widget->clip);
   widget->clip = NULL;
}

/**************************
 *
 * Etk_Widget's content-object
 *
 **************************/

/* Creates a new content-object for the widget */
static Evas_Object *_etk_widget_content_object_add(Evas *evas, Etk_Widget *widget)
{
   Evas_Object *new_object;
   
   if (!evas || !widget)
      return NULL;

   if (!_etk_widget_content_object_smart)
   {
      _etk_widget_content_object_smart = evas_smart_new("Content_Object",
         NULL, /* add */
         NULL, /* del */
         NULL, /* layer_set */
         NULL, /* raise */
         NULL, /* lower */
         NULL, /* stack_above */
         NULL, /* stack_below */
         _etk_widget_content_object_move_cb, /* move */
         _etk_widget_content_object_resize_cb, /* resize */
         NULL, /* show */
         NULL, /* hide */
         NULL, /* color_set */
         _etk_widget_content_object_clip_set_cb, /* clip_set */
         _etk_widget_content_object_clip_unset_cb, /* clip_unset */
         NULL); /* data*/
   }

   new_object = evas_object_smart_add(evas, _etk_widget_content_object_smart);
   evas_object_smart_data_set(new_object, widget);
   
   return new_object;
}

/* Called when the content-object of the widget is moved */
static void _etk_widget_content_object_move_cb(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   Etk_Widget *widget;
   Evas_Coord prev_x, prev_y;
   
   if (!obj || !(widget = ETK_WIDGET(evas_object_smart_data_get(obj))))
      return;
   
   widget->inner_geometry.x = x;
   widget->inner_geometry.y = y;
   
   evas_object_geometry_get(obj, &prev_x, &prev_y, NULL, NULL);
   if (prev_x != x || prev_y != y)
   {
      Etk_Widget *child;
      Etk_Widget_Member_Object *m;
      Evas_List *l;
      Evas_Coord child_x, child_y;
      int offset_x, offset_y;
      
      offset_x = x - prev_x;
      offset_y = y - prev_y;
      
      /* Move the member-objects and the children to the right place */
      for (l = widget->member_objects; l; l = l->next)
      {
         m = l->data;
         evas_object_geometry_get(m->object, &child_x, &child_y, NULL, NULL);
         evas_object_move(m->object, child_x + offset_x, child_y + offset_y);
      }
      for (l = widget->children; l; l = l->next)
      {
         child = l->data;
         if (!child->swallowed)
         {
            evas_object_geometry_get(child->smart_object, &child_x, &child_y, NULL, NULL);
            evas_object_move(child->smart_object, child_x + offset_x, child_y + offset_y);
         }
      }
   }
}

/* Called when the content-object of the widget is resized */
static void _etk_widget_content_object_resize_cb(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   Etk_Widget *widget;
   Evas_Coord prev_w, prev_h;
   
   if (!obj || !(widget = ETK_WIDGET(evas_object_smart_data_get(obj))))
      return;
   
   widget->inner_geometry.w = w;
   widget->inner_geometry.h = h;
   
   evas_object_geometry_get(obj, &prev_w, &prev_h, NULL, NULL);
   if ((prev_w != w || prev_h != h || widget->need_redraw) && widget->size_allocate)
   {
      widget->size_allocate(widget, widget->inner_geometry);
      widget->need_redraw = ETK_FALSE;
   }
}

/* Called when the content-object of the widget is clipped */
static void _etk_widget_content_object_clip_set_cb(Evas_Object *obj, Evas_Object *clip)
{
   Etk_Widget *widget;
   Etk_Widget *child;
   Etk_Widget_Member_Object *member_object;
   Evas_List *l;

   if (!obj || !clip || !(widget = ETK_WIDGET(evas_object_smart_data_get(obj))))
      return;
   
   /* Clip the member-objects and the children */
   for (l = widget->member_objects; l; l = l->next)
   {
      member_object = l->data;
      if (!evas_object_clip_get(member_object->object))
         evas_object_clip_set(member_object->object, clip);
   }
   for (l = widget->children; l; l = l->next)
   {
      child = ETK_WIDGET(l->data);
      if (!child->swallowed && !etk_widget_clip_get(child))
         etk_widget_clip_set(child, clip);
   }
}

/* Called when the content-object of the widget is unclipped */
static void _etk_widget_content_object_clip_unset_cb(Evas_Object *obj)
{
   Etk_Widget *widget;
   Etk_Widget *child;
   Etk_Widget_Member_Object *member_object;
   Evas_Object *prev_clip;
   Evas_List *l;

   if (!obj || !(widget = ETK_WIDGET(evas_object_smart_data_get(obj))))
      return;
   if (!(prev_clip = evas_object_clip_get(obj)))
      return;
   
   /* Unclip the member-objects and the children */
   for (l = widget->member_objects; l; l = l->next)
   {
      member_object = l->data;
      if (evas_object_clip_get(member_object->object) == prev_clip)
         evas_object_clip_unset(member_object->object);
   }
   for (l = widget->children; l; l = l->next)
   {
      child = ETK_WIDGET(l->data);
      if (!child->swallowed && etk_widget_clip_get(child) == prev_clip)
         etk_widget_clip_unset(child);
   }
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Widget
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *
 * \par Signals:
 * @signal_name "show": Emitted when the widget is shown
 * @signal_cb void callback(Etk_Widget *widget, void *data)
 * @signal_arg widget: the widget that has been shown
 * @signal_data
 * \par
 * @signal_name "hide": Emitted when the widget is hidden
 * @signal_cb void callback(Etk_Widget *widget, void *data)
 * @signal_arg widget: the widget that has been hidden
 * @signal_data
 * \par
 * @signal_name "realize": Emitted when the widget is realized (i.e. when the graphical resources of
 * the widget are allocated - when the widget is attached to an Evas, or when its theme is changed)
 * @signal_cb void callback(Etk_Widget *widget, void *data)
 * @signal_arg widget: the widget that has been realized
 * @signal_data
 * \par
 * @signal_name "unrealize": Emitted when the widget is unrealized (i.e. when its graphical resources are deallocated)
 * @signal_cb void callback(Etk_Widget *widget, void *data)
 * @signal_arg widget: the widget that has been unrealized
 * @signal_data
 * \par
 * @signal_name "size_request": Emitted each time the function etk_widget_size_request() is called on a widget
 * @signal_cb void callback(Etk_Widget *widget, Etk_Size *size, void *data)
 * @signal_arg widget: the widget whose size has been requested
 * @signal_arg size: The result of the call to etk_widget_size_request()
 * @signal_data
 * \par
 * @signal_name "mouse_in": Emitted when the mouse enters the widget
 * @signal_cb void callback(Etk_Widget *widget, Etk_Event_Mouse_In *event, void *data)
 * @signal_arg widget: the widget that the mouse has entered
 * @signal_arg event: The event data
 * @signal_data
 * \par
 * @signal_name "mouse_out": Emitted when the mouse leaves the widget
 * @signal_cb void callback(Etk_Widget *widget, Etk_Event_Mouse_Out *event, void *data)
 * @signal_arg widget: the widget that the mouse has left
 * @signal_arg event: The event data
 * @signal_data
 * \par
 * @signal_name "mouse_move": Emitted when the mouse moves over the widget
 * @signal_cb void callback(Etk_Widget *widget, Etk_Event_Mouse_Move *event, void *data)
 * @signal_arg widget: the widget above which the mouse is moving
 * @signal_arg event: The event data
 * @signal_data
 * \par
 * @signal_name "mouse_down": Emitted when the user presses the widget with the mouse
 * @signal_cb void callback(Etk_Widget *widget, Etk_Event_Mouse_Down *event, void *data)
 * @signal_arg widget: the widget that has been pressed
 * @signal_arg event: The event data
 * @signal_data
 * \par
 * @signal_name "mouse_up": Emitted when the user releases the widget with the mouse
 * @signal_cb void callback(Etk_Widget *widget, Etk_Event_Mouse_Up *event, void *data)
 * @signal_arg widget: the widget that has been released
 * @signal_arg event: The event data
 * @signal_data
 * \par
 * @signal_name "mouse_click": Emitted when the user has clicked on the widget (i.e. the mouse button has been released
 * and the mouse is still above the widget)
 * @signal_cb void callback(Etk_Widget *widget, Etk_Event_Mouse_Up *event, void *data)
 * @signal_arg widget: the widget that has been clicked
 * @signal_arg event: The event data
 * @signal_data
 * \par
 * @signal_name "mouse_wheel": Emitted when the mouse wheel is used over the widget
 * @signal_cb void callback(Etk_Widget *widget, Etk_Event_Mouse_Wheel *event, void *data)
 * @signal_arg widget: the widget above which the mouse wheel has been used
 * @signal_arg event: The event data
 * @signal_data
 * \par
 * @signal_name "key_down": Emitted on the focused widget when a key has been pressed
 * @signal_cb void callback(Etk_Widget *widget, Etk_Event_Key_Down *event, void *data)
 * @signal_arg widget: the focused widget
 * @signal_arg event: The event data
 * @signal_data
 * \par
 * @signal_name "key_up": Emitted on the focused widget when a key has been released
 * @signal_cb void callback(Etk_Widget *widget, Etk_Event_Key_Up *event, void *data)
 * @signal_arg widget: the focused widget
 * @signal_arg event: The event data
 * @signal_data
 * \par
 * @signal_name "enter": Emitted when the widget is entered (not necessarily with the mouse)
 * @signal_cb void callback(Etk_Widget *widget, void *data)
 * @signal_arg widget: the widget that has been entered
 * @signal_data
 * \par
 * @signal_name "leave": Emitted when the widget is left (not necessarily with the mouse)
 * @signal_cb void callback(Etk_Widget *widget, void *data)
 * @signal_arg widget: the widget that has been left
 * @signal_data
 * \par
 * @signal_name "focus": Emitted when the widget is focused
 * @signal_cb void callback(Etk_Widget *widget, void *data)
 * @signal_arg widget: the widget that has been focused
 * @signal_data
 * \par
 * @signal_name "unfocus": Emitted when the widget is unfocused
 * @signal_cb void callback(Etk_Widget *widget, void *data)
 * @signal_arg widget: the widget that has been unfocused
 * @signal_data
 * \par
 * @signal_name "scroll_size_changed": Emitted when the scroll-size of the widget has changed
 * (only for widgets with a scrolling abilty)
 * @signal_cb void callback(Etk_Widget *widget, void *data)
 * @signal_arg widget: the widget whose scroll-size has changed
 * @signal_data
 * \par
 * @signal_name "selection_received": Emitted when a selection has been received (after a clipboard request for example)
 * @signal_cb void callback(Etk_Widget *widget, Etk_Selection_Event *event, void *data)
 * @signal_arg widget: the widget whose scroll-size has changed
 * @signal_arg event: the selection data
 * @signal_data
 *
 * (TODOC: dnd signals)
 *
 * \par Properties:
 * @prop_name "parent": The parent of the widget
 * @prop_type Pointer (Etk_Widget *)
 * @prop_ro
 * @prop_val NULL
 * \par
 * @prop_name "theme_file": The path to the theme-file used by the widget (NULL if the widget used
 * the default theme-file)
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "theme_group": The theme-group used by the widget
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "theme_parent": The theme-parent of the widget
 * @prop_type Pointer (Etk_Widget *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "geometry": The geometry of the widget (use etk_widget_geometry_get() to get it)
 * @prop_type Other (Etk_Geometry)
 * @prop_na
 * \par
 * @prop_name "width_request": The width requested for the widget (-1 means it's calculated automatically)
 * @prop_type Integer
 * @prop_rw
 * @prop_val -1
 * \par
 * @prop_name "height_request": The height requested for the widget (-1 means it's calculated automatically)
 * @prop_type Integer
 * @prop_rw
 * @prop_val -1
 * \par
 * @prop_name "visible": Whether or not the widget is visible
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 * \par
 * @prop_name "internal": Whether or not the widget is an internal widget. See etk_widget_internal_set() for more info
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 * \par
 * @prop_name "focusable": Whether or not the widget can be focused
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 * \par
 * @prop_name "focus_on_click": Whether or not the widget should be automatically focused when it is clicked
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 * \par
 * @prop_name "repeat_mouse_events": Whether or not the mouse-events received by the widget should
 * be propagated to its parent
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 * \par
 * @prop_name "pass_mouse_events": Whether or not the mouse-events received by the widget should
 * be ignored by the widget and directly passed to its parent
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 * \par
 * @prop_name "has_event_object": Whether or not the widget uses an event-object to grab the mouse-events
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 */
