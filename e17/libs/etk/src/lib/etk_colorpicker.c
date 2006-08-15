/** @file etk_colorpicker.c */
#include "etk_colorpicker.h"
#include <stdlib.h>
#include <stdint.h>
#include <Edje.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"
#include "etk_table.h"
#include "etk_slider.h"
#include "etk_label.h"
#include "etk_radio_button.h"
#include "etk_theme.h"

/**
 * @addtogroup Etk_Colorpicker
 * @{
 */

typedef struct Etk_Colorpicker_Picker_SD
{
   Evas_List *objects;
   Etk_Colorpicker *cp;
   void (*move_resize)(Etk_Colorpicker *cp, int x, int y, int w, int h);
} Etk_Colorpicker_Picker_SD;

enum Etk_Combobox_Signal_Id
{
   ETK_CP_COLOR_CHANGED_SIGNAL,
   ETK_CP_NUM_SIGNALS
};

enum Etk_Colorpicker_Property_Id
{
   ETK_CP_MODE_PROPERTY
};

static void _etk_colorpicker_constructor(Etk_Colorpicker *cp);
static void _etk_colorpicker_destructor(Etk_Colorpicker *cp);
static void _etk_colorpicker_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_colorpicker_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_colorpicker_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_colorpicker_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

static void _etk_colorpicker_realize_cb(Etk_Object *object, void *data);
static void _etk_colorpicker_unrealize_cb(Etk_Object *object, void *data);
static void _etk_colorpicker_slider_realize_cb(Etk_Object *object, void *data);
static void _etk_colorpicker_slider_unrealize_cb(Etk_Object *object, void *data);
static void _etk_colorpicker_current_color_realize_cb(Etk_Object *object, void *data);
static void _etk_colorpicker_current_color_unrealize_cb(Etk_Object *object, void *data);
static void _etk_colorpicker_slider_value_changed_cb(Etk_Object *object, double value, void *data);
static void _etk_colorpicker_radio_toggled_cb(Etk_Object *object, void *data);

static void _etk_colorpicker_sp_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_colorpicker_sp_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_colorpicker_sp_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_colorpicker_vp_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_colorpicker_vp_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_colorpicker_vp_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_colorpicker_slider_image_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

static Evas_Object *_etk_colorpicker_picker_object_add(Evas *evas, Etk_Colorpicker *cp, void (*move_resize)(Etk_Colorpicker *cp, int x, int y, int w, int h));
static void _etk_colorpicker_picker_smart_add(Evas_Object *obj);
static void _etk_colorpicker_picker_smart_del(Evas_Object *obj);
static void _etk_colorpicker_picker_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _etk_colorpicker_picker_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _etk_colorpicker_picker_smart_show(Evas_Object *obj);
static void _etk_colorpicker_picker_smart_hide(Evas_Object *obj);
static void _etk_colorpicker_picker_smart_clip_set(Evas_Object *obj, Evas_Object *clip);
static void _etk_colorpicker_picker_smart_clip_unset(Evas_Object *obj);

static void _etk_colorpicker_sp_move_resize(Etk_Colorpicker *cp, int x, int y, int w, int h);
static void _etk_colorpicker_vp_move_resize(Etk_Colorpicker *cp, int x, int y, int w, int h);
static void _etk_colorpicker_sp_cursor_replace(Etk_Colorpicker *cp);
static void _etk_colorpicker_vp_cursor_replace(Etk_Colorpicker *cp);

static void _etk_colorpicker_sp_cursor_move(Etk_Colorpicker *cp, float xpercent, float ypercent);
static void _etk_colorpicker_update(Etk_Colorpicker *cp, Etk_Bool sp_image, Etk_Bool sp_cursor, Etk_Bool vp_image, Etk_Bool vp_cursor);
static void _etk_colorpicker_sp_image_update(Etk_Colorpicker *cp);
static void _etk_colorpicker_sp_cursor_update(Etk_Colorpicker *cp);
static void _etk_colorpicker_vp_image_update(Etk_Colorpicker *cp);
static void _etk_colorpicker_vp_cursor_update(Etk_Colorpicker *cp);
static void _etk_colorpicker_sliders_update(Etk_Colorpicker *cp);
static void _etk_colorpicker_sp_color_get(Etk_Colorpicker *cp, int i, int j, int *r, int *g, int *b);
static void _etk_colorpicker_vp_color_get(Etk_Colorpicker *cp, int i, int *r, int *g, int *b);
static void _etk_colorpicker_color_calc(Etk_Colorpicker_Mode mode, float sp_xpos, float sp_ypos, float vp_pos, int *r, int *g, int *b);

static Evas_Smart *_etk_colorpicker_picker_smart = NULL;
static int _etk_colorpicker_picker_smart_use = 0;
static Etk_Signal *_etk_colorpicker_signals[ETK_CP_NUM_SIGNALS];


/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Colorpicker
 * @return Returns the type of an Etk_Colorpicker
 */
Etk_Type *etk_colorpicker_type_get()
{
   static Etk_Type *cp_type = NULL;

   if (!cp_type)
   {
      cp_type = etk_type_new("Etk_Colorpicker", ETK_WIDGET_TYPE, sizeof(Etk_Colorpicker),
         ETK_CONSTRUCTOR(_etk_colorpicker_constructor), ETK_DESTRUCTOR(_etk_colorpicker_destructor));
   
      _etk_colorpicker_signals[ETK_CP_COLOR_CHANGED_SIGNAL] = etk_signal_new("color_changed",
         cp_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      
      etk_type_property_add(cp_type, "mode", ETK_CP_MODE_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_COLORPICKER_H));
   
      cp_type->property_set = _etk_colorpicker_property_set;
      cp_type->property_get = _etk_colorpicker_property_get;
   }

   return cp_type;
}

/**
 * @brief Creates a new colorpicker
 * @return Returns the new colorpicker
 */
Etk_Widget *etk_colorpicker_new()
{
   return etk_widget_new(ETK_COLORPICKER_TYPE, "theme_group", "colorpicker", NULL);
}

/**
 * @brief Sets the current color mode of the colorpicker
 * @param cp a colorpicker
 * @param mode the color mode to use
 */
