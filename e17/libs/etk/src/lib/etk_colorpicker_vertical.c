/** @file etk_colorpicker_vertical.c */
#include "etk_colorpicker_vertical.h"
#include <stdlib.h>
#include <stdint.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"
#include "etk_colorpicker.h"

/**
 * @addtogroup Etk_Colorpicker_Vertical
 * @{
 */

enum _Etk_Cpv_Signal_Id
{
   ETK_CPV_COLOR_SELECTED_SIGNAL,
   ETK_CPV_NUM_SIGNALS
};

enum _Etk_Cpv_Property_Id
{
   ETK_CPV_MAP_WIDTH_PROPERTY,
   ETK_CPV_MAP_HEIGHT_PROPERTY,
   ETK_CPV_COLOR_MODE_PROPERTY,
   ETK_CPV_CURRENT_POS_PROPERTY
};

static void _etk_colorpicker_vertical_constructor(Etk_Colorpicker_Vertical *cpv);
static void _etk_colorpicker_vertical_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_colorpicker_vertical_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_colorpicker_vertical_realize_cb(Etk_Object *object, void *data);
static void _etk_colorpicker_vertical_unrealize_cb(Etk_Object *object, void *data);
static void _etk_colorpicker_vertical_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

static void _etk_colorpicker_vertical_move_cb(Etk_Object *w, void *ev_data, void *user_data);
static void _etk_colorpicker_vertical_down_cb(Etk_Object *w, void *ev_data, void *user_data);
static void _etk_colorpicker_vertical_up_cb(Etk_Object *w, void *ev_data, void *user_data);

static void _etk_colorpicker_vertical_map_draw(Etk_Colorpicker_Vertical *cpv);
static void _etk_colorpicker_vertical_cursor_draw(Etk_Colorpicker_Vertical *cpv, unsigned char pos);

static void _etk_colorpicker_vertical_color_get_h(unsigned char pos, Etk_Color *color);
static void _etk_colorpicker_vertical_color_get_s(unsigned char pos, Etk_Color *color);
static void _etk_colorpicker_vertical_color_get_v(unsigned char pos, Etk_Color *color);
static void _etk_colorpicker_vertical_color_get_r(unsigned char pos, Etk_Color *color);
static void _etk_colorpicker_vertical_color_get_g(unsigned char pos, Etk_Color *color);
static void _etk_colorpicker_vertical_color_get_b(unsigned char pos, Etk_Color *color);

static void _etk_colorpicker_vertical_resize_map_cursor(Etk_Colorpicker_Vertical *cpv);


static Etk_Signal *_etk_colorpicker_vertical_signals[ETK_CPV_NUM_SIGNALS];


/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Colorpicker_Vertical
 * @return Returns the type on an Etk_Colorpicker_Vertical
 */
Etk_Type *etk_colorpicker_vertical_type_get()
{
   static Etk_Type *cpv_type = NULL;

   if (!cpv_type)
   {
      cpv_type = etk_type_new("Etk_Colorpicker_Vertical", ETK_WIDGET_TYPE, sizeof(Etk_Colorpicker_Vertical), ETK_CONSTRUCTOR(_etk_colorpicker_vertical_constructor), NULL);

      _etk_colorpicker_vertical_signals[ETK_CPV_COLOR_SELECTED_SIGNAL] = etk_signal_new("color_selected", cpv_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      
      etk_type_property_add(cpv_type, "map_width", ETK_CPV_MAP_WIDTH_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(0));
      etk_type_property_add(cpv_type, "map_height", ETK_CPV_MAP_HEIGHT_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(0));
      etk_type_property_add(cpv_type, "color_mode", ETK_CPV_COLOR_MODE_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_COLOR_MODE_H));
      etk_type_property_add(cpv_type, "cursor_pos", ETK_CPV_CURRENT_POS_PROPERTY, ETK_PROPERTY_CHAR, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_char(0));

      cpv_type->property_set = _etk_colorpicker_vertical_property_set;
      cpv_type->property_get = _etk_colorpicker_vertical_property_get;
   }

   return cpv_type;
}

