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

typedef struct Etk_Colorpicker_Picker_Data
{
   Evas_List *objects;
   Etk_Colorpicker *cp;
   void (*move_resize)(Etk_Colorpicker *cp, int x, int y, int w, int h);
} Etk_Colorpicker_Picker_Data;

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
static void _etk_colorpicker_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_colorpicker_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_colorpicker_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_colorpicker_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

static void _etk_colorpicker_realize_cb(Etk_Object *object, void *data);
static void _etk_colorpicker_unrealize_cb(Etk_Object *object, void *data);
static void _etk_colorpicker_slider_realize_cb(Etk_Object *object, void *data);
static void _etk_colorpicker_slider_unrealize_cb(Etk_Object *object, void *data);
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
static void _etk_colorpicker_sp_cursor_move_resize(Etk_Colorpicker *cp);
static void _etk_colorpicker_vp_cursor_move_resize(Etk_Colorpicker *cp);

static void _etk_colorpicker_update(Etk_Colorpicker *cp, Etk_Bool sp_image, Etk_Bool sp_cursor, Etk_Bool vp_image, Etk_Bool vp_cursor);
static void _etk_colorpicker_sp_image_update(Etk_Colorpicker *cp);
static void _etk_colorpicker_sp_cursor_update(Etk_Colorpicker *cp);
static void _etk_colorpicker_vp_image_update(Etk_Colorpicker *cp);
static void _etk_colorpicker_vp_cursor_update(Etk_Colorpicker *cp);
static void _etk_colorpicker_sliders_update(Etk_Colorpicker *cp);
static void _etk_colorpicker_sp_color_get(Etk_Colorpicker *cp, int i, int j, int *r, int *g, int *b);
static void _etk_colorpicker_vp_color_get(Etk_Colorpicker *cp, int i, int *r, int *g, int *b);
static void _etk_colorpicker_color_calc(Etk_Colorpicker_Mode mode, float sp_xpos, float sp_ypos, float vp_pos, int *r, int *g, int *b);

static void _etk_colorpicker_hsv_to_rgb(float h, float s, float v, int *r, int *g, int *b);
static void _etk_colorpicker_rgb_to_hsv(int r, int g, int b, float *h, float *s, float *v);

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
         ETK_CONSTRUCTOR(_etk_colorpicker_constructor), NULL);
   
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
   if (!cp || (cp->mode == mode))
      return;
   
   cp->mode = mode;
   etk_colorpicker_current_color_set(cp, cp->current_color);
   
   etk_object_notify(ETK_OBJECT(cp), "mode");
}

