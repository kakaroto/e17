/** @file etk_colorpicker.c */
#include "etk_colorpicker.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"
#include "etk_box.h"
#include "etk_radio_button.h"
#include "etk_colorpicker_vertical.h"
#include "etk_colorpicker_square.h"

/**
 * @addtogroup Etk_Colorpicker
 * @{
 */

static int _etk_colorpicker_mode[6] = {
  ETK_COLOR_MODE_H, 
  ETK_COLOR_MODE_S,
  ETK_COLOR_MODE_V,
  ETK_COLOR_MODE_R,
  ETK_COLOR_MODE_G,
  ETK_COLOR_MODE_B
};

enum _Etk_Cp_Property_Id
{
   ETK_CP_COLOR_MODE_PROPERTY
};

static void _etk_colorpicker_constructor(Etk_Colorpicker *cp);
static void _etk_colorpicker_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_colorpicker_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);

static void _etk_colorpicker_cps_cb(Etk_Object *object, void *data);
static void _etk_colorpicker_cpv_cb(Etk_Object *object, void *data);
static void _etk_colorpicker_radio_cb(Etk_Object *object, void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Colorpicker
 * @return Returns the type on an Etk_Colorpicker
 */
Etk_Type *etk_colorpicker_type_get()
{
   static Etk_Type *cp_type = NULL;

   if (!cp_type)
   {
      cp_type = etk_type_new("Etk_Colorpicker", ETK_HBOX_TYPE, sizeof(Etk_Colorpicker), ETK_CONSTRUCTOR(_etk_colorpicker_constructor), NULL);

      etk_type_property_add(cp_type, "color_mode", ETK_CP_COLOR_MODE_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_COLOR_MODE_H));

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
   return etk_widget_new(ETK_COLORPICKER_TYPE, NULL);
}

/**
 * @brief Sets the color mode used by the colorpicker
 * @param cp a colorpicker
 * @param color_mode the color mode to use
 */
void etk_colorpicker_color_mode_set(Etk_Colorpicker *cp, Etk_Color_Mode color_mode)
{
   if (!cp)
      return;

   if (cp->color_mode != color_mode)
   {
      Etk_Colorpicker_Square *cps;
      Etk_Colorpicker_Vertical *cpv;
      
      cps = ETK_COLORPICKER_SQUARE(cp->cps);
      cpv = ETK_COLORPICKER_VERTICAL(cp->cpv);
      
      cp->color_mode = color_mode;
      cps->current_color = cp->color;
        
      switch (cp->color_mode)
      {
         case ETK_COLOR_MODE_H:
         {
            double h, s, v;
           
            etk_colorpicker_rgb_to_hsv(cp->color, &h, &s, &v);
            cpv->cursor_pos = 255 - h * 2.55;
            cps->cursor_y = 255 - s * 2.55;
            cps->cursor_x = v * 2.55;
            break;
         }
         case ETK_COLOR_MODE_S:
         {
            double h, s, v;

            etk_colorpicker_rgb_to_hsv(cp->color, &h, &s, &v);
            cpv->cursor_pos = 255 - s * 2.55;
            cps->cursor_y = 255 - h * 2.55;
            cps->cursor_x = v * 2.55;
            break;
         }
         case ETK_COLOR_MODE_V:
         {
            double h, s, v;
            
            etk_colorpicker_rgb_to_hsv(cp->color, &h, &s, &v);
            cpv->cursor_pos = 255 - v * 2.55;
            cps->cursor_y = 255 - h * 2.55;
            cps->cursor_x = s * 2.55;
            break;
         }
         case ETK_COLOR_MODE_R:
         {
            cpv->cursor_pos = 255 - cp->color.r;
            cps->cursor_y = 255 - cp->color.g;
            cps->cursor_x = cp->color.b;
            break;
         }
         case ETK_COLOR_MODE_G:
         {
            cpv->cursor_pos = 255 - cp->color.g;
            cps->cursor_y = 255 - cp->color.r;
            cps->cursor_x = cp->color.b;
            break;
         }
         case ETK_COLOR_MODE_B:
         {
            cpv->cursor_pos = 255 - cp->color.b;
            cps->cursor_y = 255 - cp->color.r;
            cps->cursor_x = cp->color.g;
            break;
         }
      }
      etk_colorpicker_vertical_color_mode_set(ETK_COLORPICKER_VERTICAL(cp->cpv), cp->color_mode);
      etk_colorpicker_square_color_mode_set(ETK_COLORPICKER_SQUARE(cp->cps), cp->color_mode);
      etk_object_notify(ETK_OBJECT(cp), "color_mode");
   }
}

/**
 * @brief Gets the color mode used by the colorpicker
 * @param cp a colorpicker
 * @return Returns the color mode used by the colorpicker
 */
Etk_Color_Mode etk_colorpicker_color_mode_get(Etk_Colorpicker *cp)
{
   if (!cp)
      return ETK_COLOR_MODE_H;
   return cp->color_mode;
}

/**
 * @brief Gets the color selected by the colorpicker
 * @param cp a colorpicker
 * @return Returns the color selected by the colorpicker
 */
Etk_Color etk_colorpicker_color_get(Etk_Colorpicker *cp)
{
   if (!cp)
   {
      Etk_Color black;
      black.a = 0;
      black.r = 0;
      black.g = 0;
      black.b = 0;
      return black;
   }
   return cp->color;
}

/**
 * @brief Converts from rgb to hsv
 * @param color the rgb color
 * @param h the location to store the h component of the result
 * @param s the location to store the s component of the result
 * @param v the location to store the v component of the result
 */
void etk_colorpicker_rgb_to_hsv(Etk_Color color, double *h, double *s, double *v)
{
   int min, max;
   int delta;
   
   max = (color.r + color.g + abs(color.r - color.g)) / 2;
   max = (max + color.b + abs(max - color.b)) / 2;
   min = (color.r + color.g - abs(color.r - color.g)) / 2;
   min = (min + color.b - abs(min - color.b)) / 2;
   
   delta = max - min;
   *v = (double)(100 * max) / 255.0;
   
   if (max != 0)
      *s = (double)(100 * delta) / (double)max;
   else
   {
      *s = 0.0;
      *h = 0.0;
      *v = 0.0;
   }  
   if (color.r == max)
   {
      *h = (double)(100 * (color.g - color.b)) / (double)(6.0 * delta);
   }
   else
   {
      if (color.g == max)
         *h = (double)(100 * (2 * delta + color.b - color.r)) / (double)(6.0 * delta);
      else
         *h = (double)(100 * (4 * delta + color.r - color.g)) / (double)(6.0 * delta);
   }
   if (*h < 0.0)
      *h += 100.0;
   if (*h > 100.0)
      *h -= 100.0;
}

/**
 * @brief Converts from hsv to rgb
 * @param h the h component of the color to convert
 * @param s the s component of the color to convert
 * @param v the v component of the color to convert
 * @param color the location to store the resulting rgb color
 */
void etk_colorpicker_hsv_to_rgb(double h, double s, double v, Etk_Color *color)
{
   double hh, f;
   double p, q, t;
   int i;
   
   h = fabs(h);
   s = fabs(s);
   v = fabs(v);

   if (s == 0.0)
   {
      color->r = ETK_ROUND((v * 255.0) / 100.0);
      color->g = ETK_ROUND((v * 255.0) / 100.0);
      color->b = ETK_ROUND((v * 255.0) / 100.0);
      return;
   }
   
   hh = (h * 6.0) / 100.0;
   i = floor(hh);
   f = hh - (double)i;
   
   p = fabs(v * (1.0 - s / 100.0) / 100.0);
   q = fabs(v * (1.0 - (s * f) / 100.0) / 100.0);
   t = fabs(v * (1.0 - s * (1.0 - f) / 100.0) / 100.0);
   switch (i)
   {
      case 0:
      {
         color->r = ETK_ROUND(v * 255.0 / 100.0);
         color->g = ETK_ROUND(t * 255.0);
         color->b = ETK_ROUND(p * 255.0);
         break;
      }
      case 1:
      {
         color->r = ETK_ROUND(q * 255.0);
         color->g = ETK_ROUND(v * 255.0 / 100.0);
         color->b = ETK_ROUND(p * 255.0);
         break;
      }
      case 2:
      {
         color->r = ETK_ROUND(p * 255.0);
         color->g = ETK_ROUND(v * 255.0 / 100.0);
         color->b = ETK_ROUND(t * 255.0);
         break;
      }
      case 3:
      {
         color->r = ETK_ROUND(p * 255.0);
         color->g = ETK_ROUND(q * 255.0);
         color->b = ETK_ROUND(v * 255.0 / 100.0);
         break;
      }
      case 4:
      {
         color->r = ETK_ROUND(t * 255.0);
         color->g = ETK_ROUND(p * 255.0);
         color->b = ETK_ROUND(v * 255.0 / 100.0);
         break;
      }
      case 5:
      {
         color->r = ETK_ROUND(v * 255.0 / 100.0);
         color->g = ETK_ROUND(p * 255.0);
         color->b = ETK_ROUND(q * 255.0);
         break;
      }
   }
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_colorpicker_constructor(Etk_Colorpicker *cp)
{
   Etk_Widget *widget;
   Etk_Widget *vbox;
   char *label[6] = {"H", "S", "V", "R", "G", "B"};
   int i;

   if (!(widget = ETK_WIDGET(cp)))
      return;

   cp->cps = NULL;
   cp->cpv = NULL;
   cp->color_mode = ETK_COLOR_MODE_H;
   cp->color.r = 0;
   cp->color.g = 0;
   cp->color.b = 0;
   etk_box_spacing_set(ETK_BOX(cp), 6);

   cp->cps = etk_colorpicker_square_new(64, 64);
   if (cp->cps)
   {
      etk_widget_visibility_locked_set(cp->cps, ETK_TRUE);
      etk_widget_size_request_set(cp->cps, 256, 256);
      etk_box_pack_start(ETK_BOX(cp), cp->cps, ETK_FALSE, ETK_FALSE, 0);
      etk_signal_connect("color_selected", ETK_OBJECT(cp->cps), ETK_CALLBACK(_etk_colorpicker_cps_cb), NULL);
      etk_widget_show(cp->cps);
   }

   cp->cpv = etk_colorpicker_vertical_new(1, 256);
   if (cp->cpv)
   {
      etk_widget_visibility_locked_set(cp->cpv, ETK_TRUE);
      etk_widget_size_request_set(cp->cpv, 16, 256);
      etk_box_pack_start(ETK_BOX(cp), cp->cpv, ETK_FALSE, ETK_FALSE, 0);
      etk_signal_connect("color_selected", ETK_OBJECT(cp->cpv), ETK_CALLBACK(_etk_colorpicker_cpv_cb), NULL);
      etk_widget_show(cp->cpv);
   }

   vbox = etk_vbox_new(0, ETK_TRUE);
   etk_widget_visibility_locked_set(vbox, ETK_TRUE);
   etk_box_pack_start(ETK_BOX(cp), vbox, ETK_FALSE, ETK_FALSE, 0);
   etk_widget_show(vbox);
   for (i = 0; i < 6; i++)
   {
      if (i == 0)
         cp->radio[i] = etk_radio_button_new_with_label(label[i], NULL);
      else
         cp->radio[i] = etk_radio_button_new_with_label_from_widget(label[i], ETK_RADIO_BUTTON(cp->radio[0]));
      etk_widget_visibility_locked_set(cp->radio[i], ETK_TRUE);
      etk_box_pack_start(ETK_BOX(vbox), cp->radio[i], ETK_TRUE, ETK_TRUE, 0);
      etk_widget_show(cp->radio[i]);

      etk_signal_connect("pressed", ETK_OBJECT(cp->radio[i]), ETK_CALLBACK(_etk_colorpicker_radio_cb), &_etk_colorpicker_mode[i]);
   }
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_colorpicker_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Colorpicker *cp;

   if (!(cp = ETK_COLORPICKER(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_CP_COLOR_MODE_PROPERTY:
         etk_colorpicker_color_mode_set(cp, etk_property_value_int_get(value));
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
      case ETK_CP_COLOR_MODE_PROPERTY:
         etk_property_value_int_set(value, cp->color_mode);
         break;
      default:
         break;
   }
}

/* Called when a color is selected with the cps */
static void _etk_colorpicker_cps_cb(Etk_Object *object, void *data)
{
   Etk_Colorpicker *cp;
   
   cp = ETK_COLORPICKER(ETK_WIDGET(object)->parent);
   cp->color = etk_colorpicker_square_color_get(ETK_COLORPICKER_SQUARE(object), ETK_COLORPICKER_SQUARE(object)->cursor_x, ETK_COLORPICKER_SQUARE(object)->cursor_y);
}

/* Called when a color is selected with the cpv */
static void _etk_colorpicker_cpv_cb(Etk_Object *object, void *data)
{
   Etk_Colorpicker *cp;
   Etk_Colorpicker_Square *cps;
   Etk_Colorpicker_Vertical *cpv;
   Etk_Color color;
   
   cp = ETK_COLORPICKER(ETK_WIDGET(object)->parent);
   cps = ETK_COLORPICKER_SQUARE(cp->cps);
   cpv = ETK_COLORPICKER_VERTICAL(cp->cpv);

   color = etk_colorpicker_vertical_color_get(cpv, cpv->cursor_pos);

   switch (cp->color_mode)
   {
      case ETK_COLOR_MODE_H:
      {
         double h, s, v;
         double h1, s1, v1;

         etk_colorpicker_rgb_to_hsv(cp->color, &h1, &s1, &v1);
         etk_colorpicker_rgb_to_hsv(color, &h, &s, &v);
         etk_colorpicker_hsv_to_rgb(h, s1, v1, &color);
         if (cp->color.r != color.r ||
            cp->color.g != color.g ||
            cp->color.b != color.b)
         {
            cp->color = color;
            cps->current_color = color;
         }
         break;
      }
      case ETK_COLOR_MODE_S:
      {
         double h, s, v;
         double h1, s1, v1;
      
         etk_colorpicker_rgb_to_hsv(cp->color, &h1, &s1, &v1);
         etk_colorpicker_rgb_to_hsv(color, &h, &s, &v);
         etk_colorpicker_hsv_to_rgb(h1, s, v1, &color);
         if (cp->color.r != color.r ||
            cp->color.g != color.g ||
            cp->color.b != color.b)
         {
            cp->color = color;
            cps->current_color = color;
         }
         break;
      }
      case ETK_COLOR_MODE_V:
      {
         double h, s, v;
         double h1, s1, v1;
      
         etk_colorpicker_rgb_to_hsv(cp->color, &h1, &s1, &v1);
         etk_colorpicker_rgb_to_hsv(color, &h, &s, &v);
         etk_colorpicker_hsv_to_rgb(h1, s1, v, &color);
         if (cp->color.r != color.r ||
            cp->color.g != color.g ||
            cp->color.b != color.b)
         {
            cp->color = color;
            cps->current_color = color;
         }
         break;
      }
      case ETK_COLOR_MODE_R:
      {
         double h, s, v;
      
         if (cp->color.r != color.r)
         {
            cp->color.r = color.r;
            cps->current_color.r = color.r;
            etk_colorpicker_rgb_to_hsv(cp->color, &h, &s, &v);
         }
         break;
      }
      case ETK_COLOR_MODE_G:
      {
         double h, s, v;
      
         if (cp->color.g != color.g)
         {
            cp->color.g = color.g;
            cps->current_color.g = color.g;
            etk_colorpicker_rgb_to_hsv(cp->color, &h, &s, &v);
         }
         break;
      }
      case ETK_COLOR_MODE_B:
      {
         double h, s, v;
      
         if (cp->color.b != color.b)
         {
            cp->color.b = color.b;
            cps->current_color.b = color.b;
            etk_colorpicker_rgb_to_hsv(cp->color, &h, &s, &v);
         }
         break;
      }
   }
   etk_colorpicker_square_update(cps);
}

/* Called when the color mode is changed with the radio buttons */
static void _etk_colorpicker_radio_cb(Etk_Object *object, void *data)
{
   Etk_Colorpicker *cp;
   Etk_Widget *w;
   Etk_Color_Mode color_mode = *(Etk_Color_Mode *)data;
   
   w = ETK_WIDGET(ETK_WIDGET(object)->parent);
   cp = ETK_COLORPICKER(w->parent);
   etk_colorpicker_color_mode_set(cp, color_mode);
}

/** @} */
