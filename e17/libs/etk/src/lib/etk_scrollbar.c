/** @file etk_scrollbar.c */
#include "etk_scrollbar.h"
#include <stdlib.h>
#include <string.h>
#include <Edje.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Scrollbar
* @{
 */

#define ETK_SCROLLBAR_FIRST_DELAY 0.4
#define ETK_SCROLLBAR_REPEAT_DELAY 0.05

enum _Etk_Scrollbar_Property_Id
{
   ETK_SCROLLBAR_PAGE_SIZE_PROPERTY
};

static void _etk_scrollbar_constructor(Etk_Scrollbar *scrollbar);
static void _etk_scrollbar_destructor(Etk_Scrollbar *scrollbar);
static void _etk_hscrollbar_constructor(Etk_HScrollbar *hscrollbar);
static void _etk_vscrollbar_constructor(Etk_VScrollbar *vscrollbar);
static void _etk_scrollbar_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_scrollbar_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_hscrollbar_move_resize(Etk_Widget *widget, int x, int y, int w, int h);
static void _etk_vscrollbar_move_resize(Etk_Widget *widget, int x, int y, int w, int h);
static void _etk_hscrollbar_realized_cb(Etk_Object *object, void *data);
static void _etk_vscrollbar_realized_cb(Etk_Object *object, void *data);
static void _etk_scrollbar_value_changed_handler(Etk_Range *range, double value);

static void _etk_hscrollbar_drag_pressed_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_vscrollbar_drag_pressed_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_scrollbar_drag_released_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_hscrollbar_drag_moved_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_vscrollbar_drag_moved_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);