void etk_colorpicker_mode_set(Etk_Colorpicker *cp, Etk_Colorpicker_Mode mode)
{
   float value;
   int i;
   
   if (!cp || (cp->mode == mode))
      return;
   
   switch (mode)
   {
      case ETK_COLORPICKER_H:
         cp->sp_xcomponent = ETK_COLORPICKER_V;
         cp->sp_ycomponent = ETK_COLORPICKER_S;
         break;
      case ETK_COLORPICKER_S:
         cp->sp_xcomponent = ETK_COLORPICKER_V;
         cp->sp_ycomponent = ETK_COLORPICKER_H;
         break;
      case ETK_COLORPICKER_V:
         cp->sp_xcomponent = ETK_COLORPICKER_S;
         cp->sp_ycomponent = ETK_COLORPICKER_H;
         break;
      case ETK_COLORPICKER_R:
         cp->sp_xcomponent = ETK_COLORPICKER_B;
         cp->sp_ycomponent = ETK_COLORPICKER_G;
         break;
      case ETK_COLORPICKER_G:
         cp->sp_xcomponent = ETK_COLORPICKER_B;
         cp->sp_ycomponent = ETK_COLORPICKER_R;
         break;
      case ETK_COLORPICKER_B:
         cp->sp_xcomponent = ETK_COLORPICKER_G;
         cp->sp_ycomponent = ETK_COLORPICKER_R;
         break;
      default:
         break;
   }
   
   
   /* Update of the colorpicker */
   for (i = (mode / 3) * 3; i < ((mode / 3) + 1) * 3; i++)
   {
      value = etk_range_value_get(ETK_RANGE(cp->sliders[i]));
      if (i == mode)
         cp->vp_pos = value / cp->sliders_max_value[i];
      else if (i == cp->sp_xcomponent)
         cp->sp_xpos = value / cp->sliders_max_value[i];
      else if (i == cp->sp_ycomponent)
         cp->sp_ypos = value / cp->sliders_max_value[i];
   }
   _etk_colorpicker_update(cp, ETK_TRUE, ETK_TRUE, ETK_TRUE, ETK_TRUE);
   
   cp->mode = mode;
   etk_object_notify(ETK_OBJECT(cp), "mode");
}

/**
 * @brief Gets the current color mode of the colorpicker
 * @param cp a colorpicker
 * @return Returns the current color mode of the colorpicker
 */
Etk_Colorpicker_Mode etk_colorpicker_mode_get(Etk_Colorpicker *cp)
{
   if (!cp)
      return ETK_COLORPICKER_H;
   return cp->mode;
}

/**
 * @brief Sets the color currently selected by the colorpicker
 * @param cp a colorpicker
 * @param color the color to set
 */
void etk_colorpicker_current_color_set(Etk_Colorpicker *cp, Etk_Color color)
{
   if (cp->current_color.r == color.r && cp->current_color.g == color.g && cp->current_color.b == color.b)
      return;
   
   cp->ignore_value_changed = ETK_TRUE;
   etk_range_value_set(ETK_RANGE(cp->sliders[ETK_COLORPICKER_R]), color.r);
   etk_range_value_set(ETK_RANGE(cp->sliders[ETK_COLORPICKER_G]), color.g);
   cp->ignore_value_changed = ETK_FALSE;
   etk_range_value_set(ETK_RANGE(cp->sliders[ETK_COLORPICKER_B]), color.b);
}

/**
 * @brief Gets the color currently selected by the colorpicker
 * @param cp a colorpicker
 * @return Returns the color selected by the colorpicker
 */