/**
 * @brief Gets the current color mode of colorpicker
 * @param cp a colorpicker
 * @return Returns the current color mode of colorpicker
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
   int r, g, b;
   float h, s, v;
   
   if (cp->current_color.r == color.r && cp->current_color.g == color.g && cp->current_color.b == color.b)
      return;
   
   r = color.r;
   g = color.g;
   b = color.b;
   _etk_colorpicker_rgb_to_hsv(r, g, b, &h, &s, &v);
   
   switch (cp->mode)
   {
      case ETK_COLORPICKER_H:
         cp->sp_xpos = v;
         cp->sp_ypos = s;
         cp->vp_pos = h / 360.0;
         break;
      case ETK_COLORPICKER_S:
         cp->sp_xpos = v;
         cp->sp_ypos = h / 360.0;
         cp->vp_pos = s;
         break;
      case ETK_COLORPICKER_V:
         cp->sp_xpos = s;
         cp->sp_ypos = h / 360.0;
         cp->vp_pos = v;
         break;
      case ETK_COLORPICKER_R:
         cp->sp_xpos = b / 255.0;
         cp->sp_ypos = g / 255.0;
         cp->vp_pos = r / 255.0;
         break;
      case ETK_COLORPICKER_G:
         cp->sp_xpos = b / 255.0;
         cp->sp_ypos = r / 255.0;
         cp->vp_pos = g / 255.0;
         break;
      case ETK_COLORPICKER_B:
         cp->sp_xpos = g / 255.0;
         cp->sp_ypos = r / 255.0;
         cp->vp_pos = b / 255.0;
         break;
      default:
         break;
   }
   
   _etk_colorpicker_update(cp, ETK_TRUE, ETK_TRUE, ETK_TRUE, ETK_TRUE);
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
   char *labels[6] = {"H", "S", "V", "R", "G", "B"};
   int i;

   if (!(cp_widget = ETK_WIDGET(cp)))
      return;
   
   cp->mode = ETK_COLORPICKER_H;
   cp->current_color.r = 0;
   cp->current_color.g = 0;
   cp->current_color.b = 0;
   cp->current_color.a = 255;
   
   cp->picker_theme_object = NULL;
   
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
   
   cp->sliders_res = 256;
   cp->sp_dragging = ETK_FALSE;
   cp->vp_dragging = ETK_FALSE;
   cp->sp_image_needs_update = ETK_FALSE;
   cp->sp_cursor_needs_update = ETK_FALSE;
   cp->vp_image_needs_update = ETK_FALSE;
   cp->vp_cursor_needs_update = ETK_FALSE;
   cp->sliders_need_update = ETK_FALSE;
   
   cp->table = etk_table_new(2, 6, ETK_FALSE);
   etk_widget_parent_set(cp->table, cp_widget);
   etk_widget_visibility_locked_set(cp->table, ETK_TRUE);
   etk_widget_show(cp->table);
   
   for (i = 0; i < 6; i++)
   {
      if (i == 0)
         cp->radios[i] = etk_radio_button_new_with_label(labels[i], NULL);
      else
         cp->radios[i] = etk_radio_button_new_with_label_from_widget(labels[i], ETK_RADIO_BUTTON(cp->radios[0]));
      etk_table_attach(ETK_TABLE(cp->table), cp->radios[i], 0, 0, i, i,
         0, 0, ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VEXPAND);
      etk_widget_visibility_locked_set(cp->radios[i], ETK_TRUE);
      etk_widget_show(cp->radios[i]);
      
      switch (i)
      {
         case ETK_COLORPICKER_H:
            cp->sliders[i] = etk_hslider_new(0.0, 360.0, 0.0, 1.0, 5.0);
            break;
         case ETK_COLORPICKER_S:
         case ETK_COLORPICKER_V:
            cp->sliders[i] = etk_hslider_new(0.0, 1.0, 0.0, 0.01, 0.05);
            break;
         case ETK_COLORPICKER_R:
         case ETK_COLORPICKER_G:
         case ETK_COLORPICKER_B:
            cp->sliders[i] = etk_hslider_new(0.0, 255.0, 0.0, 1.0, 5.0);
            break;
         default:
            break;
      }
      etk_table_attach(ETK_TABLE(cp->table), cp->sliders[i], 1, 1, i, i,
         0, 0, ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_HEXPAND | ETK_FILL_POLICY_VEXPAND);
      etk_widget_visibility_locked_set(cp->sliders[i], ETK_TRUE);
      etk_widget_show(cp->sliders[i]);
      cp->sliders_image[i] = NULL;
      
      etk_signal_connect("toggled", ETK_OBJECT(cp->radios[i]),
         ETK_CALLBACK(_etk_colorpicker_radio_toggled_cb), cp);
      etk_signal_connect("realize", ETK_OBJECT(cp->sliders[i]),
         ETK_CALLBACK(_etk_colorpicker_slider_realize_cb), cp);
      etk_signal_connect("unrealize", ETK_OBJECT(cp->sliders[i]),
         ETK_CALLBACK(_etk_colorpicker_slider_unrealize_cb), cp);
      etk_signal_connect("value_changed", ETK_OBJECT(cp->sliders[i]),
         ETK_CALLBACK(_etk_colorpicker_slider_value_changed_cb), cp);
   }
   
   cp_widget->size_request = _etk_colorpicker_size_request;
   cp_widget->size_allocate = _etk_colorpicker_size_allocate;
   
   etk_signal_connect("realize", ETK_OBJECT(cp), ETK_CALLBACK(_etk_colorpicker_realize_cb), NULL);
   etk_signal_connect("unrealize", ETK_OBJECT(cp), ETK_CALLBACK(_etk_colorpicker_unrealize_cb), NULL);
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
/* TODO: size_request */
static void _etk_colorpicker_size_request(Etk_Widget *widget, Etk_Size *size)
{
   if (!size)
      return;
   
   size->w = 480;
   size->h = 200;
}

