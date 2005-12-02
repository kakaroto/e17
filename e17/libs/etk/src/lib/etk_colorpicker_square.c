/** @file etk_colorpicker_square.c */
#include "etk_colorpicker_square.h"
#include <stdlib.h>
#include <stdint.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Colorpicker_Square
 * @{
 */

enum _Etk_Cps_Signal_Id
{
   ETK_CPS_COLOR_SELECTED_SIGNAL,
   ETK_CPS_NUM_SIGNALS
};

enum _Etk_Cps_Property_Id
{
   ETK_CPS_MAP_WIDTH_PROPERTY,
   ETK_CPS_MAP_HEIGHT_PROPERTY,
   ETK_CPS_COLOR_MODE_PROPERTY,
   ETK_CPS_CURSOR_X_PROPERTY,
   ETK_CPS_CURSOR_Y_PROPERTY
};

static void _etk_colorpicker_square_constructor(Etk_Colorpicker_Square *cps);
static void _etk_colorpicker_square_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_colorpicker_square_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_colorpicker_square_realize_cb(Etk_Object *object, void *data);
static void _etk_colorpicker_square_unrealize_cb(Etk_Object *object, void *data);
static void _etk_colorpicker_square_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

static void _etk_colorpicker_square_move_cb(Etk_Object *w, void *ev_data, void *user_data);
static void _etk_colorpicker_square_down_cb(Etk_Object *w, void *ev_data, void *user_data);
static void _etk_colorpicker_square_up_cb(Etk_Object *w, void *ev_data, void *user_data);

static void _etk_colorpicker_square_map_draw(Etk_Colorpicker_Square *cps);
static void _etk_colorpicker_square_cursor_draw(Etk_Colorpicker_Square *cps, unsigned char x, unsigned char y);
static void _etk_colorpicker_square_hcursor_draw(Etk_Colorpicker_Square *sp, unsigned char y);
static void _etk_colorpicker_square_vcursor_draw(Etk_Colorpicker_Square *sp, unsigned char x);

static void _etk_colorpicker_square_color_get_h(Etk_Colorpicker_Square *cps, unsigned char x, unsigned char y, Etk_Color *color);
static void _etk_colorpicker_square_color_get_s(Etk_Colorpicker_Square *cps, unsigned char x, unsigned char y, Etk_Color *color);
static void _etk_colorpicker_square_color_get_v(Etk_Colorpicker_Square *cps, unsigned char x, unsigned char y, Etk_Color *color);
static void _etk_colorpicker_square_color_get_r(Etk_Colorpicker_Square *cps, unsigned char x, unsigned char y, Etk_Color *color);
static void _etk_colorpicker_square_color_get_g(Etk_Colorpicker_Square *cps, unsigned char x, unsigned char y, Etk_Color *color);
static void _etk_colorpicker_square_color_get_b(Etk_Colorpicker_Square *cps, unsigned char x, unsigned char y, Etk_Color *color);

static void _etk_colorpicker_square_resize_map_cursor(Etk_Colorpicker_Square *cps);

static Etk_Signal *_etk_colorpicker_square_signals[ETK_CPS_NUM_SIGNALS];


/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Colorpicker_Square
 * @return Returns the type on an Etk_Colorpicker_Square
 */
Etk_Type *etk_colorpicker_square_type_get()
{
   static Etk_Type *cps_type = NULL;

   if (!cps_type)
   {
      cps_type = etk_type_new("Etk_Colorpicker_Square", ETK_WIDGET_TYPE, sizeof(Etk_Colorpicker_Square), ETK_CONSTRUCTOR(_etk_colorpicker_square_constructor), NULL);

      _etk_colorpicker_square_signals[ETK_CPS_COLOR_SELECTED_SIGNAL] = etk_signal_new("color_selected", cps_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      
      etk_type_property_add(cps_type, "map_width", ETK_CPS_MAP_WIDTH_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(0));
      etk_type_property_add(cps_type, "map_height", ETK_CPS_MAP_HEIGHT_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(0));
      etk_type_property_add(cps_type, "color_mode", ETK_CPS_COLOR_MODE_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(ETK_COLOR_MODE_H));
      etk_type_property_add(cps_type, "cursor_x", ETK_CPS_CURSOR_X_PROPERTY, ETK_PROPERTY_CHAR, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_char(0));
      etk_type_property_add(cps_type, "cursor_y", ETK_CPS_CURSOR_Y_PROPERTY, ETK_PROPERTY_CHAR, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_char(0));

      cps_type->property_set = _etk_colorpicker_square_property_set;
      cps_type->property_get = _etk_colorpicker_square_property_get;
   }

   return cps_type;
}

/**
 * @brief Creates a new square colorpicker
 * @param map_width the width of the map used by the square colorpicker
 * @param map_height the height of the map used by the square colorpicker
 * @return Returns the new square colorpicker widget
 */
Etk_Widget *etk_colorpicker_square_new(int map_width, int map_height)
{
   return etk_widget_new(ETK_COLORPICKER_SQUARE_TYPE, "map_width", map_width, "map_height", map_height, NULL);
}

/**
 * @brief Sets the size of the map used by the color picker
 * @param cps a square colorpicker
 * @param map_width the width of the map used by the square colorpicker
 * @param map_height the height of the map used by the square colorpicker
 */
void etk_colorpicker_square_map_size_set(Etk_Colorpicker_Square *cps, int map_width, int map_height)
{
   Etk_Bool need_resize = FALSE;

   if (!cps)
      return;

   if (cps->map_width != map_width)
   {
      cps->map_width = map_width;
      etk_object_notify(ETK_OBJECT(cps), "map_width");
      need_resize = TRUE;
   }
   if (cps->map_height != map_height)
   {
      cps->map_height = map_height;
      etk_object_notify(ETK_OBJECT(cps), "map_height");
      need_resize = TRUE;
   }

   if (need_resize)
      _etk_colorpicker_square_resize_map_cursor(cps);
}

/**
 * @brief Gets the size of the square colorpicker
 * @param cps a square colorpicker
 * @param map_width the location to store the width of the map
 * @param map_height the location to store the height of the map
 */
void etk_colorpicker_square_map_size_get(Etk_Colorpicker_Square *cps, int *map_width, int *map_height)
{
   if (!cps)
      return;

   if (map_width)
      *map_width = cps->map_width;
   if (map_height)
      *map_height = cps->map_height;
}

/**
 * @brief Sets the color mode of the square colorpicker
 * @param cps a square colorpicker
 * @param color_mode the color mode to use
 */
void etk_colorpicker_square_color_mode_set(Etk_Colorpicker_Square *cps, Etk_Color_Mode color_mode)
{
   if (!cps)
      return;

   if (cps->color_mode != color_mode)
   {
      cps->color_mode = color_mode;
      etk_colorpicker_square_update(cps);
      etk_object_notify(ETK_OBJECT(cps), "color_mode");
   }
}

/**
 * @brief Gets the color mode used by the square colorpicker
 * @param cps a square colorpicker
 * @return Returns the color mode used by the square colorpicker
 */
Etk_Color_Mode etk_colorpicker_square_color_mode_get(Etk_Colorpicker_Square *cps)
{
   if (!cps)
      return ETK_COLOR_MODE_H;
   return cps->color_mode;
}

/**
 * @brief Sets the position of the cursor of the square colorpicker
 * @param cps a square colorpicker
 * @param x the x position of the cursor of the square colorpicker
 * @param y the y position of the cursor of the square colorpicker
 * @return Returns the color mode used by the square colorpicker
 */
void etk_colorpicker_square_cursor_xy_set(Etk_Colorpicker_Square *cps, unsigned char x, unsigned char y)
{
   int old_x, old_y;

   if (!cps)
      return;

   old_x = cps->cursor_x;
   old_y = cps->cursor_y;

   /* We update the cps if necessary */
   if (cps->cursor_x != x)
   {
      cps->cursor_x = x;
      etk_object_notify(ETK_OBJECT(cps), "cursor_x");
   }
   if (cps->cursor_y != y)
   {
      cps->cursor_y = y;
      etk_object_notify(ETK_OBJECT(cps), "cursor_y");
   }

   if ((old_x != cps->cursor_x) || (old_y != cps->cursor_y))
   {
      etk_signal_emit(_etk_colorpicker_square_signals[ETK_CPS_COLOR_SELECTED_SIGNAL], ETK_OBJECT(cps), NULL);
      _etk_colorpicker_square_cursor_draw(cps, cps->cursor_x, cps->cursor_y);
      etk_widget_redraw_queue(ETK_WIDGET(cps));
   }
}

/**
 * @brief Gets the position of the cursor of the square colorpicker
 * @param cps a square colorpicker
 * @param x the location to store the x position of the cursor
 * @param y the location to store the y position of the cursor
 */
void etk_colorpicker_square_cursor_xy_get(Etk_Colorpicker_Square *cps, unsigned char *x, unsigned char *y)
{
   if (!cps)
      return;

   if (x)
      *x = cps->cursor_x;
   if (y)
      *y = cps->cursor_y;
}

/**
 * @brief Gets a color in the map of the square colorpicker
 * @param cps a square colorpicker
 * @param x the x position of the color in the map
 * @param y the y position of the color in the map
 * @return Returns the color in the map of the square colorpicker
 */
Etk_Color etk_colorpicker_square_color_get(Etk_Colorpicker_Square *cps, unsigned char x, unsigned char y)
{
   Etk_Color color;

   if (!cps)
   {
      color.r = 0;
      color.g = 0;
      color.b = 0;

      return color;
   }

   switch (cps->color_mode)
   {
      case ETK_COLOR_MODE_H:
      {
         _etk_colorpicker_square_color_get_h(cps, x, y, &color);
         break;
      }
      case ETK_COLOR_MODE_S:
      {
         _etk_colorpicker_square_color_get_s(cps, x, y, &color);
         break;
      }
      case ETK_COLOR_MODE_V:
      {
         _etk_colorpicker_square_color_get_v(cps, x, y, &color);
         break;
      }
      case ETK_COLOR_MODE_R:
      {
         _etk_colorpicker_square_color_get_r(cps, x, y, &color);
         break;
      }
      case ETK_COLOR_MODE_G:
      {
         _etk_colorpicker_square_color_get_g(cps, x, y, &color);
         break;
      }
      case ETK_COLOR_MODE_B:
      {
         _etk_colorpicker_square_color_get_b(cps, x, y, &color);
         break;
      }
   }
   
   return color;
}

/**
 * @brief Update the map and the cursor of the square colorpicker
 * @param cps a square colorpicker
 */
void etk_colorpicker_square_update(Etk_Colorpicker_Square *cps)
{
   if (!cps)
      return;

   _etk_colorpicker_square_map_draw(cps);
   _etk_colorpicker_square_cursor_draw(cps, cps->cursor_x, cps->cursor_y);
   etk_widget_redraw_queue(ETK_WIDGET(cps));
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_colorpicker_square_constructor(Etk_Colorpicker_Square *cps)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(cps)))
      return;

   cps->hcursor = NULL;
   cps->vcursor = NULL;
   cps->map = NULL;
   cps->map_width = 0;
   cps->map_height = 0;

   cps->color_mode = ETK_COLOR_MODE_H;
   cps->cursor_x = 0;
   cps->cursor_y = 0;
   cps->drag = FALSE;
   cps->current_color.r = 255;
   cps->current_color.g = 0;
   cps->current_color.b = 0;

   widget->size_allocate = _etk_colorpicker_square_size_allocate;

   etk_signal_connect("realize", ETK_OBJECT(cps), ETK_CALLBACK(_etk_colorpicker_square_realize_cb), NULL);
   etk_signal_connect("unrealize", ETK_OBJECT(cps), ETK_CALLBACK(_etk_colorpicker_square_unrealize_cb), NULL);
   etk_signal_connect("mouse_down", ETK_OBJECT(cps), ETK_CALLBACK(_etk_colorpicker_square_down_cb), NULL);
   etk_signal_connect("mouse_up", ETK_OBJECT(cps), ETK_CALLBACK(_etk_colorpicker_square_up_cb), NULL);
   etk_signal_connect("mouse_move", ETK_OBJECT(cps), ETK_CALLBACK(_etk_colorpicker_square_move_cb), NULL);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_colorpicker_square_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Colorpicker_Square *cps;

   if (!(cps = ETK_COLORPICKER_SQUARE(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_CPS_MAP_WIDTH_PROPERTY:
         etk_colorpicker_square_map_size_set(cps, etk_property_value_int_get(value), cps->map_height);
         break;
      case ETK_CPS_MAP_HEIGHT_PROPERTY:
         etk_colorpicker_square_map_size_set(cps, cps->map_width, etk_property_value_int_get(value));
         break;
      case ETK_CPS_CURSOR_X_PROPERTY:
         etk_colorpicker_square_cursor_xy_set(cps, etk_property_value_int_get(value), cps->cursor_y);
         break;
      case ETK_CPS_CURSOR_Y_PROPERTY:
         etk_colorpicker_square_cursor_xy_set(cps, cps->cursor_x, etk_property_value_int_get(value));
         break; 
      case ETK_CPS_COLOR_MODE_PROPERTY:
         etk_colorpicker_square_color_mode_set(cps, etk_property_value_int_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_colorpicker_square_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Colorpicker_Square *cps;

   if (!(cps = ETK_COLORPICKER_SQUARE(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_CPS_MAP_WIDTH_PROPERTY:
         etk_property_value_int_set(value, cps->map_width);
         break;
      case ETK_CPS_MAP_HEIGHT_PROPERTY:
         etk_property_value_int_set(value, cps->map_height);
         break;
      case ETK_CPS_CURSOR_X_PROPERTY:
         etk_property_value_int_set(value, cps->cursor_x);
         break;
      case ETK_CPS_CURSOR_Y_PROPERTY:
         etk_property_value_int_set(value, cps->cursor_y);
         break;
      case ETK_CPS_COLOR_MODE_PROPERTY:
         etk_property_value_int_set(value, cps->color_mode);
         break;
      default:
         break;
   }
}

/* Called when the cps is realized */
static void _etk_colorpicker_square_realize_cb(Etk_Object *object, void *data)
{
   Etk_Colorpicker_Square *cps;
   Evas *evas;
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(object)))
      return;

   if (!(cps = ETK_COLORPICKER_SQUARE(object)) || !(evas = etk_widget_toplevel_evas_get(widget)))
      return;

   cps->map = evas_object_image_add(evas);
   evas_object_image_alpha_set(cps->map, 0);
   evas_object_show(cps->map);
   etk_widget_member_object_add(widget, cps->map);
   cps->hcursor = evas_object_image_add(evas);
   evas_object_image_alpha_set(cps->hcursor, 0);
   evas_object_show(cps->hcursor);
   etk_widget_member_object_add(widget, cps->hcursor);
   cps->vcursor = evas_object_image_add(evas);
   evas_object_image_alpha_set(cps->vcursor, 0);
   evas_object_show(cps->vcursor);
   etk_widget_member_object_add(widget, cps->vcursor);

   _etk_colorpicker_square_resize_map_cursor(cps);

   etk_colorpicker_square_update(cps);
}

/* Called when the cps is unrealized */
static void _etk_colorpicker_square_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Colorpicker_Square *cps;

   if (!(cps = ETK_COLORPICKER_SQUARE(object)))
      return;

   cps->map = NULL;
   cps->hcursor = NULL;
   cps->vcursor = NULL;
}

/* Moves and resizes the cps */
static void _etk_colorpicker_square_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Colorpicker_Square *cps;

   if (!(cps = ETK_COLORPICKER_SQUARE(widget)))
      return;

   if (cps->map)
   {
      evas_object_move(cps->map, geometry.x, geometry.y);
      evas_object_resize(cps->map, geometry.w, geometry.h);
      evas_object_image_fill_set(cps->map, 0, 0, geometry.w, geometry.h);
   }
   if (cps->hcursor)
   {
      evas_object_move(cps->hcursor, geometry.x, geometry.y + (cps->cursor_y * geometry.h) / 255);
      evas_object_resize(cps->hcursor, geometry.w, 1);
      evas_object_image_fill_set(cps->hcursor, 0, 0, geometry.w, 1);
   }
   if (cps->vcursor)
   {
      evas_object_move(cps->vcursor, geometry.x + (cps->cursor_x * geometry.w) / 255, geometry.y);
      evas_object_resize(cps->vcursor, 1, geometry.h);
      evas_object_image_fill_set(cps->vcursor, 0, 0, 1, geometry.h);
   }
}

/* Called when the mouse is moved above the colorpicker */
static void _etk_colorpicker_square_move_cb(Etk_Object *w, void *ev_data, void *user_data)
{
   Etk_Colorpicker_Square *cps;
   Etk_Event_Mouse_Move *ev = ev_data;
   
   if (!(cps = ETK_COLORPICKER_SQUARE(w)))
      return;
   
   if (cps->drag)
   {
      etk_colorpicker_square_cursor_xy_set(cps, ETK_CLAMP((ev->cur.widget.x * 255) / ETK_WIDGET(cps)->geometry.w, 0, 255),
         ETK_CLAMP((ev->cur.widget.y * 255) / ETK_WIDGET(cps)->geometry.h, 0, 255));
   }
}

/* Called when a mouse button is pressed */
static void _etk_colorpicker_square_down_cb(Etk_Object *w, void *ev_data, void *user_data)
{
   Etk_Colorpicker_Square *cps;
   Etk_Event_Mouse_Up_Down *ev = ev_data;

   if (!(cps = ETK_COLORPICKER_SQUARE(w)))
      return;

   etk_colorpicker_square_cursor_xy_set(cps, ETK_CLAMP((ev->widget.x * 255) / ETK_WIDGET(cps)->geometry.w, 0, 255),
      ETK_CLAMP((ev->widget.y * 255) / ETK_WIDGET(cps)->geometry.h, 0, 255));
   cps->drag = TRUE;
}

/* Called when a mouse button is released */
static void _etk_colorpicker_square_up_cb(Etk_Object *w, void *ev_data, void *user_data)
{
   Etk_Colorpicker_Square *cps;

   if (!(cps = ETK_COLORPICKER_SQUARE(w)))
      return;
  
   cps->drag = FALSE;
}


/**************************
 *
 * Private methods
 *
 **************************/

/**
 * @brief (Re)draws the map of the square colorpicker
 * @param cps a square colorpicker
 */
static void _etk_colorpicker_square_map_draw(Etk_Colorpicker_Square *cps)
{
   uint32_t *map ;
   uint32_t *m;
   register int i, j;
   float xratio, yratio;
   
   if (!cps || !cps->map)
      return;
   
   map = (uint32_t *)evas_object_image_data_get(cps->map, 1);
   if (!map)
      return;
   
   m = map;
   xratio = 255.0 / (cps->map_width - 1);
   yratio = 255.0 / (cps->map_height - 1);
   for (j = 0; j < cps->map_height; j++)
   {
      Etk_Color color;
      float y = j * yratio;
      
      for (i = 0; i < cps->map_width; i++)
      {
         color = etk_colorpicker_square_color_get(cps, i * xratio, y);
         *m = ((color.r << 16) |
            (color.g << 8) |
            (color.b));
         m++;
      }
   }
   evas_object_image_data_update_add(cps->map, 0, 0, cps->map_width, cps->map_height);
}

/* (Re)draws the cursor of the square colorpicker */
static void _etk_colorpicker_square_cursor_draw(Etk_Colorpicker_Square *cps, unsigned char x, unsigned char y)
{
   _etk_colorpicker_square_hcursor_draw(cps, x);
   _etk_colorpicker_square_vcursor_draw(cps, y);
}

/* (Re)draws the horizontal part of the cursor of the square colorpicker */
static void _etk_colorpicker_square_hcursor_draw(Etk_Colorpicker_Square *cps, unsigned char y)
{
   uint32_t *hcursor;
   uint32_t *c;
   register int i;
   float xratio;
   
   if (!cps || !cps->hcursor)
      return;
   
   hcursor = (uint32_t *)evas_object_image_data_get(cps->hcursor, 1);
   if (!hcursor)
      return;
   
   c = hcursor;
   xratio = 255.0 / (cps->map_width - 1);
   for (i = 0; i < cps->map_width; i++)
   {
      Etk_Color color;
      color = etk_colorpicker_square_color_get(cps, i * xratio, y);
      color.r = 255 - color.r;
      color.g = 255 - color.g;
      color.b = 255 - color.b;

      *c = ((color.r << 16) |
         (color.g << 8) |
         (color.b));
      c++;
   }
   evas_object_image_data_update_add(cps->hcursor, 0, 0, cps->map_width, 1);
}

/* (Re)draws the vertical part of the cursor of the square colorpicker */
static void _etk_colorpicker_square_vcursor_draw(Etk_Colorpicker_Square *cps, unsigned char x)
{
   uint32_t *vcursor;
   uint32_t *c;
   register int i;
   float yratio;
   
   if (!cps || !cps->vcursor)
      return;
   
   vcursor = (uint32_t *)evas_object_image_data_get(cps->vcursor, 1);
   if (!vcursor)
      return;
   
   c = vcursor;
   yratio = 255.0 / (cps->map_height - 1);
   for (i = 0; i < cps->map_height; i++)
   {
      Etk_Color color;
      color = etk_colorpicker_square_color_get(cps, x, i * yratio);
      color.r = 255 - color.r;
      color.g = 255 - color.g;
      color.b = 255 - color.b;

      *c = ((color.r << 16) |
         (color.g << 8) |
         (color.b));
      c++;
   }
   evas_object_image_data_update_add(cps->vcursor, 0, 0, 1, cps->map_width);
}

/* Gets the color in the H color mode */
static void _etk_colorpicker_square_color_get_h(Etk_Colorpicker_Square *cps, unsigned char x, unsigned char y, Etk_Color *color)
{
   double h, s, v;

   *color = cps->current_color;
   etk_colorpicker_rgb_to_hsv(*color, &h, &s, &v);

   s = 100.0 - y / 2.55;
   v = x / 2.55;

   etk_colorpicker_hsv_to_rgb(h, s, v, color);
}

/* Gets the color in the S color mode */
static void _etk_colorpicker_square_color_get_s(Etk_Colorpicker_Square *cps, unsigned char x, unsigned char y, Etk_Color *color)
{
   double h, s, v;
   
   *color = cps->current_color;
   etk_colorpicker_rgb_to_hsv(*color, &h, &s, &v);
   
   h = 100.0 - y / 2.55;
   v = x / 2.55;
   
   etk_colorpicker_hsv_to_rgb(h, s, v, color);
}

/* Gets the color in the V color mode */
static void _etk_colorpicker_square_color_get_v(Etk_Colorpicker_Square *cps, unsigned char x, unsigned char y, Etk_Color *color)
{
   double h, s, v;
   
   *color = cps->current_color;
   etk_colorpicker_rgb_to_hsv(*color, &h, &s, &v);
   
   h = 100.0 - y / 2.55;
   s = x / 2.55;
   
   etk_colorpicker_hsv_to_rgb(h, s, v, color);
}

/* Gets the color in the R color mode */
static void _etk_colorpicker_square_color_get_r(Etk_Colorpicker_Square *cps, unsigned char x, unsigned char y, Etk_Color *color)
{
   color->r = cps->current_color.r;
   color->g = 255 - y;
   color->b = x;
}

/* Gets the color in the G color mode */
static void _etk_colorpicker_square_color_get_g(Etk_Colorpicker_Square *cps, unsigned char x, unsigned char y, Etk_Color *color)
{
   color->r = 255 - y;
   color->g = cps->current_color.g;
   color->b = x;
}

/* Gets the color in the B color mode */
static void _etk_colorpicker_square_color_get_b(Etk_Colorpicker_Square *cps, unsigned char x, unsigned char y, Etk_Color *color)
{
  color->r = 255 - y;
  color->g = x;
  color->b = cps->current_color.b;
}

/* Resizes the map and the cursor images */
static void _etk_colorpicker_square_resize_map_cursor(Etk_Colorpicker_Square *cps)
{
   if (!cps)
      return;

   if (cps->map)
      evas_object_image_size_set(cps->map, cps->map_width, cps->map_height);
   if (cps->hcursor)
      evas_object_image_size_set(cps->hcursor, cps->map_width, 1);
   if (cps->vcursor)
      evas_object_image_size_set(cps->vcursor, 1, cps->map_height);
}

/** @} */