Etk_Color etk_colorpicker_current_color_get(Etk_Colorpicker *cp)
{
   if (!cp)
   {
      Etk_Color black;
      black.r = 0;
      black.g = 0;
      black.b = 0;
      black.a = 255;
      return black;
   }
   
   return cp->current_color;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the colorpicker */
static void _etk_colorpicker_constructor(Etk_Colorpicker *cp)
{
   Etk_Widget *cp_widget;
   char *labels[6] = { _("H"), _("S"), _("V"), _("R"), _("G"), _("B") };
   float values[6] = { 360.0, 1.0, 1.0, 255.0, 255.0, 255.0 };
   float steps[6] = { 1.0, 0.01, 0.01, 1.0, 1.0, 1.0 };
   int i;

   if (!(cp_widget = ETK_WIDGET(cp)))
      return;
   
   cp->mode = ETK_COLORPICKER_H;
   cp->sp_xcomponent = ETK_COLORPICKER_V;
   cp->sp_ycomponent = ETK_COLORPICKER_S;
   
   cp->current_color.r = 0;
   cp->current_color.g = 0;
   cp->current_color.b = 0;
   cp->current_color.a = 255;
      
   cp->sp_object = NULL;
   cp->sp_image = NULL;
   cp->sp_hcursor = NULL;
   cp->sp_vcursor = NULL;
   cp->sp_res = 64;
   cp->sp_xpos = 0.0;
   cp->sp_ypos = 0.0;
   
   cp->vp_object = NULL;
   cp->vp_image = NULL;
   cp->vp_cursor = NULL;
   cp->vp_res = 256;
   cp->vp_pos = 0.0;
   
   cp->current_color_rect = NULL;
   
   cp->sliders_res = 256;
   cp->sp_dragging = ETK_FALSE;
   cp->vp_dragging = ETK_FALSE;
   cp->sp_image_needs_update = ETK_FALSE;
   cp->sp_cursor_needs_update = ETK_FALSE;
   cp->vp_image_needs_update = ETK_FALSE;
   cp->vp_cursor_needs_update = ETK_FALSE;
   cp->sliders_need_update = ETK_FALSE;
   cp->ignore_value_changed = ETK_FALSE;
   cp->emit_signal = ETK_TRUE;
   
   
   /* We create the widgets */
   cp->main_table = etk_table_new(2, 2, ETK_FALSE);
   etk_widget_parent_set(cp->main_table, cp_widget);
   etk_widget_visibility_locked_set(cp->main_table, ETK_TRUE);
   etk_widget_show(cp->main_table);
   
   cp->picker_widget = etk_widget_new(ETK_WIDGET_TYPE, "theme_group", "picker", NULL);
   etk_table_attach_default(ETK_TABLE(cp->main_table), cp->picker_widget, 0, 0, 0, 0);
   etk_widget_visibility_locked_set(cp->picker_widget, ETK_TRUE);
   etk_widget_show(cp->picker_widget);
   
   cp->component_table = etk_table_new(3, 6, ETK_FALSE);
   etk_table_attach_default(ETK_TABLE(cp->main_table), cp->component_table, 1, 1, 0, 0);
   etk_widget_visibility_locked_set(cp->component_table, ETK_TRUE);
   etk_widget_show(cp->component_table);
   
   /* Radios, sliders and labels for each component */
   for (i = 0; i < 6; i++)
   {
      if (i == 0)
         cp->radios[i] = etk_radio_button_new_with_label(labels[i], NULL);
      else
         cp->radios[i] = etk_radio_button_new_with_label_from_widget(labels[i], ETK_RADIO_BUTTON(cp->radios[0]));
      etk_table_attach(ETK_TABLE(cp->component_table), cp->radios[i], 0, 0, i, i,
         0, 0, ETK_TABLE_HFILL | ETK_TABLE_VEXPAND);
      etk_widget_visibility_locked_set(cp->radios[i], ETK_TRUE);
      etk_widget_show(cp->radios[i]);
      
      cp->sliders[i] = etk_hslider_new(0.0, values[i], 0.0, steps[i], steps[i] * 5);
      etk_table_attach(ETK_TABLE(cp->component_table), cp->sliders[i], 1, 1, i, i,
         0, 0, ETK_TABLE_HFILL | ETK_TABLE_EXPAND);
      etk_widget_visibility_locked_set(cp->sliders[i], ETK_TRUE);
      etk_widget_show(cp->sliders[i]);
      
      cp->sliders_max_value[i] = values[i];
      cp->sliders_image[i] = NULL;
      
      cp->value_labels[i] = etk_label_new("0");
      etk_table_attach(ETK_TABLE(cp->component_table), cp->value_labels[i], 2, 2, i, i,
         0, 0, ETK_TABLE_HFILL | ETK_TABLE_VEXPAND);
      etk_widget_size_request_set(cp->value_labels[i], 28, -1);
      etk_widget_visibility_locked_set(cp->value_labels[i], ETK_TRUE);
      etk_widget_show(cp->value_labels[i]);
      
      etk_signal_connect("toggled", ETK_OBJECT(cp->radios[i]),
         ETK_CALLBACK(_etk_colorpicker_radio_toggled_cb), cp);
      etk_signal_connect("realize", ETK_OBJECT(cp->sliders[i]),
         ETK_CALLBACK(_etk_colorpicker_slider_realize_cb), cp);
      etk_signal_connect("unrealize", ETK_OBJECT(cp->sliders[i]),
         ETK_CALLBACK(_etk_colorpicker_slider_unrealize_cb), cp);
      etk_signal_connect("value_changed", ETK_OBJECT(cp->sliders[i]),
         ETK_CALLBACK(_etk_colorpicker_slider_value_changed_cb), cp);
   }
   
   /* We create the widgets for the color preview */
   cp->color_table = etk_table_new(2, 1, ETK_FALSE);
   etk_table_attach(ETK_TABLE(cp->main_table), cp->color_table, 0, 0, 1, 1,
      0, 0, ETK_TABLE_FILL | ETK_TABLE_HEXPAND);
   etk_widget_visibility_locked_set(cp->color_table, ETK_TRUE);
   etk_widget_show(cp->color_table);
   
   /* Current color preview */
   cp->current_color_label = etk_label_new(_("Current"));
   etk_table_attach(ETK_TABLE(cp->color_table), cp->current_color_label, 0, 0, 0, 0,
      0, 0, ETK_TABLE_HFILL);
   etk_widget_visibility_locked_set(cp->current_color_label, ETK_TRUE);
   etk_widget_show(cp->current_color_label);
   
   cp->current_color_widget = etk_widget_new(ETK_WIDGET_TYPE, "theme_group", "color_preview", NULL);
   etk_table_attach(ETK_TABLE(cp->color_table), cp->current_color_widget, 1, 1, 0, 0,
      0, 0, ETK_TABLE_HFILL | ETK_TABLE_HEXPAND);
   etk_widget_visibility_locked_set(cp->current_color_widget, ETK_TRUE);
   etk_widget_show(cp->current_color_widget);
   
   
   cp_widget->size_request = _etk_colorpicker_size_request;
   cp_widget->size_allocate = _etk_colorpicker_size_allocate;
   
   etk_signal_connect("realize", ETK_OBJECT(cp->picker_widget), ETK_CALLBACK(_etk_colorpicker_realize_cb), cp);
   etk_signal_connect("unrealize", ETK_OBJECT(cp->picker_widget), ETK_CALLBACK(_etk_colorpicker_unrealize_cb), cp);
   etk_signal_connect("realize", ETK_OBJECT(cp->current_color_widget),
      ETK_CALLBACK(_etk_colorpicker_current_color_realize_cb), cp);
   etk_signal_connect("unrealize", ETK_OBJECT(cp->current_color_widget),
      ETK_CALLBACK(_etk_colorpicker_current_color_unrealize_cb), cp);
}

/* Destroys the colorpicker */
static void _etk_colorpicker_destructor(Etk_Colorpicker *cp)
{
   int i;
   
   if (!cp)
      return;
   
   for (i = 0; i < 6; i++)
      etk_signal_disconnect("toggled", ETK_OBJECT(cp->radios[i]), ETK_CALLBACK(_etk_colorpicker_radio_toggled_cb));
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_colorpicker_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Colorpicker *cp;

   if (!(cp = ETK_COLORPICKER(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_CP_MODE_PROPERTY:
         etk_colorpicker_mode_set(cp, etk_property_value_int_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_colorpicker_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Colorpicker *cp;

   if (!(cp = ETK_COLORPICKER(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_CP_MODE_PROPERTY:
         etk_property_value_int_set(value, cp->mode);
         break;
      default:
         break;
   }
}

/* Calculates the ideal size of the colorpicker */
static void _etk_colorpicker_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_Colorpicker *cp;
   
   if (!(cp = ETK_COLORPICKER(widget)) || !size)
      return;
   etk_widget_size_request(cp->main_table, size);
}

/* Resizes the colorpicker to the allocated size */
static void _etk_colorpicker_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Colorpicker *cp;
   
   if (!(cp = ETK_COLORPICKER(widget)))
      return;
   
   /* First, update the data of the images if needed */
   if (cp->sp_image_needs_update)
      _etk_colorpicker_sp_image_update(cp);
   if (cp->sp_cursor_needs_update)
      _etk_colorpicker_sp_cursor_update(cp);
   if (cp->vp_image_needs_update)
      _etk_colorpicker_vp_image_update(cp);
   if (cp->vp_cursor_needs_update)
      _etk_colorpicker_vp_cursor_update(cp);
   if (cp->sliders_need_update)
      _etk_colorpicker_sliders_update(cp);
   
   /* Then, we move and resize the main table and we replace the cursors of the pickers */
   etk_widget_size_allocate(cp->main_table, geometry);
   _etk_colorpicker_sp_cursor_replace(cp);
   _etk_colorpicker_vp_cursor_replace(cp);
   
   cp->sp_image_needs_update = ETK_FALSE;
   cp->sp_cursor_needs_update = ETK_FALSE;
   cp->vp_image_needs_update = ETK_FALSE;
   cp->vp_cursor_needs_update = ETK_FALSE;
   cp->sliders_need_update = ETK_FALSE;
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the colorpicker is realized */
static void _etk_colorpicker_realize_cb(Etk_Object *object, void *data)
{
   Etk_Colorpicker *cp;
   Etk_Colorpicker_Picker_SD *picker_sd;
   Evas *evas;
   
   if (!(cp = ETK_COLORPICKER(data)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(cp))))
      return;
   
   /* Square picker objects */
   cp->sp_object = _etk_colorpicker_picker_object_add(evas, cp, _etk_colorpicker_sp_move_resize);
   etk_widget_swallow_object(cp->picker_widget, "square_picker", cp->sp_object);
   picker_sd = evas_object_smart_data_get(cp->sp_object);
   
   cp->sp_image = evas_object_image_add(evas);
   evas_object_image_alpha_set(cp->sp_image, 0);
   evas_object_image_size_set(cp->sp_image, cp->sp_res, cp->sp_res);
   picker_sd->objects = evas_list_append(picker_sd->objects, cp->sp_image);
   
   cp->sp_hcursor = evas_object_image_add(evas);
   evas_object_image_alpha_set(cp->sp_hcursor, 0);
   evas_object_image_size_set(cp->sp_hcursor, cp->sp_res, 1);
   evas_object_pass_events_set(cp->sp_hcursor, 1);
   picker_sd->objects = evas_list_append(picker_sd->objects, cp->sp_hcursor);
   
   cp->sp_vcursor = evas_object_image_add(evas);
   evas_object_image_alpha_set(cp->sp_vcursor, 0);
   evas_object_image_size_set(cp->sp_vcursor, 1, cp->sp_res);
   evas_object_pass_events_set(cp->sp_vcursor, 1);
   picker_sd->objects = evas_list_append(picker_sd->objects, cp->sp_vcursor);
   
   evas_object_show(cp->sp_object);
   
   /* Vertical picker objects */
   cp->vp_object = _etk_colorpicker_picker_object_add(evas, cp, _etk_colorpicker_vp_move_resize);
   etk_widget_swallow_object(cp->picker_widget, "vertical_picker", cp->vp_object);
   picker_sd = evas_object_smart_data_get(cp->vp_object);
   
   cp->vp_image = evas_object_image_add(evas);
   evas_object_image_alpha_set(cp->vp_image, 0);
   evas_object_image_size_set(cp->vp_image, 1, cp->vp_res);
   picker_sd->objects = evas_list_append(picker_sd->objects, cp->vp_image);
   
   cp->vp_cursor = evas_object_rectangle_add(evas);
   evas_object_pass_events_set(cp->vp_cursor, 1);
   picker_sd->objects = evas_list_append(picker_sd->objects, cp->vp_cursor);
   
   evas_object_show(cp->vp_object);
   
   /* Adds the mouse callbacks */
   evas_object_event_callback_add(cp->sp_image, EVAS_CALLBACK_MOUSE_DOWN, _etk_colorpicker_sp_mouse_down_cb, cp);
   evas_object_event_callback_add(cp->sp_image, EVAS_CALLBACK_MOUSE_UP, _etk_colorpicker_sp_mouse_up_cb, cp);
   evas_object_event_callback_add(cp->sp_image, EVAS_CALLBACK_MOUSE_MOVE, _etk_colorpicker_sp_mouse_move_cb, cp);
   evas_object_event_callback_add(cp->vp_image, EVAS_CALLBACK_MOUSE_DOWN, _etk_colorpicker_vp_mouse_down_cb, cp);
   evas_object_event_callback_add(cp->vp_image, EVAS_CALLBACK_MOUSE_UP, _etk_colorpicker_vp_mouse_up_cb, cp);
   evas_object_event_callback_add(cp->vp_image, EVAS_CALLBACK_MOUSE_MOVE, _etk_colorpicker_vp_mouse_move_cb, cp);
   
   /* Updates the colorpicker */
   _etk_colorpicker_update(cp, ETK_TRUE, ETK_TRUE, ETK_TRUE, ETK_TRUE);
}

/* Called when the colorpicker is unrealized */
static void _etk_colorpicker_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Colorpicker *cp;
   
   if (!(cp = ETK_COLORPICKER(data)))
      return;
   
   /* TODO: delete objects! */
   cp->sp_object = NULL;
   cp->sp_image = NULL;
   cp->sp_hcursor = NULL;
   cp->sp_vcursor = NULL;
   cp->vp_object = NULL;
   cp->vp_image = NULL;
   cp->vp_cursor = NULL;
}

/* Called when a slider of the colorpicker is realized */
static void _etk_colorpicker_slider_realize_cb(Etk_Object *object, void *data)
{
   Etk_Widget *slider;
   Etk_Colorpicker *cp;
   Evas *evas;
   int i;
   
   if (!(slider = ETK_WIDGET(object)) || !(cp = ETK_COLORPICKER(data)) || !(evas = etk_widget_toplevel_evas_get(slider)))
      return;
   
   for (i = 0; i < 6; i++)
   {
      if (cp->sliders[i] == slider)
      {
         cp->sliders_image[i] = evas_object_image_add(evas);
         evas_object_image_alpha_set(cp->sliders_image[i], 0);
         evas_object_image_size_set(cp->sliders_image[i], cp->sliders_res, 1);
         evas_object_show(cp->sliders_image[i]);
         evas_object_event_callback_add(cp->sliders_image[i], EVAS_CALLBACK_RESIZE,
            _etk_colorpicker_slider_image_resize_cb, NULL);
         etk_widget_swallow_object(slider, "slider_image", cp->sliders_image[i]);
         
         _etk_colorpicker_update(cp, ETK_FALSE, ETK_FALSE, ETK_FALSE, ETK_FALSE);
         return;
      }
   }
}

/* Called when a slider of the colorpicker is unrealized */
static void _etk_colorpicker_slider_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Widget *slider;
   Etk_Colorpicker *cp;
   int i;
   
   if (!(slider = ETK_WIDGET(object)) || !(cp = ETK_COLORPICKER(data)))
      return;
   
   /* TODO: del image */
   for (i = 0; i < 6; i++)
   {
      if (cp->sliders[i] == slider)
         cp->sliders_image[i] = NULL;
   }
}

/* Called when the current color widget of the colorpicker is realized */
static void _etk_colorpicker_current_color_realize_cb(Etk_Object *object, void *data)
{
   Etk_Colorpicker *cp;
   Evas *evas;
   
   if (!(cp = ETK_COLORPICKER(data)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(cp))))
      return;
   
   cp->current_color_rect = evas_object_rectangle_add(evas);
   etk_widget_swallow_object(cp->current_color_widget, "preview_rect", cp->current_color_rect);
   evas_object_show(cp->current_color_rect);
   
   evas_object_color_set(cp->current_color_rect, cp->current_color.r,
      cp->current_color.g, cp->current_color.b, 255);
}

/* Called when the current color widget of the colorpicker is unrealized */
static void _etk_colorpicker_current_color_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Colorpicker *cp;
   
   if (!(cp = ETK_COLORPICKER(data)))
      return;
   
   /* TODO: del object */
   cp->current_color_rect = NULL;
}

/* Called when the value of the slider is changed */
static void _etk_colorpicker_slider_value_changed_cb(Etk_Object *object, double value, void *data)
{
   Etk_Widget *slider;
   Etk_Colorpicker *cp;
   Etk_Color color;
   Etk_Bool update_sp_image;
   Etk_Bool update_vp_image;
   char string[10];
   float h, s, v;
   int i;
   
   if (!(slider = ETK_WIDGET(object)) || !(cp = ETK_COLORPICKER(data)))
      return;
   
   for (i = 0; i < 6; i++)
   {
      if (cp->sliders[i] == slider)
      {
         /* First, we update the position of the cursors */
         if (i == cp->mode)
            cp->vp_pos = value / cp->sliders_max_value[i];
         else if (i == cp->sp_xcomponent)
            cp->sp_xpos = value / cp->sliders_max_value[i];
         else if (i == cp->sp_ycomponent)
            cp->sp_ypos = value / cp->sliders_max_value[i];
         
         /* We update the corresponding label */
         if (i == ETK_COLORPICKER_S || i == ETK_COLORPICKER_V)
            snprintf(string, 10, "%d", (int)(value * 100));
         else
            snprintf(string, 10, "%d",(int)value);
         etk_label_set(ETK_LABEL(cp->value_labels[i]), string);
         
         if (cp->ignore_value_changed)
            return;
         
         /* Then, we update the values of the other sliders */
         switch (i)
         {
            case ETK_COLORPICKER_H:
               h = value;
               s = etk_range_value_get(ETK_RANGE(cp->sliders[ETK_COLORPICKER_S]));
               v = etk_range_value_get(ETK_RANGE(cp->sliders[ETK_COLORPICKER_V]));
               evas_color_hsv_to_rgb(h, s, v, &color.r, &color.g, &color.b);
               break;
            case ETK_COLORPICKER_S:
               h = etk_range_value_get(ETK_RANGE(cp->sliders[ETK_COLORPICKER_H]));
               s = value;
               v = etk_range_value_get(ETK_RANGE(cp->sliders[ETK_COLORPICKER_V]));
               evas_color_hsv_to_rgb(h, s, v, &color.r, &color.g, &color.b);
               break;
            case ETK_COLORPICKER_V:
               h = etk_range_value_get(ETK_RANGE(cp->sliders[ETK_COLORPICKER_H]));
               s = etk_range_value_get(ETK_RANGE(cp->sliders[ETK_COLORPICKER_S]));
               v = value;
               evas_color_hsv_to_rgb(h, s, v, &color.r, &color.g, &color.b);
               break;
            case ETK_COLORPICKER_R:
               color.r = (int)value;
               color.g = (int)etk_range_value_get(ETK_RANGE(cp->sliders[ETK_COLORPICKER_G]));
               color.b = (int)etk_range_value_get(ETK_RANGE(cp->sliders[ETK_COLORPICKER_B]));
               break;
            case ETK_COLORPICKER_G:
               color.r = (int)etk_range_value_get(ETK_RANGE(cp->sliders[ETK_COLORPICKER_R]));
               color.g = (int)value;
               color.b = (int)etk_range_value_get(ETK_RANGE(cp->sliders[ETK_COLORPICKER_B]));
               break;
            case ETK_COLORPICKER_B:
               color.r = (int)etk_range_value_get(ETK_RANGE(cp->sliders[ETK_COLORPICKER_R]));
               color.g = (int)etk_range_value_get(ETK_RANGE(cp->sliders[ETK_COLORPICKER_G]));
               color.b = (int)value;
               break;
            default:
               break;
         }
         
         if (i == ETK_COLORPICKER_H || i == ETK_COLORPICKER_S || i == ETK_COLORPICKER_V)
         {
            cp->ignore_value_changed = ETK_TRUE;
            etk_range_value_set(ETK_RANGE(cp->sliders[ETK_COLORPICKER_R]), color.r);
            etk_range_value_set(ETK_RANGE(cp->sliders[ETK_COLORPICKER_G]), color.g);
            etk_range_value_set(ETK_RANGE(cp->sliders[ETK_COLORPICKER_B]), color.b);
            cp->ignore_value_changed = ETK_FALSE;
         }
         else
         {
            cp->ignore_value_changed = ETK_TRUE;
            evas_color_rgb_to_hsv(color.r, color.g, color.b, &h, &s, &v);
            etk_range_value_set(ETK_RANGE(cp->sliders[ETK_COLORPICKER_H]), h);
            etk_range_value_set(ETK_RANGE(cp->sliders[ETK_COLORPICKER_S]), s);
            etk_range_value_set(ETK_RANGE(cp->sliders[ETK_COLORPICKER_V]), v);
            cp->ignore_value_changed = ETK_FALSE;
         }
         
         /* And then, we update the current color and the colorpicker */
         if (color.r != cp->current_color.r || color.g != cp->current_color.g || color.b != cp->current_color.b)
         {
            cp->current_color.r = color.r;
            cp->current_color.g = color.g;
            cp->current_color.b = color.b;
            evas_object_color_set(cp->current_color_rect, color.r, color.g, color.b, 255);
            if (cp->emit_signal)
               etk_signal_emit(_etk_colorpicker_signals[ETK_CP_COLOR_CHANGED_SIGNAL], ETK_OBJECT(cp), NULL);
         }
         
         update_sp_image = !((i != cp->mode) && ((i / 3) == (cp->mode / 3)));
         update_vp_image = !((i / 3) == (cp->mode / 3));
         _etk_colorpicker_update(cp, update_sp_image, ETK_TRUE, update_vp_image, ETK_TRUE);
         
         return;
      }
   }
}

/* Called when the color mode is changed with the radio buttons */
static void _etk_colorpicker_radio_toggled_cb(Etk_Object *object, void *data)
{
   Etk_Colorpicker *cp;
   Etk_Widget *radio;
   int i;
   
   if (!(radio = ETK_WIDGET(object)) || !(cp = ETK_COLORPICKER(data)))
      return;
   if (!etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(radio)))
      return;
   
   for (i = 0; i < 6; i++)
   {
      if (cp->radios[i] == radio)
      {
         etk_colorpicker_mode_set(cp, i);
         return;
      }
   }
}