/* Resizes the colorpicker to the allocated size */
static void _etk_colorpicker_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Colorpicker *cp;
   Etk_Geometry child_geometry;
   
   if (!(cp = ETK_COLORPICKER(widget)))
      return;
   
   /* First, updates the data of the images if needed */
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
   
   /* Then, moves and resizes the objects */
   evas_object_move(cp->picker_theme_object, geometry.x, geometry.y);
   evas_object_resize(cp->picker_theme_object, (geometry.w / 2) - 5, geometry.h);
   _etk_colorpicker_sp_cursor_move_resize(cp);
   _etk_colorpicker_vp_cursor_move_resize(cp);
   
   child_geometry.x = geometry.x + (geometry.w / 2);
   child_geometry.y = geometry.y;
   child_geometry.w = geometry.w / 2;
   child_geometry.h = geometry.h;
   etk_widget_size_allocate(cp->table, child_geometry);
   
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
   Etk_Colorpicker_Picker_Data *picker_data;
   Evas *evas;
   
   if (!(cp = ETK_COLORPICKER(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(cp))))
      return;
   
   /* Picker theme object */
   cp->picker_theme_object = etk_theme_object_load_from_parent(evas, ETK_WIDGET(cp), NULL, "picker");
   evas_object_show(cp->picker_theme_object);
   etk_widget_member_object_add(ETK_WIDGET(cp), cp->picker_theme_object);
   
   /* Square picker objects */
   cp->sp_object = _etk_colorpicker_picker_object_add(evas, cp, _etk_colorpicker_sp_move_resize);
   edje_object_part_swallow(cp->picker_theme_object, "square_picker", cp->sp_object);
   picker_data = evas_object_smart_data_get(cp->sp_object);
   
   cp->sp_image = evas_object_image_add(evas);
   evas_object_image_alpha_set(cp->sp_image, 0);
   evas_object_image_size_set(cp->sp_image, cp->sp_res, cp->sp_res);
   picker_data->objects = evas_list_append(picker_data->objects, cp->sp_image);
   
   cp->sp_hcursor = evas_object_image_add(evas);
   evas_object_image_alpha_set(cp->sp_hcursor, 0);
   evas_object_image_size_set(cp->sp_hcursor, cp->sp_res, 1);
   evas_object_pass_events_set(cp->sp_hcursor, 1);
   picker_data->objects = evas_list_append(picker_data->objects, cp->sp_hcursor);
   
   cp->sp_vcursor = evas_object_image_add(evas);
   evas_object_image_alpha_set(cp->sp_vcursor, 0);
   evas_object_image_size_set(cp->sp_vcursor, 1, cp->sp_res);
   evas_object_pass_events_set(cp->sp_vcursor, 1);
   picker_data->objects = evas_list_append(picker_data->objects, cp->sp_vcursor);
   
   evas_object_show(cp->sp_object);
   
   /* Vertical picker objects */
   cp->vp_object = _etk_colorpicker_picker_object_add(evas, cp, _etk_colorpicker_vp_move_resize);
   edje_object_part_swallow(cp->picker_theme_object, "vertical_picker", cp->vp_object);
   picker_data = evas_object_smart_data_get(cp->vp_object);
   
   cp->vp_image = evas_object_image_add(evas);
   evas_object_image_alpha_set(cp->vp_image, 0);
   evas_object_image_size_set(cp->vp_image, 1, cp->vp_res);
   picker_data->objects = evas_list_append(picker_data->objects, cp->vp_image);
   
   cp->vp_cursor = evas_object_rectangle_add(evas);
   evas_object_pass_events_set(cp->vp_cursor, 1);
   picker_data->objects = evas_list_append(picker_data->objects, cp->vp_cursor);
   
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
   
   if (!(cp = ETK_COLORPICKER(object)))
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
         etk_widget_member_object_add(slider, cp->sliders_image[i]);
         etk_widget_theme_object_swallow(slider, "slider_image", cp->sliders_image[i]);
         
         _etk_colorpicker_update(cp, ETK_FALSE, ETK_FALSE, ETK_FALSE, ETK_FALSE);
         return;
      }
   }
}