static void _etk_scrollbar_scroll_start_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _etk_scrollbar_scroll_stop_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static int _etk_scrollbar_step_decrement_cb(void *data);
static int _etk_scrollbar_step_increment_cb(void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Scrollbar
 * @return Returns the type on an Etk_Scrollbar
 */
Etk_Type *etk_scrollbar_type_get()
{
   static Etk_Type *scrollbar_type = NULL;

   if (!scrollbar_type)
   {
      scrollbar_type = etk_type_new("Etk_Scrollbar", ETK_RANGE_TYPE, sizeof(Etk_Scrollbar), ETK_CONSTRUCTOR(_etk_scrollbar_constructor), ETK_DESTRUCTOR(_etk_scrollbar_destructor), NULL);

      etk_type_property_add(scrollbar_type, "page_size", ETK_SCROLLBAR_PAGE_SIZE_PROPERTY, ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_double(0.0));
   
      scrollbar_type->property_set = _etk_scrollbar_property_set;
      scrollbar_type->property_get = _etk_scrollbar_property_get;
   }

   return scrollbar_type;
}

/**
 * @brief Gets the type of an Etk_HScrollbar
 * @return Returns the type on an Etk_HScrollbar
 */
Etk_Type *etk_hscrollbar_type_get()
{
   static Etk_Type *hscrollbar_type = NULL;

   if (!hscrollbar_type)
   {
      hscrollbar_type = etk_type_new("Etk_HScrollbar", ETK_SCROLLBAR_TYPE, sizeof(Etk_HScrollbar), ETK_CONSTRUCTOR(_etk_hscrollbar_constructor), NULL, NULL);
   }

   return hscrollbar_type;
}

/**
 * @brief Creates a new horizontal scrollbar
 * @param lower the lower bound of the scrollbar
 * @param upper the upper bound of the scrollbar
 * @param value the initial value of the scrollbar
 * @param step the step increment value. Used when the arrow of a scrollbar is clicked, or when the keyboard arrows are pressed (for a scale)
 * @param page the page increment value. Used when the trough of a scrollbar is clicked, or when page up/down are pressed
 * @param page_size the page size value of the scrollbar: this value is used to know the size of the drag button of the scrollbar
 * @return Returns the new horizontal scrollbar widget
 */
Etk_Widget *etk_hscrollbar_new(double lower, double upper, double value, double step_increment, double page_increment, double page_size)
{
   return etk_widget_new(ETK_HSCROLLBAR_TYPE, "theme_group", "hscrollbar", "focusable", TRUE, "lower", lower, "upper", upper,
      "value", value, "step_increment", step_increment, "page_increment", page_increment, "page_size", page_size, NULL);
}

/**
 * @brief Gets the type of an Etk_VScrollbar
 * @return Returns the type on an Etk_VScrollbar
 */
Etk_Type *etk_vscrollbar_type_get()
{
   static Etk_Type *vscrollbar_type = NULL;

   if (!vscrollbar_type)
   {
      vscrollbar_type = etk_type_new("Etk_VScrollbar", ETK_SCROLLBAR_TYPE, sizeof(Etk_VScrollbar), ETK_CONSTRUCTOR(_etk_vscrollbar_constructor), NULL, NULL);
   }

   return vscrollbar_type;
}

/**
 * @brief Creates a new vertical scrollbar
 * @param lower the lower bound of the scrollbar
 * @param upper the upper bound of the scrollbar
 * @param value the initial value of the scrollbar
 * @param step the step increment value. Used when the arrow of a scrollbar is clicked, or when the keyboard arrows are pressed (for a scale)
 * @param page the page increment value. Used when the trough of a scrollbar is clicked, or when page up/down are pressed
 * @param page_size the page size value of the scrollbar: this value is used to know the size of the drag button of the scrollbar
 * @return Returns the new vertical scrollbar widget
 */
Etk_Widget *etk_vscrollbar_new(double lower, double upper, double value, double step_increment, double page_increment, double page_size)
{
   return etk_widget_new(ETK_VSCROLLBAR_TYPE, "theme_group", "vscrollbar", "focusable", TRUE, "lower", lower, "upper", upper,
      "value", value, "step_increment", step_increment, "page_increment", page_increment, "page_size", page_size, NULL);
}

/**
 * @brief Sets the page size of the scrollbar:this value is used to know the size of the drag button of the scrollbar
 * @param scrollbar a scrollbar
 * @param page_size the value to set
 */
void etk_scrollbar_page_size_set(Etk_Scrollbar *scrollbar, double page_size)
{
   if (!scrollbar)
      return;

   scrollbar->page_size = page_size;
   etk_object_notify(ETK_OBJECT(scrollbar), "page_size");
   etk_widget_redraw_queue(ETK_WIDGET(scrollbar));
}

/**
 * @brief Gets the page size of the scrollbar
 * @param scrollbar a scrollbar
 * @return Returns the page size of the scrollbar
 */
double etk_scrollbar_page_size_get(Etk_Scrollbar *scrollbar)
{
   if (!scrollbar)
      return 0.0;

   return scrollbar->page_size;
}


/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the scrollbar */
static void _etk_scrollbar_constructor(Etk_Scrollbar *scrollbar)
{
   if (!scrollbar)
      return;

   scrollbar->page_size = 0.0;
   scrollbar->drag_button = NULL;
   scrollbar->drag_orig_position = -1;
   scrollbar->drag_size = 0;
   scrollbar->confine_size = 0;
   scrollbar->scrolling_timer = NULL;
   scrollbar->first_scroll = FALSE;
   ETK_RANGE(scrollbar)->value_changed = _etk_scrollbar_value_changed_handler;
}

/* Destroys the scrollbar */
static void _etk_scrollbar_destructor(Etk_Scrollbar *scrollbar)
{
   if (!scrollbar)
      return;

   if (scrollbar->scrolling_timer)
      ecore_timer_del(scrollbar->scrolling_timer);
}

/* Initializes the default values of the hscrollbar */
static void _etk_hscrollbar_constructor(Etk_HScrollbar *hscrollbar)
{
   if (!hscrollbar)
      return;

   ETK_WIDGET(hscrollbar)->move_resize = _etk_hscrollbar_move_resize;
   etk_signal_connect_after("realized", ETK_OBJECT(hscrollbar), ETK_CALLBACK(_etk_hscrollbar_realized_cb), NULL);
}

/* Initializes the default values of the vscrollbar */
static void _etk_vscrollbar_constructor(Etk_VScrollbar *vscrollbar)
{
   if (!vscrollbar)
      return;

   ETK_WIDGET(vscrollbar)->move_resize = _etk_vscrollbar_move_resize;
   etk_signal_connect_after("realized", ETK_OBJECT(vscrollbar), ETK_CALLBACK(_etk_vscrollbar_realized_cb), NULL);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_scrollbar_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Scrollbar *scrollbar;

   if (!(scrollbar = ETK_SCROLLBAR(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_SCROLLBAR_PAGE_SIZE_PROPERTY:
         etk_scrollbar_page_size_set(scrollbar, etk_property_value_double_get(value)); 
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_scrollbar_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Scrollbar *scrollbar;

   if (!(scrollbar = ETK_SCROLLBAR(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_SCROLLBAR_PAGE_SIZE_PROPERTY:
         etk_property_value_double_set(value, scrollbar->page_size);
         break;
   }
}

/* Moves and resizes the hscrollbar */ 
static void _etk_hscrollbar_move_resize(Etk_Widget *widget, int x, int y, int w, int h)
{
   Etk_Scrollbar *scrollbar;
   Etk_Range *range;
   Evas_Coord tx, ty, cx, cy, cw, ch, mw;
   float x_offset, drag_width; 

   if (!(scrollbar = ETK_SCROLLBAR(widget)) || !widget->theme_object || !scrollbar->drag_button)
      return;

   range = ETK_RANGE(scrollbar);
   evas_object_geometry_get(widget->theme_object, &tx, &ty, NULL, NULL);
   edje_object_part_geometry_get(widget->theme_object, "confine", &cx, &cy, &cw, &ch);
   edje_object_size_min_get(scrollbar->drag_button, &mw, NULL);

   if (scrollbar->page_size > 0 && range->lower < range->upper)
   {
      drag_width = ((float)scrollbar->page_size / (range->upper - range->lower)) * cw;
      drag_width = ETK_MAX(mw, drag_width);
      x_offset = (cw - drag_width) * ((float)(range->value - range->lower) / (range->upper - range->lower));
   }
   else
   {
      drag_width = 0.3 * cw;
      drag_width = ETK_MAX(mw, drag_width);
      if (range->lower < range->upper)
         x_offset = (cw - drag_width) * ((float)(range->value - range->lower) / (range->upper - range->lower));
      else
         x_offset = 0.0;
   }
   evas_object_move(ETK_SCROLLBAR(widget)->drag_button, tx + cx + x_offset, ty + cy);
   evas_object_resize(ETK_SCROLLBAR(widget)->drag_button, drag_width, ch);

   ETK_SCROLLBAR(widget)->confine_size = cw;
   ETK_SCROLLBAR(widget)->drag_size = drag_width;
}

/* Moves and resizes the vscrollbar */ 
static void _etk_vscrollbar_move_resize(Etk_Widget *widget, int x, int y, int w, int h)
{
   Etk_Scrollbar *scrollbar;
   Etk_Range *range;
   Evas_Coord tx, ty, cx, cy, cw, ch, mh;
   float y_offset, drag_height; 

   if (!(scrollbar = ETK_SCROLLBAR(widget)) || !widget->theme_object || !scrollbar->drag_button)
      return;

   range = ETK_RANGE(scrollbar);
   evas_object_geometry_get(widget->theme_object, &tx, &ty, NULL, NULL);
   edje_object_part_geometry_get(widget->theme_object, "confine", &cx, &cy, &cw, &ch);
   edje_object_size_min_get(scrollbar->drag_button, NULL, &mh);

   if (scrollbar->page_size > 0 && range->lower < range->upper)
   {
      drag_height = ((float)scrollbar->page_size / (range->upper - range->lower)) * ch;
      drag_height = ETK_MAX(mh, drag_height);
      y_offset = (ch - drag_height) * ((float)(range->value - range->lower) / (range->upper - range->lower));
   }
   else
   {
      drag_height = 0.3 * ch;
      drag_height = ETK_MAX(mh, drag_height);
      if (range->lower < range->upper)
         y_offset = (ch - drag_height) * ((float)(range->value - range->lower) / (range->upper - range->lower));
      else
         y_offset = 0.0;
   }
   evas_object_move(ETK_SCROLLBAR(widget)->drag_button, tx + cx, ty + cy + y_offset);
   evas_object_resize(ETK_SCROLLBAR(widget)->drag_button, cw, drag_height);
   
   ETK_SCROLLBAR(widget)->confine_size = ch;
   ETK_SCROLLBAR(widget)->drag_size = drag_height;
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the hscrollbar is realized */
static void _etk_hscrollbar_realized_cb(Etk_Object *object, void *data)
{
   Etk_Scrollbar *scrollbar;
   Etk_Widget *scrollbar_widget;
   Evas *evas;

   if (!(scrollbar_widget = ETK_WIDGET(object)) || !(evas = etk_widget_toplevel_evas_get(scrollbar_widget)))
      return;
   scrollbar = ETK_SCROLLBAR(scrollbar_widget);

   scrollbar->drag_button = edje_object_add(evas);
   edje_object_file_set(scrollbar->drag_button, scrollbar_widget->theme_file, "hscrollbar_drag");
   evas_object_show(scrollbar->drag_button);
   etk_widget_member_object_add(scrollbar_widget, scrollbar->drag_button);

   evas_object_event_callback_add(scrollbar->drag_button, EVAS_CALLBACK_MOUSE_DOWN, _etk_hscrollbar_drag_pressed_cb, scrollbar);
   evas_object_event_callback_add(scrollbar->drag_button, EVAS_CALLBACK_MOUSE_UP, _etk_scrollbar_drag_released_cb, scrollbar);
   evas_object_event_callback_add(scrollbar->drag_button, EVAS_CALLBACK_MOUSE_MOVE, _etk_hscrollbar_drag_moved_cb, scrollbar);

   edje_object_signal_callback_add(scrollbar_widget->theme_object, "scroll_left_start", "", _etk_scrollbar_scroll_start_cb, scrollbar);
   edje_object_signal_callback_add(scrollbar_widget->theme_object, "scroll_right_start", "", _etk_scrollbar_scroll_start_cb, scrollbar);
   edje_object_signal_callback_add(scrollbar_widget->theme_object, "scroll_stop", "", _etk_scrollbar_scroll_stop_cb, scrollbar);
}

/* Called when the vscrollbar is realized */
static void _etk_vscrollbar_realized_cb(Etk_Object *object, void *data)
{
   Etk_Scrollbar *scrollbar;
   Etk_Widget *scrollbar_widget;
   Evas *evas;

   if (!(scrollbar_widget = ETK_WIDGET(object)) || !(evas = etk_widget_toplevel_evas_get(scrollbar_widget)))
      return;
   scrollbar = ETK_SCROLLBAR(scrollbar_widget);

   scrollbar->drag_button = edje_object_add(evas);
   edje_object_file_set(scrollbar->drag_button, scrollbar_widget->theme_file, "vscrollbar_drag");
   evas_object_show(scrollbar->drag_button);
   etk_widget_member_object_add(scrollbar_widget, scrollbar->drag_button);

   evas_object_event_callback_add(scrollbar->drag_button, EVAS_CALLBACK_MOUSE_DOWN, _etk_vscrollbar_drag_pressed_cb, scrollbar);
   evas_object_event_callback_add(scrollbar->drag_button, EVAS_CALLBACK_MOUSE_UP, _etk_scrollbar_drag_released_cb, scrollbar);
   evas_object_event_callback_add(scrollbar->drag_button, EVAS_CALLBACK_MOUSE_MOVE, _etk_vscrollbar_drag_moved_cb, scrollbar);

   edje_object_signal_callback_add(scrollbar_widget->theme_object, "scroll_up_start", "", _etk_scrollbar_scroll_start_cb, scrollbar);
   edje_object_signal_callback_add(scrollbar_widget->theme_object, "scroll_down_start", "", _etk_scrollbar_scroll_start_cb, scrollbar);
   edje_object_signal_callback_add(scrollbar_widget->theme_object, "scroll_stop", "", _etk_scrollbar_scroll_stop_cb, scrollbar);
}

/* Called when the value of the scrollbar is changed */
static void _etk_scrollbar_value_changed_handler(Etk_Range *range, double value)
{
   if (!range)
      return;

   etk_widget_redraw_queue(ETK_WIDGET(range));
}

/* Called when the user presses the drag button of a hscrollbar */
static void _etk_hscrollbar_drag_pressed_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Evas_Event_Mouse_Down *event = event_info;
   Etk_Scrollbar *scrollbar;

   if (!(scrollbar = ETK_SCROLLBAR(data)))
      return;

   scrollbar->drag_orig_position = event->canvas.x;
   scrollbar->drag_orig_value = ETK_RANGE(scrollbar)->value;
}

/* Called when the user presses the drag button of a vscrollbar */
static void _etk_vscrollbar_drag_pressed_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Evas_Event_Mouse_Down *event = event_info;
   Etk_Scrollbar *scrollbar;

   if (!(scrollbar = ETK_SCROLLBAR(data)))
      return;

   scrollbar->drag_orig_position = event->canvas.y;
   scrollbar->drag_orig_value = ETK_RANGE(scrollbar)->value;
}

/* Called when the user releases the drag button of a scrollbar */
static void _etk_scrollbar_drag_released_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Scrollbar *scrollbar;

   if (!(scrollbar = ETK_SCROLLBAR(data)))
      return;

   scrollbar->drag_orig_position = -1;
}

/* Called when the user moves the mouse above the drag button of a hscrollbar */
static void _etk_hscrollbar_drag_moved_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Evas_Event_Mouse_Move *event = event_info;
   Etk_Scrollbar *scrollbar;
   double new_value;

   if (!(scrollbar = ETK_SCROLLBAR(data)) || scrollbar->drag_orig_position < 0 || scrollbar->confine_size <= scrollbar->drag_size)
      return;
   
   new_value = scrollbar->drag_orig_value + ((float)(event->cur.canvas.x - scrollbar->drag_orig_position) / (scrollbar->confine_size - scrollbar->drag_size)) *
      (ETK_RANGE(scrollbar)->upper - ETK_RANGE(scrollbar)->lower);
   etk_range_value_set(ETK_RANGE(scrollbar), new_value);
}

/* Called when the user moves the mouse above the drag button of a vscrollbar */
static void _etk_vscrollbar_drag_moved_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Evas_Event_Mouse_Move *event = event_info;
   Etk_Scrollbar *scrollbar;
   double new_value;

   if (!(scrollbar = ETK_SCROLLBAR(data)) || scrollbar->drag_orig_position < 0 || scrollbar->confine_size <= scrollbar->drag_size)
      return;
   
   new_value = scrollbar->drag_orig_value + ((float)(event->cur.canvas.y - scrollbar->drag_orig_position) / (scrollbar->confine_size - scrollbar->drag_size)) *
      (ETK_RANGE(scrollbar)->upper - ETK_RANGE(scrollbar)->lower);
   etk_range_value_set(ETK_RANGE(scrollbar), new_value);
}

/* TODO doc */
static void _etk_scrollbar_scroll_start_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Scrollbar *scrollbar;

   if (!(scrollbar = ETK_SCROLLBAR(data)) || !emission)
      return;

   if (scrollbar->scrolling_timer)
   {
      ecore_timer_del(scrollbar->scrolling_timer);
      scrollbar->scrolling_timer = NULL;
   }

   if (strcmp(emission, "scroll_left_start") == 0 || strcmp(emission, "scroll_up_start") == 0)
   {
      scrollbar->scrolling_timer = ecore_timer_add(0.0, _etk_scrollbar_step_decrement_cb, scrollbar);
      scrollbar->first_scroll = TRUE;
   }
   else if (strcmp(emission, "scroll_right_start") == 0 || strcmp(emission, "scroll_down_start") == 0)
   {
      scrollbar->scrolling_timer = ecore_timer_add(0.0, _etk_scrollbar_step_increment_cb, scrollbar);
      scrollbar->first_scroll = TRUE;
   }
}

/* TODO doc */
static void _etk_scrollbar_scroll_stop_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Scrollbar *scrollbar;

   if (!(scrollbar = ETK_SCROLLBAR(data)) || !scrollbar->scrolling_timer)
      return;

   ecore_timer_del(scrollbar->scrolling_timer);
   scrollbar->scrolling_timer = NULL;
}