/* Called when the square picker is pressed by the mouse */
static void _etk_colorpicker_sp_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Colorpicker *cp;
   Evas_Event_Mouse_Down *event;
   int x, y, w, h;
   float xpercent, ypercent;
   
   if (!(cp = ETK_COLORPICKER(data)) || !(event = event_info))
      return;
   
   evas_object_geometry_get(cp->sp_image, &x, &y, &w, &h);
   xpercent = ETK_CLAMP((float)(event->canvas.x - x) / w, 0.0, 1.0);
   ypercent = 1.0 - ETK_CLAMP((float)(event->canvas.y - y) / h, 0.0, 1.0);
   
   _etk_colorpicker_sp_cursor_move(cp, xpercent, ypercent);
   cp->sp_dragging = ETK_TRUE;
}

/* Called when the square picker is released by the mouse */
static void _etk_colorpicker_sp_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Colorpicker *cp;
   
   if (!(cp = ETK_COLORPICKER(data)))
      return;
   cp->sp_dragging = ETK_FALSE;
}

/* Called when the mouse is moved over the square picker */
static void _etk_colorpicker_sp_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Colorpicker *cp;
   Evas_Event_Mouse_Move *event;
   int x, y, w, h;
   float xpercent, ypercent;
   
   if (!(cp = ETK_COLORPICKER(data)) || !(event = event_info) || !cp->sp_dragging)
      return;
   
   evas_object_geometry_get(cp->sp_image, &x, &y, &w, &h);
   xpercent = ETK_CLAMP((float)(event->cur.canvas.x - x) / w, 0.0, 1.0);
   ypercent = 1.0 - ETK_CLAMP((float)(event->cur.canvas.y - y) / h, 0.0, 1.0);
   
   _etk_colorpicker_sp_cursor_move(cp, xpercent, ypercent);
}