/* Called when a slider of the colorpicker is unrealized */
static void _etk_colorpicker_slider_unrealize_cb(Etk_Object *object, void *data)
{
   /* TODO: del image */
   Etk_Widget *slider;
   Etk_Colorpicker *cp;
   int i;
   
   if (!(slider = ETK_WIDGET(object)) || !(cp = ETK_COLORPICKER(data)))
      return;
   
   for (i = 0; i < 6; i++)
   {
      if (cp->sliders[i] == slider)
         cp->sliders_image[i] = NULL;
   }
}

/* Called when the value of the slider is changed */
static void _etk_colorpicker_slider_value_changed_cb(Etk_Object *object, double value, void *data)
{
   Etk_Widget *slider;
   Etk_Colorpicker *cp;
   Etk_Color color;
   int r, g, b;
   float h, s, v;
   int i;
   
   if (!(slider = ETK_WIDGET(object)) || !(cp = ETK_COLORPICKER(data)))
      return;
   if (cp->sp_dragging || cp->vp_dragging)
      return;
   
   r = cp->current_color.r;
   g = cp->current_color.g;
   b = cp->current_color.b;
   _etk_colorpicker_rgb_to_hsv(r, g, b, &h, &s, &v);
   
   for (i = 0; i < 6; i++)
   {
      if (cp->sliders[i] == slider)
      {
         switch (i)
         {
            case ETK_COLORPICKER_H:
               _etk_colorpicker_hsv_to_rgb(value, s, v, &color.r, &color.g, &color.b);
               break;
            case ETK_COLORPICKER_S:
               _etk_colorpicker_hsv_to_rgb(h, value, v, &color.r, &color.g, &color.b);
               break;
            case ETK_COLORPICKER_V:
               _etk_colorpicker_hsv_to_rgb(h, s, value, &color.r, &color.g, &color.b);
               break;
            case ETK_COLORPICKER_R:
               color.r = (int)value;
               color.g = g;
               color.b = b;
               break;
            case ETK_COLORPICKER_G:
               color.r = r;
               color.g = (int)value;
               color.b = b;
               break;
            case ETK_COLORPICKER_B:
               color.r = r;
               color.g = g;
               color.b = (int)value;
               break;
            default:
               break;
         }
         etk_colorpicker_current_color_set(cp, color);
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
   
   if (!(cp = ETK_COLORPICKER(data)) || !(event = event_info))
      return;
   
   evas_object_geometry_get(cp->sp_image, &x, &y, &w, &h);
   cp->sp_xpos = ETK_CLAMP((float)(event->canvas.x - x) / w, 0.0, 1.0);
   cp->sp_ypos = 1.0 - ETK_CLAMP((float)(event->canvas.y - y) / h, 0.0, 1.0);
   _etk_colorpicker_update(cp, ETK_FALSE, ETK_TRUE, ETK_FALSE, ETK_FALSE);
   
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
   
   if (!(cp = ETK_COLORPICKER(data)) || !(event = event_info) || !cp->sp_dragging)
      return;
   
   evas_object_geometry_get(cp->sp_image, &x, &y, &w, &h);
   cp->sp_xpos = ETK_CLAMP((float)(event->cur.canvas.x - x) / w, 0.0, 1.0);
   cp->sp_ypos = 1.0 - ETK_CLAMP((float)(event->cur.canvas.y - y) / h, 0.0, 1.0);
   _etk_colorpicker_update(cp, ETK_FALSE, ETK_TRUE, ETK_FALSE, ETK_FALSE);
}

/* Called when the vertical picker is pressed by the mouse */
static void _etk_colorpicker_vp_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Colorpicker *cp;
   Evas_Event_Mouse_Down *event;
   int y, h;
   
   if (!(cp = ETK_COLORPICKER(data)) || !(event = event_info))
      return;
   
   evas_object_geometry_get(cp->vp_image, NULL, &y, NULL, &h);
   cp->vp_pos = 1.0 - ETK_CLAMP((float)(event->canvas.y - y) / h, 0.0, 1.0);
   _etk_colorpicker_update(cp, ETK_TRUE, ETK_TRUE, ETK_TRUE, ETK_TRUE);
   
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
   
   if (!(cp = ETK_COLORPICKER(data)) || !(event = event_info) || !cp->vp_dragging)
      return;
   
   evas_object_geometry_get(cp->vp_image, NULL, &y, NULL, &h);
   cp->vp_pos = 1.0 - ETK_CLAMP((float)(event->cur.canvas.y - y) / h, 0.0, 1.0);
   _etk_colorpicker_update(cp, ETK_TRUE, ETK_TRUE, ETK_TRUE, ETK_TRUE);
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
   Etk_Colorpicker_Picker_Data *picker_data;
   
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
   picker_data = evas_object_smart_data_get(obj);
   picker_data->cp = cp;
   picker_data->move_resize = move_resize;
   
   return obj;
}

/* Initializes the new picker object */
static void _etk_colorpicker_picker_smart_add(Evas_Object *obj)
{
   Etk_Colorpicker_Picker_Data *picker_data;
   Evas *evas;
   
   if (!obj || !(evas = evas_object_evas_get(obj)))
      return;
   
   picker_data = malloc(sizeof(Etk_Colorpicker_Picker_Data));
   picker_data->objects = NULL;
   picker_data->cp = NULL;
   picker_data->move_resize = NULL;
   evas_object_smart_data_set(obj, picker_data);
}

/* Destroys the picker object */
static void _etk_colorpicker_picker_smart_del(Evas_Object *obj)
{
   Etk_Colorpicker_Picker_Data *picker_data;
   
   if (!obj || !(picker_data = evas_object_smart_data_get(obj)))
      return;

   while (picker_data->objects)
   {
      evas_object_del(picker_data->objects->data);
      picker_data->objects = evas_list_remove_list(picker_data->objects, picker_data->objects);
   }
   free(picker_data);
}

/* Moves the picker object */
static void _etk_colorpicker_picker_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   Etk_Colorpicker_Picker_Data *picker_data;
   Evas_Coord w, h;
   
   if (!obj || !(picker_data = evas_object_smart_data_get(obj)))
      return;

   evas_object_geometry_get(obj, NULL, NULL, &w, &h);
   if (picker_data->cp && picker_data->move_resize)
      picker_data->move_resize(picker_data->cp, x, y, w, h);
}

