/** @file etk_scale.c */
#include "etk_scale.h"
#include <stdlib.h>
#include <string.h>
#include <Edje.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Scale
* @{
 */

static void _etk_scale_constructor(Etk_Scale *scale);
static void _etk_hscale_constructor(Etk_HScale *hscale);
static void _etk_vscale_constructor(Etk_VScale *vscale);
static void _etk_hscale_realized_cb(Etk_Object *object, void *data);
static void _etk_vscale_realized_cb(Etk_Object *object, void *data);
static void _etk_scale_key_down_cb(Etk_Object *object, void *event, void *data);
static void _etk_hscale_cursor_dragged_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _etk_vscale_cursor_dragged_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _etk_hscale_value_changed_handler(Etk_Range *range, double value);
static void _etk_vscale_value_changed_handler(Etk_Range *range, double value);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Scale
 * @return Returns the type on an Etk_Scale
 */
Etk_Type *etk_scale_type_get()
{
   static Etk_Type *scale_type = NULL;

   if (!scale_type)
   {
      scale_type = etk_type_new("Etk_Scale", ETK_RANGE_TYPE, sizeof(Etk_Scale), ETK_CONSTRUCTOR(_etk_scale_constructor), NULL, NULL);
   }

   return scale_type;
}

/**
 * @brief Gets the type of an Etk_HScale
 * @return Returns the type on an Etk_HScale
 */
Etk_Type *etk_hscale_type_get()
{
   static Etk_Type *hscale_type = NULL;

   if (!hscale_type)
   {
      hscale_type = etk_type_new("Etk_HScale", ETK_SCALE_TYPE, sizeof(Etk_HScale), ETK_CONSTRUCTOR(_etk_hscale_constructor), NULL, NULL);
   }

   return hscale_type;
}

/**
 * @brief Creates a new horizontal scale
 * @return Returns the new horizontal scale widget
 */
Etk_Widget *etk_hscale_new(double lower, double upper, double value, double step_increment, double page_increment)
{
   return etk_widget_new(ETK_HSCALE_TYPE, "theme_group", "hscale", "focusable", TRUE, "lower", lower, "upper", upper,
      "value", value, "step_increment", step_increment, "page_increment", page_increment, NULL);
}

/**
 * @brief Gets the type of an Etk_VScale
 * @return Returns the type on an Etk_VScale
 */
Etk_Type *etk_vscale_type_get()
{
   static Etk_Type *vscale_type = NULL;

   if (!vscale_type)
   {
      vscale_type = etk_type_new("Etk_VScale", ETK_SCALE_TYPE, sizeof(Etk_VScale), ETK_CONSTRUCTOR(_etk_vscale_constructor), NULL, NULL);
   }

   return vscale_type;
}

/**
 * @brief Creates a new vertical scale
 * @return Returns the new vertical scale widget
 */
Etk_Widget *etk_vscale_new(double lower, double upper, double value, double step_increment, double page_increment)
{
   return etk_widget_new(ETK_VSCALE_TYPE, "theme_group", "vscale", "focusable", TRUE, "lower", lower, "upper", upper,
      "value", value, "step_increment", step_increment, "page_increment", page_increment, NULL);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the scale */
static void _etk_scale_constructor(Etk_Scale *scale)
{
   if (!scale)
      return;

   etk_signal_connect("key_down", ETK_OBJECT(scale), ETK_CALLBACK(_etk_scale_key_down_cb), NULL);
}

/* Initializes the default values of the hscale */
static void _etk_hscale_constructor(Etk_HScale *hscale)
{
   if (!hscale)
      return;

   ETK_RANGE(hscale)->value_changed = _etk_hscale_value_changed_handler;
   etk_signal_connect_after("realized", ETK_OBJECT(hscale), ETK_CALLBACK(_etk_hscale_realized_cb), NULL);
}

/* Initializes the default values of the vscale */
static void _etk_vscale_constructor(Etk_VScale *vscale)
{
   if (!vscale)
      return;

   ETK_RANGE(vscale)->value_changed = _etk_vscale_value_changed_handler;
   etk_signal_connect_after("realized", ETK_OBJECT(vscale), ETK_CALLBACK(_etk_vscale_realized_cb), NULL);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the hscale is realized */
static void _etk_hscale_realized_cb(Etk_Object *object, void *data)
{
   Evas_Object *theme_object;

   if (!object || !(theme_object = ETK_WIDGET(object)->theme_object))
      return;

   _etk_hscale_value_changed_handler(ETK_RANGE(object), ETK_RANGE(object)->value);
   edje_object_signal_callback_add(theme_object, "drag", "drag", _etk_hscale_cursor_dragged_cb, object);
}

/* Called when the vscale is realized */
static void _etk_vscale_realized_cb(Etk_Object *object, void *data)
{
   Evas_Object *theme_object;

   if (!object || !(theme_object = ETK_WIDGET(object)->theme_object))
      return;

   _etk_vscale_value_changed_handler(ETK_RANGE(object), ETK_RANGE(object)->value);
   edje_object_signal_callback_add(theme_object, "drag", "drag", _etk_vscale_cursor_dragged_cb, object);
}

/* Called when the user presses a key */
static void _etk_scale_key_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Key_Up_Down *key_event = event;
   Etk_Range *range;

   if (!(range = ETK_RANGE(object)))
      return;

   if (strcmp(key_event->key, "Right") == 0 || strcmp(key_event->key, "Down") == 0)
      etk_range_value_set(range, range->value + range->step_increment);
   else if (strcmp(key_event->key, "Left") == 0 || strcmp(key_event->key, "Up") == 0)
      etk_range_value_set(range, range->value - range->step_increment);
   else if (strcmp(key_event->key, "Home") == 0)
      etk_range_value_set(range, range->lower);
   else if (strcmp(key_event->key, "End") == 0)
      etk_range_value_set(range, range->upper);
   else if (strcmp(key_event->key, "Next") == 0)
      etk_range_value_set(range, range->value + range->page_increment);
   else if (strcmp(key_event->key, "Prior") == 0)
      etk_range_value_set(range, range->value - range->page_increment);
}

/* Called when the cursor of the hscale is dragged */
static void _etk_hscale_cursor_dragged_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Range *range;
   double x;

   if (!(range = ETK_RANGE(data)))
      return;

   edje_object_part_drag_value_get(obj, "drag", &x, NULL);
   etk_range_value_set(range, range->lower + x * (range->upper - range->lower));
}

/* Called when the cursor of the hscale is dragged */
static void _etk_vscale_cursor_dragged_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Range *range;
   double y;

   if (!(range = ETK_RANGE(data)))
      return;

   edje_object_part_drag_value_get(obj, "drag", NULL, &y);
   etk_range_value_set(range, range->lower + y * (range->upper - range->lower));
}

/* Default handler for the "value_changed" signal of an hscale */
static void _etk_hscale_value_changed_handler(Etk_Range *range, double value)
{
   Etk_HScale *hscale;
   Evas_Object *theme_object;

   if (!(hscale = ETK_HSCALE(range)) || !(theme_object = ETK_WIDGET(hscale)->theme_object))
      return;

   if (range->upper > range->lower)
      value = ETK_CLAMP(value / (range->upper - range->lower), 0.0, 1.0);
   else
      value = 0.0;
   edje_object_part_drag_value_set(theme_object, "drag", value, 0.0);
}

/* Default handler for the "value_changed" signal of a vscale */
static void _etk_vscale_value_changed_handler(Etk_Range *range, double value)
{
   Etk_VScale *vscale;
   Evas_Object *theme_object;

   if (!(vscale = ETK_VSCALE(range)) || !(theme_object = ETK_WIDGET(vscale)->theme_object))
      return;

   if (range->upper > range->lower)
      value = ETK_CLAMP(value / (range->upper - range->lower), 0.0, 1.0);
   else
      value = 0.0;
   edje_object_part_drag_value_set(theme_object, "drag", 0.0, value);
}

/** @} */