/* Called when the vertical picker is pressed by the mouse */
static void _etk_colorpicker_vp_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Colorpicker *cp;
   Evas_Event_Mouse_Down *event;
   int y, h;
   float percent;
   
   if (!(cp = ETK_COLORPICKER(data)) || !(event = event_info))
      return;
   
   evas_object_geometry_get(cp->vp_image, NULL, &y, NULL, &h);
   percent = 1.0 - ETK_CLAMP((float)(event->canvas.y - y) / h, 0.0, 1.0);
   
   etk_range_value_set(ETK_RANGE(cp->sliders[cp->mode]), percent * cp->sliders_max_value[cp->mode]);
   cp->vp_dragging = ETK_TRUE;
}

/* Called when the vertical picker is released by the mouse */
static void _etk_colorpicker_vp_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Colorpicker *cp;
   
   if (!(cp = ETK_COLORPICKER(data)))
      return;
   cp->vp_dragging = ETK_FALSE;
}

/* Called when the mouse is moved over the vertical picker */
static void _etk_colorpicker_vp_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Colorpicker *cp;
   Evas_Event_Mouse_Move *event;
   int y, h;
   float percent;
   
   if (!(cp = ETK_COLORPICKER(data)) || !(event = event_info) || !cp->vp_dragging)
      return;
   
   evas_object_geometry_get(cp->vp_image, NULL, &y, NULL, &h);
   percent = 1.0 - ETK_CLAMP((float)(event->cur.canvas.y - y) / h, 0.0, 1.0);
   
   etk_range_value_set(ETK_RANGE(cp->sliders[cp->mode]), percent * cp->sliders_max_value[cp->mode]);
}