/* Resizes the picker object */
static void _etk_colorpicker_picker_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   Etk_Colorpicker_Picker_Data *picker_data;
   Evas_Coord x, y;
   
   if (!obj || !(picker_data = evas_object_smart_data_get(obj)))
      return;

   evas_object_geometry_get(obj, &x, &y, NULL, NULL);
   if (picker_data->cp && picker_data->move_resize)
      picker_data->move_resize(picker_data->cp, x, y, w, h);
}

/* Shows the picker object */
static void _etk_colorpicker_picker_smart_show(Evas_Object *obj)
{
   Etk_Colorpicker_Picker_Data *picker_data;
   Evas_List *l;
   
   if (!obj || !(picker_data = evas_object_smart_data_get(obj)))
      return;

   for (l = picker_data->objects; l; l = l->next)
      evas_object_show(l->data);
}

/* Hides the picker object */
static void _etk_colorpicker_picker_smart_hide(Evas_Object *obj)
{
   Etk_Colorpicker_Picker_Data *picker_data;
   Evas_List *l;
   
   if (!obj || !(picker_data = evas_object_smart_data_get(obj)))
      return;

   for (l = picker_data->objects; l; l = l->next)
      evas_object_hide(l->data);
}

/* Sets the clip of the picker object */
static void _etk_colorpicker_picker_smart_clip_set(Evas_Object *obj, Evas_Object *clip)
{
   Etk_Colorpicker_Picker_Data *picker_data;
   Evas_List *l;
   
   if (!obj || !(picker_data = evas_object_smart_data_get(obj)))
      return;

   for (l = picker_data->objects; l; l = l->next)
      evas_object_clip_set(l->data, clip);
}