/**
 * @brief Creates a new cpv
 * @param map_width the width of the map used by the vertical colorpicker
 * @param map_height the height of the map used by the vertical colorpicker
 * @return Returns the new cpv widget
 */
Etk_Widget *etk_colorpicker_vertical_new(int map_width, int map_height)
{
   return etk_widget_new(ETK_COLORPICKER_VERTICAL_TYPE, "map_width", map_width, "map_height", map_height, NULL);
}

/**
 * @brief Sets the size of the map used by the colorpicker
 * @param cpv a vertical colorpicker
 * @param map_width the width of the map used by the vertical colorpicker
 * @param map_height the height of the map used by the vertical colorpicker
 */
void etk_colorpicker_vertical_map_size_set(Etk_Colorpicker_Vertical *cpv, int map_width, int map_height)
{
   Etk_Bool need_resize = ETK_FALSE;

   if (!cpv)
      return;

   if (cpv->map_width != map_width)
   {
      cpv->map_width = map_width;
      etk_object_notify(ETK_OBJECT(cpv), "map_width");
      need_resize = ETK_TRUE;
   }
   if (cpv->map_height != map_height)
   {
      cpv->map_height = map_height;
      etk_object_notify(ETK_OBJECT(cpv), "map_height");
      need_resize = ETK_TRUE;
   }

   if (need_resize)
      _etk_colorpicker_vertical_resize_map_cursor(cpv);
}

/**
 * @brief Gets the size of the vertical colorpicker
 * @param cpv a vertical colorpicker
 * @param map_width the location to store the width of the map
 * @param map_height the location to store the height of the map
 */
void etk_colorpicker_vertical_map_size_get(Etk_Colorpicker_Vertical *cpv, int *map_width, int *map_height)
{
   if (!cpv)
      return;

   if (map_width)
      *map_width = cpv->map_width;
   if (map_height)
      *map_height = cpv->map_height;
}

/**
 * @brief Sets the color mode of the vertical colorpicker
 * @param cpv a vertical colorpicker
 * @param color_mode the color mode to use
 */
void etk_colorpicker_vertical_color_mode_set(Etk_Colorpicker_Vertical *cpv, Etk_Color_Mode color_mode)
{
   if (!cpv)
      return;

   if (cpv->color_mode != color_mode)
   {
      cpv->color_mode = color_mode;

      _etk_colorpicker_vertical_map_draw(cpv);
      _etk_colorpicker_vertical_cursor_draw(cpv, cpv->cursor_pos);

      etk_object_notify(ETK_OBJECT(cpv), "color_mode");
   }
}

/**
 * @brief Gets the color mode used by the vertical colorpicker
 * @param cpv a vertical colorpicker
 * @return Returns the color mode used by the vertical colorpicker
 */
Etk_Color_Mode etk_colorpicker_vertical_color_mode_get(Etk_Colorpicker_Vertical *cpv)
{
   if (!cpv)
      return ETK_COLOR_MODE_H;
   return cpv->color_mode;
}

/**
 * @brief Sets the position of the cursor
 * @param cpv a vertical colorpicker
 * @param pos the position of the cursor to set
 */
void etk_colorpicker_vertical_pos_set(Etk_Colorpicker_Vertical *cpv, unsigned char pos)
{
   if (!cpv)
      return;

   /* We update the cpv if necessary */
   if (cpv->cursor_pos != pos)
   {
      cpv->cursor_pos = pos;
      etk_signal_emit(_etk_colorpicker_vertical_signals[ETK_CPV_COLOR_SELECTED_SIGNAL], ETK_OBJECT(cpv), NULL);
      etk_object_notify(ETK_OBJECT(cpv), "cursor_pos");
      _etk_colorpicker_vertical_cursor_draw(cpv, cpv->cursor_pos);
      etk_widget_redraw_queue(ETK_WIDGET(cpv));
   }
}

/**
 * @brief Gets the position of the cursor of the vertical colorpicker
 * @param cpv a vertical colorpicker
 * @return Returns the position of the cursor of the vertical colorpicker
 */