/* Called when the image of a slider of the colorpicker is resized */
static void _etk_colorpicker_slider_image_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Coord w, h;
   
   if (!obj)
      return;
   
   evas_object_geometry_get(obj, NULL, NULL, &w, &h);
   evas_object_image_fill_set(obj, 0, 0, w, h);
}

/**************************
 *
 * Colorpicker's smart object
 *
 **************************/

/* Creates a new picker object */
static Evas_Object *_etk_colorpicker_picker_object_add(Evas *evas, Etk_Colorpicker *cp, void (*move_resize)(Etk_Colorpicker *cp, int x, int y, int w, int h))
{
   Evas_Object *obj;
   Etk_Colorpicker_Picker_SD *picker_sd;
   
   if (!evas || !cp)
      return NULL;
   
   if (!_etk_colorpicker_picker_smart)
   {
      _etk_colorpicker_picker_smart = evas_smart_new("Picker_Object",
         _etk_colorpicker_picker_smart_add,
         _etk_colorpicker_picker_smart_del,
         NULL,
         NULL,
         NULL,
         NULL,
         NULL,
         _etk_colorpicker_picker_smart_move,
         _etk_colorpicker_picker_smart_resize,
         _etk_colorpicker_picker_smart_show,
         _etk_colorpicker_picker_smart_hide,
         NULL,
         _etk_colorpicker_picker_smart_clip_set,
         _etk_colorpicker_picker_smart_clip_unset,
         NULL);
   }
   /* TODO: _etk_colorpicker_picker_smart_use */
   _etk_colorpicker_picker_smart_use++;
   
   obj = evas_object_smart_add(evas, _etk_colorpicker_picker_smart);
   picker_sd = evas_object_smart_data_get(obj);
   picker_sd->cp = cp;
   picker_sd->move_resize = move_resize;
   
   return obj;
}

/* Initializes the new picker object */
static void _etk_colorpicker_picker_smart_add(Evas_Object *obj)
{
   Etk_Colorpicker_Picker_SD *picker_sd;
   Evas *evas;
   
   if (!obj || !(evas = evas_object_evas_get(obj)))
      return;
   
   picker_sd = malloc(sizeof(Etk_Colorpicker_Picker_SD));
   picker_sd->objects = NULL;
   picker_sd->cp = NULL;
   picker_sd->move_resize = NULL;
   evas_object_smart_data_set(obj, picker_sd);
}

/* Destroys the picker object */
static void _etk_colorpicker_picker_smart_del(Evas_Object *obj)
{
   Etk_Colorpicker_Picker_SD *picker_sd;
   
   if (!obj || !(picker_sd = evas_object_smart_data_get(obj)))
      return;

   while (picker_sd->objects)
   {
      evas_object_del(picker_sd->objects->data);
      picker_sd->objects = evas_list_remove_list(picker_sd->objects, picker_sd->objects);
   }
   free(picker_sd);
}

/* Moves the picker object */
static void _etk_colorpicker_picker_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   Etk_Colorpicker_Picker_SD *picker_sd;
   Evas_Coord w, h;
   
   if (!obj || !(picker_sd = evas_object_smart_data_get(obj)))
      return;

   evas_object_geometry_get(obj, NULL, NULL, &w, &h);
   if (picker_sd->cp && picker_sd->move_resize)
      picker_sd->move_resize(picker_sd->cp, x, y, w, h);
}

/* Resizes the picker object */
static void _etk_colorpicker_picker_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   Etk_Colorpicker_Picker_SD *picker_sd;
   Evas_Coord x, y;
   
   if (!obj || !(picker_sd = evas_object_smart_data_get(obj)))
      return;

   evas_object_geometry_get(obj, &x, &y, NULL, NULL);
   if (picker_sd->cp && picker_sd->move_resize)
      picker_sd->move_resize(picker_sd->cp, x, y, w, h);
}

/* Shows the picker object */
static void _etk_colorpicker_picker_smart_show(Evas_Object *obj)
{
   Etk_Colorpicker_Picker_SD *picker_sd;
   Evas_List *l;
   
   if (!obj || !(picker_sd = evas_object_smart_data_get(obj)))
      return;

   for (l = picker_sd->objects; l; l = l->next)
      evas_object_show(l->data);
}