/* Unsets the clip of the picker object */
static void _etk_colorpicker_picker_smart_clip_unset(Evas_Object *obj)
{
   Etk_Colorpicker_Picker_Data *picker_data;
   Evas_List *l;
   
   if (!obj || !(picker_data = evas_object_smart_data_get(obj)))
      return;

   for (l = picker_data->objects; l; l = l->next)
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
   
   _etk_colorpicker_sp_cursor_move_resize(cp);
}

/* Moves and resizes the vertical picker */
static void _etk_colorpicker_vp_move_resize(Etk_Colorpicker *cp, int x, int y, int w, int h)
{
   if (!cp || !cp->vp_image)
      return;
   
   evas_object_move(cp->vp_image, x, y);
   evas_object_resize(cp->vp_image, w, h);
   evas_object_image_fill_set(cp->vp_image, 0, 0, w, h);
   
   _etk_colorpicker_vp_cursor_move_resize(cp);
}

/* Moves and resizes the cursor of the square picker to the correct position/size */
static void _etk_colorpicker_sp_cursor_move_resize(Etk_Colorpicker *cp)
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

/* Moves and resizes the cursor of the vertical picker to the correct position/size */
static void _etk_colorpicker_vp_cursor_move_resize(Etk_Colorpicker *cp)
{
   int x, y, w, h;
   
   if (!cp || !cp->vp_image || !cp->vp_cursor)
      return;
   
   evas_object_geometry_get(cp->vp_image, &x, &y, &w, &h);
   evas_object_move(cp->vp_cursor, x, y + ((1.0 - cp->vp_pos) * (h - 1)));
   evas_object_resize(cp->vp_cursor, w, 1);
}