int etk_colorpicker_vertical_pos_get(Etk_Colorpicker_Vertical *cpv)
{
   if (!cpv)
      return 0;
   return cpv->cursor_pos;
}

/**
 * @brief Gets a color in the map of the vertical colorpicker
 * @param cpv a vertical colorpicker
 * @param pos the position of the color in the map
 * @return Returns the color in the map of the vertical colorpicker
 */
Etk_Color etk_colorpicker_vertical_color_get(Etk_Colorpicker_Vertical *cpv, unsigned char pos)
{
   Etk_Color color;

   if (!cpv)
   {
      color.r = 0;
      color.g = 0;
      color.b = 0;
      return color;
   }

   switch (cpv->color_mode)
   {
      case ETK_COLOR_MODE_H:
      {
         _etk_colorpicker_vertical_color_get_h(pos, &color);
         break;
      }
      case ETK_COLOR_MODE_S:
      {
         _etk_colorpicker_vertical_color_get_s(pos, &color);
         break;
      }
      case ETK_COLOR_MODE_V:
      {
         _etk_colorpicker_vertical_color_get_v(pos, &color);
         break;
      }
      case ETK_COLOR_MODE_R:
      {
         _etk_colorpicker_vertical_color_get_r(pos, &color);
         break;
      }
      case ETK_COLOR_MODE_G:
      {
         _etk_colorpicker_vertical_color_get_g(pos, &color);
         break;
      }
      case ETK_COLOR_MODE_B:
      {
         _etk_colorpicker_vertical_color_get_b(pos, &color);
         break;
      }
   }
   
   return color;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_colorpicker_vertical_constructor(Etk_Colorpicker_Vertical *cpv)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(cpv)))
      return;

   cpv->map = NULL;
   cpv->cursor = NULL;
   cpv->map_width = 0;
   cpv->map_height = 0;

   cpv->color_mode = ETK_COLOR_MODE_H;
   cpv->cursor_pos = 0;
   cpv->drag = ETK_FALSE;

   widget->size_allocate = _etk_colorpicker_vertical_size_allocate;

   etk_signal_connect("realize", ETK_OBJECT(cpv), ETK_CALLBACK(_etk_colorpicker_vertical_realize_cb), NULL);
   etk_signal_connect("unrealize", ETK_OBJECT(cpv), ETK_CALLBACK(_etk_colorpicker_vertical_unrealize_cb), NULL);
   etk_signal_connect("mouse_down", ETK_OBJECT(cpv), ETK_CALLBACK(_etk_colorpicker_vertical_down_cb), NULL);
   etk_signal_connect("mouse_up", ETK_OBJECT(cpv), ETK_CALLBACK(_etk_colorpicker_vertical_up_cb), NULL);
   etk_signal_connect("mouse_move", ETK_OBJECT(cpv), ETK_CALLBACK(_etk_colorpicker_vertical_move_cb), NULL);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_colorpicker_vertical_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Colorpicker_Vertical *cpv;

   if (!(cpv = ETK_COLORPICKER_VERTICAL(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_CPV_MAP_WIDTH_PROPERTY:
         etk_colorpicker_vertical_map_size_set(cpv, etk_property_value_int_get(value), cpv->map_height);
         break;
      case ETK_CPV_MAP_HEIGHT_PROPERTY:
         etk_colorpicker_vertical_map_size_set(cpv, cpv->map_width, etk_property_value_int_get(value));
         break;
      case ETK_CPV_COLOR_MODE_PROPERTY:
         etk_colorpicker_vertical_color_mode_set(cpv, etk_property_value_int_get(value));
         break;
      case ETK_CPV_CURRENT_POS_PROPERTY:
         etk_colorpicker_vertical_pos_set(cpv, etk_property_value_char_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_colorpicker_vertical_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Colorpicker_Vertical *cpv;

   if (!(cpv = ETK_COLORPICKER_VERTICAL(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_CPV_MAP_WIDTH_PROPERTY:
         etk_property_value_int_set(value, cpv->map_width);
         break;
      case ETK_CPV_MAP_HEIGHT_PROPERTY:
         etk_property_value_int_set(value, cpv->map_height);
         break;
      case ETK_CPV_COLOR_MODE_PROPERTY:
         etk_property_value_int_set(value, cpv->color_mode);
         break;
      case ETK_CPV_CURRENT_POS_PROPERTY:
         etk_property_value_char_set(value, cpv->cursor_pos);
         break;
      default:
         break;
   }
}

/* Called when the cpv is realized */
static void _etk_colorpicker_vertical_realize_cb(Etk_Object *object, void *data)
{
   Etk_Colorpicker_Vertical *cpv;
   Evas *evas;
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(object)) || !(evas = etk_widget_toplevel_evas_get(widget)))
      return;
   cpv = ETK_COLORPICKER_VERTICAL(widget);

   cpv->map = evas_object_image_add(evas);
   evas_object_show(cpv->map);
   etk_widget_member_object_add(widget, cpv->map);
   cpv->cursor = evas_object_image_add(evas);
   evas_object_show(cpv->cursor);
   etk_widget_member_object_add(widget, cpv->cursor);

   _etk_colorpicker_vertical_resize_map_cursor(cpv);

   _etk_colorpicker_vertical_cursor_draw(cpv, cpv->cursor_pos);
   _etk_colorpicker_vertical_map_draw(cpv);
}

/* Called when the cpv is unrealized */
static void _etk_colorpicker_vertical_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Colorpicker_Vertical *cpv;

   if (!(cpv = ETK_COLORPICKER_VERTICAL(object)))
      return;

   cpv->map = NULL;
   cpv->cursor = NULL;
}

/* Moves and resizes the cpv */
static void _etk_colorpicker_vertical_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Colorpicker_Vertical *cpv;

   if (!(cpv = ETK_COLORPICKER_VERTICAL(widget)))
      return;

   if (cpv->map)
   {
      evas_object_move(cpv->map, geometry.x, geometry.y);
      evas_object_resize(cpv->map, geometry.w, geometry.h);
      evas_object_image_fill_set(cpv->map, 0, 0, geometry.w, geometry.h);
   }
   if (cpv->cursor)
   {
      evas_object_move(cpv->cursor, geometry.x, geometry.y + (cpv->cursor_pos * geometry.h) / 255);
      evas_object_resize(cpv->cursor, geometry.w, 1);
      evas_object_image_fill_set(cpv->cursor, 0, 0, geometry.w, 1);
   }
}

/* Called when the button of the mouse is moved over the widget */
static void _etk_colorpicker_vertical_move_cb(Etk_Object *w, void *ev_data, void *user_data)
{
  Etk_Colorpicker_Vertical *cpv;
  Etk_Event_Mouse_Move *ev = ev_data;

   if (!(cpv = ETK_COLORPICKER_VERTICAL(w)))
      return;

   if (cpv->drag)
      etk_colorpicker_vertical_pos_set(cpv, ETK_CLAMP((ev->cur.widget.y * 255) / ETK_WIDGET(cpv)->geometry.h, 0, 255));
}

/* Called when the button of the mouse is pressed */
static void _etk_colorpicker_vertical_down_cb(Etk_Object *w, void *ev_data, void *user_data)
{
   Etk_Colorpicker_Vertical *cpv;
   Etk_Event_Mouse_Up_Down *ev = ev_data;

   if (!(cpv = ETK_COLORPICKER_VERTICAL(w)))
      return;

   cpv->drag = ETK_TRUE;
   etk_colorpicker_vertical_pos_set(cpv, (ev->widget.y * 255) / ETK_WIDGET(cpv)->geometry.h);
}

/* Called when the button of the mouse is released */
static void _etk_colorpicker_vertical_up_cb (Etk_Object *w, void *ev_data, void *user_data)
{
   Etk_Colorpicker_Vertical *cpv;

   if (!(cpv = ETK_COLORPICKER_VERTICAL(w)))
      return;
  
   cpv->drag = ETK_FALSE;
}

/**************************
 *
 * Private methods
 *
 **************************/

/* (Re)draws the map of the vertical colorpicker */ 
static void _etk_colorpicker_vertical_map_draw(Etk_Colorpicker_Vertical *cpv)
{
   uint32_t *map, *m;
   uint32_t val;
   int i, j, offset;
   float ratio;
   
   if (!cpv || !cpv->map)
      return;

   map = (uint32_t *)evas_object_image_data_get(cpv->map, 1);
   if (!map)
      return;
   
   m = map;
   offset = 0;
   ratio = 255.0 / (cpv->map_height - 1);
   for (j = 0; j < cpv->map_height; j++)
   {
      Etk_Color color;
        
      color = etk_colorpicker_vertical_color_get(cpv, j * ratio);
      val = ((255 << 24) |
         (color.r << 16) |
         (color.g << 8) |
         (color.b));
      for (i = 0; i < cpv->map_width; i++)
      {
         map[offset] = val;
         offset++;
      }
   }
   evas_object_image_data_update_add(cpv->map, 0, 0, cpv->map_width, cpv->map_height);
}

/* (Re)draws the cursor of the vertical colorpicker */ 
static void _etk_colorpicker_vertical_cursor_draw(Etk_Colorpicker_Vertical *cpv, unsigned char pos)
{
   uint32_t *cursor;
   uint32_t val;
   int i;
   Etk_Color color;
   
   if (!cpv || !cpv->cursor)
      return;

   cursor = (uint32_t *)evas_object_image_data_get(cpv->cursor, 1);
   if (!cursor)
      return;

   color = etk_colorpicker_vertical_color_get(cpv, pos);
   val = ((255 << 24) |
      ((255 - color.r) << 16) |
      ((255 - color.g) << 8) |
      ((255 - color.b)));
   
   for (i = 0; i < cpv->map_width; i++)
      cursor[i] = val;
   
   evas_object_image_data_update_add(cpv->cursor, 0, 0, cpv->map_width, 1);
}

/* Gets the color in the H color mode */
static void _etk_colorpicker_vertical_color_get_h(unsigned char pos, Etk_Color *color)
{ 
   if (pos == 0)
      pos = 255;
   etk_colorpicker_hsv_to_rgb(100.0 - (pos / 2.55), 100.0, 100.0, color);
}

/* Gets the color in the S color mode */
static void _etk_colorpicker_vertical_color_get_s(unsigned char pos, Etk_Color *color)
{
  etk_colorpicker_hsv_to_rgb(0.0, 100.0 - (pos / 2.55), 100.0, color);
}

/* Gets the color in the V color mode */
static void _etk_colorpicker_vertical_color_get_v(unsigned char pos, Etk_Color *color)
{
  etk_colorpicker_hsv_to_rgb(0.0, 0.0, 100.0 - (pos / 2.55), color);
}

/* Gets the color in the R color mode */
static void _etk_colorpicker_vertical_color_get_r(unsigned char pos, Etk_Color *color)
{
  color->r = (unsigned char)(255 - pos);
  color->g = 0;
  color->b = 0;
}

/* Gets the color in the G color mode */
static void _etk_colorpicker_vertical_color_get_g(unsigned char pos, Etk_Color *color)
{
  color->r = 0;
  color->g = (unsigned char)(255 - pos);
  color->b = 0;
}

/* Gets the color in the B color mode */
static void _etk_colorpicker_vertical_color_get_b(unsigned char pos, Etk_Color *color)
{
  color->r = 0;
  color->g = 0;
  color->b = (unsigned char)(255 - pos);
}

/* Resizes the map and the cursor images */
static void _etk_colorpicker_vertical_resize_map_cursor(Etk_Colorpicker_Vertical *cpv)
{
   if (!cpv)
      return;

   if (cpv->map)
      evas_object_image_size_set(cpv->map, cpv->map_width, cpv->map_height);
   if (cpv->cursor)
      evas_object_image_size_set(cpv->cursor, cpv->map_width, 1);
}

/** @} */