/* Hides the picker object */
static void _etk_colorpicker_picker_smart_hide(Evas_Object *obj)
{
   Etk_Colorpicker_Picker_SD *picker_sd;
   Evas_List *l;
   
   if (!obj || !(picker_sd = evas_object_smart_data_get(obj)))
      return;

   for (l = picker_sd->objects; l; l = l->next)
      evas_object_hide(l->data);
}

/* Sets the clip of the picker object */
static void _etk_colorpicker_picker_smart_clip_set(Evas_Object *obj, Evas_Object *clip)
{
   Etk_Colorpicker_Picker_SD *picker_sd;
   Evas_List *l;
   
   if (!obj || !(picker_sd = evas_object_smart_data_get(obj)))
      return;

   for (l = picker_sd->objects; l; l = l->next)
      evas_object_clip_set(l->data, clip);
}

/* Unsets the clip of the picker object */
static void _etk_colorpicker_picker_smart_clip_unset(Evas_Object *obj)
{
   Etk_Colorpicker_Picker_SD *picker_sd;
   Evas_List *l;
   
   if (!obj || !(picker_sd = evas_object_smart_data_get(obj)))
      return;

   for (l = picker_sd->objects; l; l = l->next)
      evas_object_clip_unset(l->data);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Moves and resizes the square picker */
static void _etk_colorpicker_sp_move_resize(Etk_Colorpicker *cp, int x, int y, int w, int h)
{
   if (!cp || !cp->sp_image)
      return;
   
   evas_object_move(cp->sp_image, x, y);
   evas_object_resize(cp->sp_image, w, h);
   evas_object_image_fill_set(cp->sp_image, 0, 0, w, h);
   
   _etk_colorpicker_sp_cursor_replace(cp);
}

/* Moves and resizes the vertical picker */
static void _etk_colorpicker_vp_move_resize(Etk_Colorpicker *cp, int x, int y, int w, int h)
{
   if (!cp || !cp->vp_image)
      return;
   
   evas_object_move(cp->vp_image, x, y);
   evas_object_resize(cp->vp_image, w, h);
   evas_object_image_fill_set(cp->vp_image, 0, 0, w, h);
   
   _etk_colorpicker_vp_cursor_replace(cp);
}

/* Updates the position and the size of the cursors of the square picker */
static void _etk_colorpicker_sp_cursor_replace(Etk_Colorpicker *cp)
{
   int x, y, w, h;
   
   if (!cp || !cp->sp_image || !cp->sp_hcursor || !cp->sp_vcursor)
      return;
   
   evas_object_geometry_get(cp->sp_image, &x, &y, &w, &h);
   
   evas_object_move(cp->sp_hcursor, x, y + ((1.0 - cp->sp_ypos) * (h - 1)));
   evas_object_resize(cp->sp_hcursor, w, 1);
   evas_object_image_fill_set(cp->sp_hcursor, 0, 0, w, 1);
   
   evas_object_move(cp->sp_vcursor, x + (cp->sp_xpos * (w - 1)), y);
   evas_object_resize(cp->sp_vcursor, 1, h);
   evas_object_image_fill_set(cp->sp_vcursor, 0, 0, 1, h);
}

/* Updates the position and the size of the cursor of the vertical picker */
static void _etk_colorpicker_vp_cursor_replace(Etk_Colorpicker *cp)
{
   int x, y, w, h;
   
   if (!cp || !cp->vp_image || !cp->vp_cursor)
      return;
   
   evas_object_geometry_get(cp->vp_image, &x, &y, &w, &h);
   evas_object_move(cp->vp_cursor, x, y + ((1.0 - cp->vp_pos) * (h - 1)));
   evas_object_resize(cp->vp_cursor, w, 1);
}

/* Changes the values of the corresponding sliders: it will move the cursor of the square picker */
static void _etk_colorpicker_sp_cursor_move(Etk_Colorpicker *cp, float xpercent, float ypercent)
{
   if (!cp)
      return;
   
   cp->emit_signal = ETK_FALSE;
   etk_range_value_set(ETK_RANGE(cp->sliders[cp->sp_xcomponent]), xpercent * cp->sliders_max_value[cp->sp_xcomponent]);
   cp->emit_signal = ETK_TRUE;
   etk_range_value_set(ETK_RANGE(cp->sliders[cp->sp_ycomponent]), ypercent * cp->sliders_max_value[cp->sp_ycomponent]);
}

/* Updates of the colorpicker */ 
static void _etk_colorpicker_update(Etk_Colorpicker *cp, Etk_Bool sp_image, Etk_Bool sp_cursor, Etk_Bool vp_image, Etk_Bool vp_cursor)
{
   if (!cp)
      return;
   
   cp->sp_image_needs_update |= sp_image;
   cp->sp_cursor_needs_update |= sp_cursor;
   cp->vp_image_needs_update |= vp_image;
   cp->vp_cursor_needs_update |= vp_cursor;
   cp->sliders_need_update = ETK_TRUE;
   
   etk_widget_redraw_queue(ETK_WIDGET(cp));
}

/* Updates the square picker image */
static void _etk_colorpicker_sp_image_update(Etk_Colorpicker *cp)
{
   uint32_t *data;
   int i, j;
   int r, g, b;
   
   if (!cp || !cp->sp_image)
      return;
   if (!(data = (uint32_t *)evas_object_image_data_get(cp->sp_image, 1)))
      return;
   
   for (j = 0; j < cp->sp_res; j++)
   {
      for (i = 0; i < cp->sp_res; i++)
      {
         _etk_colorpicker_sp_color_get(cp, i, j, &r, &g, &b);
         *data = ((r << 16) | (g << 8) | b);
         data++;
      }
   }
   
   evas_object_image_data_update_add(cp->sp_image, 0, 0, cp->sp_res, cp->sp_res);
}

/* Updates the cursor of the square picker */
static void _etk_colorpicker_sp_cursor_update(Etk_Colorpicker *cp)
{
   uint32_t *data;
   int i, j;
   int r, g, b;
   
   if (!cp)
      return;
   
   /* Updates the horizontal cursor */
   if (cp->sp_hcursor && (data = (uint32_t *)evas_object_image_data_get(cp->sp_hcursor, 1)))
   {
      j = cp->sp_res * (1.0 - cp->sp_ypos);
      for (i = 0; i < cp->sp_res; i++)
      {
         _etk_colorpicker_sp_color_get(cp, i, j, &r, &g, &b);
         *data = (((255 - r) << 16) | ((255 - g) << 8) | (255 - b));
         data++;
      }
      evas_object_image_data_update_add(cp->sp_hcursor, 0, 0, cp->sp_res, 1);
   }
   
   /* Updates the vertical cursor */
   if (cp->sp_vcursor && (data = (uint32_t *)evas_object_image_data_get(cp->sp_vcursor, 1)))
   {
      i = cp->sp_res * cp->sp_xpos;
      for (j = 0; j < cp->sp_res; j++)
      {
         _etk_colorpicker_sp_color_get(cp, i, j, &r, &g, &b);
         *data = (((255 - r) << 16) | ((255 - g) << 8) | (255 - b));
         data++;
      }
      evas_object_image_data_update_add(cp->sp_vcursor, 0, 0, 1, cp->sp_res);
   }
}

/* Updates the vertical picker image */
static void _etk_colorpicker_vp_image_update(Etk_Colorpicker *cp)
{
   uint32_t *data;
   int i;
   int r, g, b;
   
   if (!cp || !cp->vp_image)
      return;
   if (!(data = (uint32_t *)evas_object_image_data_get(cp->vp_image, 1)))
      return;
   
   for (i = 0; i < cp->vp_res; i++)
   {
      _etk_colorpicker_vp_color_get(cp, i, &r, &g, &b);
      *data = ((r << 16) | (g << 8) | b);
      data++;
   }
   
   evas_object_image_data_update_add(cp->vp_image, 0, 0, 1, cp->vp_res);
}

/* Updates the vertical picker cursor */
static void _etk_colorpicker_vp_cursor_update(Etk_Colorpicker *cp)
{
   int r, g, b;
   
   if (!cp || !cp->vp_cursor)
      return;
   
   _etk_colorpicker_vp_color_get(cp, cp->vp_res * (1.0 - cp->vp_pos), &r, &g, &b);
   evas_object_color_set(cp->vp_cursor, 255 - r, 255 - g, 255 - b, 255);
}

/* Updates the images of the sliders */
static void _etk_colorpicker_sliders_update(Etk_Colorpicker *cp)
{
   int i, j;
   int r, g, b;
   float h, s, v;
   int r2, g2, b2;
   uint32_t *data;
   
   r = cp->current_color.r;
   g = cp->current_color.g;
   b = cp->current_color.b;
   h = etk_range_value_get(ETK_RANGE(cp->sliders[ETK_COLORPICKER_H]));
   s = etk_range_value_get(ETK_RANGE(cp->sliders[ETK_COLORPICKER_S]));
   v = etk_range_value_get(ETK_RANGE(cp->sliders[ETK_COLORPICKER_V]));
   
   for (i = 0; i < 6; i++)
   {
      if (cp->sliders_image[i] && (data = (uint32_t *)evas_object_image_data_get(cp->sliders_image[i], 1)))
      {
         for (j = 0; j < cp->sliders_res; j++)
         {
            switch (i)
            {
               case ETK_COLORPICKER_H:
                  evas_color_hsv_to_rgb(360.0 * ((float)j / cp->sliders_res), s, v, &r2, &g2, &b2);
                  break;
               case ETK_COLORPICKER_S:
                  evas_color_hsv_to_rgb(h, (float)j / cp->sliders_res, v, &r2, &g2, &b2);
                  break;
               case ETK_COLORPICKER_V:
                  evas_color_hsv_to_rgb(h, s, (float)j / cp->sliders_res, &r2, &g2, &b2);
                  break;
               case ETK_COLORPICKER_R:
                  r2 = (255 * j) / cp->sliders_res;
                  g2 = g;
                  b2 = b;
                  break;
               case ETK_COLORPICKER_G:
                  r2 = r;
                  g2 = (255 * j) / cp->sliders_res;
                  b2 = b;
                  break;
               case ETK_COLORPICKER_B:
                  r2 = r;
                  g2 = g;
                  b2 = (255 * j) / cp->sliders_res;
                  break;
               default:
                  break;
            }
            *data = ((r2 << 16) | (g2 << 8) | b2);
            data++;
         }
         evas_object_image_data_update_add(cp->sliders_image[i], 0, 0, cp->sliders_res, 1);
      }
   }
}

/* Get the color of the square picker's image, at the point (i, j). (r, g, b) must not be NULL! */
static void _etk_colorpicker_sp_color_get(Etk_Colorpicker *cp, int i, int j, int *r, int *g, int *b)
{
   _etk_colorpicker_color_calc(cp->mode, (float)i / cp->sp_res, 1.0 - ((float)j / cp->sp_res), cp->vp_pos, r, g, b);
}

/* Get the color of the vertical picker's image, at the point i. (r, g, b) must not be NULL!  */
static void _etk_colorpicker_vp_color_get(Etk_Colorpicker *cp, int i, int *r, int *g, int *b)
{
   switch (cp->mode)
   {
      case ETK_COLORPICKER_H:
         evas_color_hsv_to_rgb(360.0 * (1.0 - ((float)i / cp->vp_res)), 1.0, 1.0, r, g, b);
         break;
      case ETK_COLORPICKER_S:
         *r = 255 - ((i * 255) / cp->vp_res);
         *g = 255 - ((i * 255) / cp->vp_res);
         *b = 255 - ((i * 255) / cp->vp_res);
         break;
      case ETK_COLORPICKER_V:
         *r = 255 - ((i * 255) / cp->vp_res);
         *g = 255 - ((i * 255) / cp->vp_res);
         *b = 255 - ((i * 255) / cp->vp_res);
         break;
      case ETK_COLORPICKER_R:
         *r = 255 - ((i * 255) / cp->vp_res);
         *g = 0;
         *b = 0;
         break;
      case ETK_COLORPICKER_G:
         *r = 0;
         *g = 255 - ((i * 255) / cp->vp_res);
         *b = 0;
         break;
      case ETK_COLORPICKER_B:
         *r = 0;
         *g = 0;
         *b = 255 - ((i * 255) / cp->vp_res);
         break;
      default:
         break;
   }
}

/* Calculates a color according to the color mode and the positions of the cursors. (r, g, b) must not be NULL! */
static void _etk_colorpicker_color_calc(Etk_Colorpicker_Mode mode, float sp_xpos, float sp_ypos, float vp_pos, int *r, int *g, int *b)
{
   switch (mode)
   {
      case ETK_COLORPICKER_H:
         evas_color_hsv_to_rgb(vp_pos * 360.0, sp_ypos, sp_xpos, r, g, b);
         break;
      case ETK_COLORPICKER_S:
         evas_color_hsv_to_rgb(sp_ypos * 360.0, vp_pos, sp_xpos, r, g, b);
         break;
      case ETK_COLORPICKER_V:
         evas_color_hsv_to_rgb(sp_ypos * 360.0, sp_xpos, vp_pos, r, g, b);
         break;
      case ETK_COLORPICKER_R:
         *r = 255 * vp_pos;
         *g = 255 * sp_ypos;
         *b = 255 * sp_xpos;
         break;
      case ETK_COLORPICKER_G:
         *r = 255 * sp_ypos;
         *g = 255 * vp_pos;
         *b = 255 * sp_xpos;
         break;
      case ETK_COLORPICKER_B:
         *r = 255 * sp_ypos;
         *g = 255 * sp_xpos;
         *b = 255 * vp_pos;
         break;
      default:
         break;
   }
}

/** @} */