/* TODO doc */
static int _etk_scrollbar_step_decrement_cb(void *data)
{
   Etk_Scrollbar *scrollbar;

   if (!(scrollbar = ETK_SCROLLBAR(data)))
      return 1;

   etk_range_value_set(ETK_RANGE(scrollbar), ETK_RANGE(scrollbar)->value - ETK_RANGE(scrollbar)->step_increment);
   if (scrollbar->first_scroll)
   {
      ecore_timer_interval_set(scrollbar->scrolling_timer, ETK_SCROLLBAR_FIRST_DELAY);
      scrollbar->first_scroll = FALSE;
   }
   else
      ecore_timer_interval_set(scrollbar->scrolling_timer, ETK_SCROLLBAR_REPEAT_DELAY);
   return 1;
}

/* TODO doc */
static int _etk_scrollbar_step_increment_cb(void *data)
{
   Etk_Scrollbar *scrollbar;

   if (!(scrollbar = ETK_SCROLLBAR(data)))
      return 1;

   etk_range_value_set(ETK_RANGE(scrollbar), ETK_RANGE(scrollbar)->value + ETK_RANGE(scrollbar)->step_increment);
   if (scrollbar->first_scroll)
   {
      ecore_timer_interval_set(scrollbar->scrolling_timer, ETK_SCROLLBAR_FIRST_DELAY);
      scrollbar->first_scroll = FALSE;
   }
   else
      ecore_timer_interval_set(scrollbar->scrolling_timer, ETK_SCROLLBAR_REPEAT_DELAY);
   return 1;
}

/** @} */