/* Updates of the colorpicker */ 
static void _etk_colorpicker_update(Etk_Colorpicker *cp, Etk_Bool sp_image, Etk_Bool sp_cursor, Etk_Bool vp_image, Etk_Bool vp_cursor)
{
   int r, g, b;
   float h, s, v;
   
   if (!cp)
      return;
   
   cp->sp_image_needs_update |= sp_image;
   cp->sp_cursor_needs_update |= sp_cursor;
   cp->vp_image_needs_update |= vp_image;
   cp->vp_cursor_needs_update |= vp_cursor;
   cp->sliders_need_update = ETK_TRUE;
   
   /* Updates the color and the sliders */
   _etk_colorpicker_color_calc(cp->mode, cp->sp_xpos, cp->sp_ypos, cp->vp_pos, &r, &g, &b);
   if (cp->current_color.r != r || cp->current_color.g != g || cp->current_color.b != b)
   {
      _etk_colorpicker_rgb_to_hsv(r, g, b, &h, &s, &v);
      
      etk_range_value_set(ETK_RANGE(cp->sliders[ETK_COLORPICKER_H]), h);
      etk_range_value_set(ETK_RANGE(cp->sliders[ETK_COLORPICKER_S]), s);
      etk_range_value_set(ETK_RANGE(cp->sliders[ETK_COLORPICKER_V]), v);
      etk_range_value_set(ETK_RANGE(cp->sliders[ETK_COLORPICKER_R]), r);
      etk_range_value_set(ETK_RANGE(cp->sliders[ETK_COLORPICKER_G]), g);
      etk_range_value_set(ETK_RANGE(cp->sliders[ETK_COLORPICKER_B]), b);
      
      cp->current_color.r = r;
      cp->current_color.g = g;
      cp->current_color.b = b;
      
      etk_signal_emit(_etk_colorpicker_signals[ETK_CP_COLOR_CHANGED_SIGNAL], ETK_OBJECT(cp), NULL);
   }
   
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
   _etk_colorpicker_rgb_to_hsv(r, g, b, &h, &s, &v);
   
   for (i = 0; i < 6; i++)
   {
      if (cp->sliders_image[i] && (data = (uint32_t *)evas_object_image_data_get(cp->sliders_image[i], 1)))
      {
         for (j = 0; j < cp->sliders_res; j++)
         {
            switch (i)
            {
               case ETK_COLORPICKER_H:
                  _etk_colorpicker_hsv_to_rgb(360.0 * ((float)j / cp->sliders_res), s, v, &r2, &g2, &b2);
                  break;
               case ETK_COLORPICKER_S:
                  _etk_colorpicker_hsv_to_rgb(h, (float)j / cp->sliders_res, v, &r2, &g2, &b2);
                  break;
               case ETK_COLORPICKER_V:
                  _etk_colorpicker_hsv_to_rgb(h, s, (float)j / cp->sliders_res, &r2, &g2, &b2);
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
         _etk_colorpicker_hsv_to_rgb(360.0 * (1.0 - ((float)i / cp->vp_res)), 1.0, 1.0, r, g, b);
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
         _etk_colorpicker_hsv_to_rgb(vp_pos * 360.0, sp_ypos, sp_xpos, r, g, b);
         break;
      case ETK_COLORPICKER_S:
         _etk_colorpicker_hsv_to_rgb(sp_ypos * 360.0, vp_pos, sp_xpos, r, g, b);
         break;
      case ETK_COLORPICKER_V:
         _etk_colorpicker_hsv_to_rgb(sp_ypos * 360.0, sp_xpos, vp_pos, r, g, b);
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

/* Converts a hsv color to rgb. (r, g, b) must not be NULL! */
static void _etk_colorpicker_hsv_to_rgb(float h, float s, float v, int *r, int *g, int *b)
{
   int i;
   float f;

   v *= 255;
   if (s == 0)
   {
      *r = v;  *g = v;  *b = v;
      return;
   }

   h /= 60;
   i = h;
   f = h - i;

   s *= v;
   f *= s;
   s = v - s;
   switch (i)
   {
      case 1:
         *r = v - f;  *g = v;  *b = s;
         return;
      case 2:
         *r = s;  *g = v;  *b = s + f;
         return;
      case 3:
         *r = s;  *g = v - f;  *b = v;
         return;
      case 4:
         *r = s + f;  *g = s;  *b = v;
         return;
      case 5:
         *r = v;  *g = s;  *b = v - f;
         return;
      default:
         *r = v;  *g = s + f;  *b = s;
   }
}

/* Converts a rgb color to hsv. (h, s, v) must not be NULL! */
static void _etk_colorpicker_rgb_to_hsv(int r, int g, int b, float *h, float *s, float *v)
{
   float  min, max, del;

   min = ETK_MIN(r,g);  min = ETK_MIN(min,b);
   max = ETK_MAX(r,g);  max = ETK_MAX(max,b);
   del = max - min;

   *v = (max / 255);
   if ((max == 0) || (del == 0))
   {
      *s = 0; *h = 0;
      return; 
   }

   *s = (del / max);

   if (r == max)
      *h = ((g - b) / del);
   else if (g == max)
      *h = 2 + ((b - r) / del);
   else if (b == max)
      *h = 4 + ((r - g) / del);

   *h *= 60;
   if (*h < 0)
      *h += 360;
}

/** @} */
