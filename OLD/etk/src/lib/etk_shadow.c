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

/** @file etk_shadow.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_shadow.h"

#include <stdlib.h>
#include <stdint.h>

#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Shadow
 * @{
 */

#define EDGE_SIZE 5
#define MAX_RADIUS 255
#define BLUR_SIZE_GET(shadow)   (_etk_shadow_num_gaussian_values * ((shadow)->radius) / MAX_RADIUS)
#define IS_HORIZONTAL(i)        (((i) % 2) == 1)

#define PIXEL_GET(siz, pix) \
   (pixels[ETK_CLAMP((shadow->type == ETK_SHADOW_OUTSIDE) ? (pix) : (siz) - 1 - (pix), 0, blur_size - 1)])

#define PIXEL_COMBINE(pix1, pix2) \
   ((shadow->type == ETK_SHADOW_OUTSIDE) ? (((pix1) * (pix2)) / 255) : \
   ETK_MIN(255, (pix1) + (((pix2) * (255 - (pix1))) / 255)))


typedef enum Etk_Shadow_Object_Id
{
   ETK_SHADOW_LEFT_OBJECT,
   ETK_SHADOW_TOP_OBJECT,
   ETK_SHADOW_RIGHT_OBJECT,
   ETK_SHADOW_BOTTOM_OBJECT
} Etk_Shadow_Object_Id;

enum Etk_Shadow_Property_Id
{
   ETK_SHADOW_SHADOW_TYPE_PROPERTY,
   ETK_SHADOW_SHADOW_EDGES_PROPERTY,
   ETK_SHADOW_SHADOW_COLOR_PROPERTY,
   ETK_SHADOW_SHADOW_OFFSET_X_PROPERTY,
   ETK_SHADOW_SHADOW_OFFSET_Y_PROPERTY,
   ETK_SHADOW_SHADOW_RADIUS_PROPERTY,
   ETK_SHADOW_SHADOW_OPACITY_PROPERTY,
   ETK_SHADOW_BORDER_PROPERTY,
   ETK_SHADOW_BORDER_COLOR_PROPERTY
};

static void _etk_shadow_constructor(Etk_Shadow *shadow);
static void _etk_shadow_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_shadow_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_shadow_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_shadow_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static Etk_Bool _etk_shadow_realized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_shadow_unrealized_cb(Etk_Object *object, void *data);
static void _etk_shadow_shadow_recalc(Etk_Shadow *shadow);
static void _etk_shadow_border_recalc(Etk_Shadow *shadow);
static Etk_Bool _etk_shadow_edge_visible(Etk_Shadow *shadow, Etk_Shadow_Object_Id object_id);

static int _etk_shadow_gaussian_values[] = {
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5,
   5, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9,
   9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14,
   14, 15, 15, 16, 16, 17, 17, 17, 18, 18, 19, 19, 20, 20, 21,
   21, 22, 22, 23, 24, 24, 25, 25, 26, 26, 27, 28, 28, 29, 30,
   30, 31, 32, 32, 33, 34, 35, 35, 36, 37, 38, 38, 39, 40, 41,
   42, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50, 51, 52, 53, 54,
   55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68, 69, 70,
   71, 72, 73, 74, 76, 77, 78, 79, 80, 81, 83, 84, 85, 86, 88,
   89, 90, 91, 93, 94, 95, 96, 98, 99, 100, 101, 103, 104, 105,
   107, 108, 109, 110, 112, 113, 114, 116, 117, 118, 120, 121,
   122, 124, 125, 126, 128, 129, 130, 132, 133, 134, 136, 137,
   138, 140, 141, 142, 144, 145, 146, 147, 149, 150, 151, 153,
   154, 155, 156, 158, 159, 160, 161, 163, 164, 165, 166, 168,
   169, 170, 171, 173, 174, 175, 176, 177, 178, 180, 181, 182,
   183, 184, 185, 186, 187, 188, 190, 191, 192, 193, 194, 195,
   196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 205, 206,
   207, 208, 209, 210, 211, 212, 212, 213, 214, 215, 216, 216,
   217, 218, 219, 219, 220, 221, 222, 222, 223, 224, 224, 225,
   226, 226, 227, 228, 228, 229, 229, 230, 230, 231, 232, 232,
   233, 233, 234, 234, 235, 235, 236, 236, 237, 237, 237, 238,
   238, 239, 239, 240, 240, 240, 241, 241, 241, 242, 242, 242,
   243, 243, 243, 244, 244, 244, 245, 245, 245, 245, 246, 246,
   246, 246, 247, 247, 247, 247, 248, 248, 248, 248, 248, 249,
   249, 249, 249, 249, 250, 250, 250, 250, 250, 250, 250, 251,
   251, 251, 251, 251, 251, 251, 251, 252, 252, 252, 252, 252,
   252, 252, 252, 252, 252, 252, 253, 253, 253, 253, 253, 253,
   253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 254,
   254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
   254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
   254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
   254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
   254, 254, 254, 254, 254, 254, 254, 255
};

static int _etk_shadow_num_gaussian_values = sizeof(_etk_shadow_gaussian_values) / sizeof(int);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Shadow
 * @return Returns the type of an Etk_Shadow
 */
Etk_Type *etk_shadow_type_get(void)
{
   static Etk_Type *shadow_type = NULL;

   if (!shadow_type)
   {
      shadow_type = etk_type_new("Etk_Shadow", ETK_BIN_TYPE, sizeof(Etk_Shadow),
         ETK_CONSTRUCTOR(_etk_shadow_constructor), NULL, NULL);

      etk_type_property_add(shadow_type, "shadow-type", ETK_SHADOW_SHADOW_TYPE_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_SHADOW_NONE));
      etk_type_property_add(shadow_type, "shadow-edges", ETK_SHADOW_SHADOW_EDGES_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_SHADOW_ALL));
      etk_type_property_add(shadow_type, "shadow-color", ETK_SHADOW_SHADOW_COLOR_PROPERTY,
         ETK_PROPERTY_OTHER, ETK_PROPERTY_NO_ACCESS, NULL);
      etk_type_property_add(shadow_type, "shadow-offset-x", ETK_SHADOW_SHADOW_OFFSET_X_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(0));
      etk_type_property_add(shadow_type, "shadow-offset-y", ETK_SHADOW_SHADOW_OFFSET_Y_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(0));
      etk_type_property_add(shadow_type, "shadow-radius", ETK_SHADOW_SHADOW_RADIUS_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(20));
      etk_type_property_add(shadow_type, "shadow-opacity", ETK_SHADOW_SHADOW_OPACITY_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(155));
      etk_type_property_add(shadow_type, "border", ETK_SHADOW_BORDER_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(0));
      etk_type_property_add(shadow_type, "border-color", ETK_SHADOW_BORDER_COLOR_PROPERTY,
         ETK_PROPERTY_OTHER, ETK_PROPERTY_NO_ACCESS, NULL);

      shadow_type->property_set = _etk_shadow_property_set;
      shadow_type->property_get = _etk_shadow_property_get;
   }

   return shadow_type;
}

/**
 * @brief Creates a new shadow container
 * @return Returns the new shadow container
 */
Etk_Widget *etk_shadow_new(void)
{
   return etk_widget_new(ETK_SHADOW_TYPE, "theme-group", "shadow", NULL);
}

/**
 * @brief Sets the different settings of the shadow cast by the shadow container
 * @param shadow a shadow container
 * @param type the type of shadow to cast: ETK_SHADOW_NONE to cast no shadow, ETK_SHADOW_OUTSIDE to cast the shadow
 * outside of the container, ETK_SHADOW_INSIDE to cast the shadow inside the container on the child.
 * @param edges the edges of the container where the shadow should be cast (ETK_SHADOW_ALL most of the time)
 * @param radius the blur radius of the shadow to cast, from 0 to 255
 * @param offset_x the horizontal offset of the shadow
 * @param offset_y the vertical offset of the shadow
 * @param opacity the opacity of the shadow to cast, from 0 (totally transparent) to 255 (totally opaque)
 */
void etk_shadow_shadow_set(Etk_Shadow *shadow, Etk_Shadow_Type type, Etk_Shadow_Edges edges, int radius, int offset_x, int offset_y, int opacity)
{
   if (!shadow)
      return;

   if (shadow->type != type)
   {
      shadow->type = type;
      etk_object_notify(ETK_OBJECT(shadow), "shadow-type");
   }
   if (shadow->edges != edges)
   {
      shadow->edges = edges;
      etk_object_notify(ETK_OBJECT(shadow), "shadow-edges");
   }
   if (shadow->radius != ETK_CLAMP(radius, 0, 255))
   {
      shadow->radius = ETK_CLAMP(radius, 0, 255);
      etk_object_notify(ETK_OBJECT(shadow), "shadow-radius");
   }
   if (shadow->offset_x != offset_x)
   {
      shadow->offset_x = offset_x;
      etk_object_notify(ETK_OBJECT(shadow), "shadow-offset-x");
   }
   if (shadow->offset_y != offset_y)
   {
      shadow->offset_y = offset_y;
      etk_object_notify(ETK_OBJECT(shadow), "shadow-offset-y");
   }
   if (shadow->color.a != ETK_CLAMP(opacity, 0, 255))
   {
      shadow->color.a = ETK_CLAMP(opacity, 0, 255);
      etk_object_notify(ETK_OBJECT(shadow), "shadow-opacity");
   }

   shadow->shadow_need_recalc = ETK_TRUE;
   etk_widget_size_recalc_queue(ETK_WIDGET(shadow));
}

/**
 * @brief Gets the different settings of the shadow cast by the shadow container
 * @param shadow a shadow container
 * @param type the location where to store the type of the cast shadow
 * @param edges the location where to store the edges where the shadow is cast
 * @param radius the location where to store the blur radius of the cast shadow
 * @param offset_x the location where to store the horizontal offset of the shadow
 * @param offset_y the location where to store the vertical offset of the shadow
 * @param opacity the location where to store the opacity of the shadow
 */
void etk_shadow_shadow_get(Etk_Shadow *shadow, Etk_Shadow_Type *type, Etk_Shadow_Edges *edges, int *radius, int *offset_x, int *offset_y, int *opacity)
{
   if (!shadow)
      return;

   if (type)       *type = shadow->type;
   if (edges)      *edges = shadow->edges;
   if (radius)     *radius = shadow->radius;
   if (offset_x)   *offset_x = shadow->offset_x;
   if (offset_y)   *offset_y = shadow->offset_y;
   if (opacity)    *opacity = shadow->color.a;
}

/**
 * @brief Sets the color of the shadow
 * @param shadow a shadow container
 * @param r the red component of the color to set, from 0 to 255
 * @param g the green component of the color to set, from 0 to 255
 * @param b the blue component of the color to set, from 0 to 255
 */
void etk_shadow_shadow_color_set(Etk_Shadow *shadow, int r, int g, int b)
{
   int i;

   if (!shadow)
      return;

   shadow->color.r = ETK_CLAMP(r, 0, 255);
   shadow->color.g = ETK_CLAMP(g, 0, 255);
   shadow->color.b = ETK_CLAMP(b, 0, 255);

   evas_color_argb_premul(shadow->color.a, &r, &g, &b);
   for (i = 0; i < 4; i++)
   {
      if (shadow->shadow_objs[i])
      {
         evas_object_color_set(shadow->shadow_objs[i], shadow->color.r,
            shadow->color.g, shadow->color.b, shadow->color.a);
      }
   }

   etk_object_notify(ETK_OBJECT(shadow), "shadow-color");
}

/**
 * @brief Gets the color of the shadow
 * @param shadow a shadow container
 * @param r the location where to store the red component of the color
 * @param g the location where to store the green component of the color
 * @param b the location where to store the blue component of the color
 */
void etk_shadow_shadow_color_get(Etk_Shadow *shadow, int *r, int *g, int *b)
{
   if (!shadow)
      return;

   if (r)   *r = shadow->color.r;
   if (g)   *g = shadow->color.g;
   if (b)   *b = shadow->color.b;
}

/**
 * @brief Sets the width of the border of the shadow container
 * @param shadow a shadow container
 * @param border_width the width of the border. If it is <= 0, the border is disabled
 */
void etk_shadow_border_set(Etk_Shadow *shadow, int border_width)
{
   if (!shadow || shadow->border_width == ETK_MAX(0, border_width))
      return;

   shadow->border_width = ETK_MAX(0, border_width);
   etk_object_notify(ETK_OBJECT(shadow), "border");

   shadow->border_need_recalc = ETK_TRUE;
   etk_widget_size_recalc_queue(ETK_WIDGET(shadow));
}

/**
 * @brief Gets the width of the border of the shadow container
 * @param shadow a shadow container
 * @return Returns the width of the border of the shadow container, 0 meaning it has no border
 */
int etk_shadow_border_get(Etk_Shadow *shadow)
{
   if (!shadow)
      return 0;
   return shadow->border_width;
}

/**
 * @brief Sets the color of the border of the shadow container. The color has to be pre-multiplied
 * (i.e. the @a r, @a g, @a b components have to be multiplied by @a a and divided by 255)
 * @param shadow a shadow container
 * @param r the red component of the border, from 0 to 255
 * @param g the green component of the border, from 0 to 255
 * @param b the blue component of the border, from 0 to 255
 * @param a the alpha component of the border, from 0 to 255
 */
void etk_shadow_border_color_set(Etk_Shadow *shadow, int r, int g, int b, int a)
{
   int i;

   if (!shadow)
      return;

   shadow->border_color.r = ETK_CLAMP(r, 0, 255);
   shadow->border_color.g = ETK_CLAMP(g, 0, 255);
   shadow->border_color.b = ETK_CLAMP(b, 0, 255);
   shadow->border_color.a = ETK_CLAMP(a, 0, 255);

   for (i = 0; i < 4; i++)
   {
      if (shadow->border_objs[i])
      {
         evas_object_color_set(shadow->border_objs[i], shadow->border_color.r,
            shadow->border_color.g, shadow->border_color.b, shadow->border_color.a);
      }
   }

   etk_object_notify(ETK_OBJECT(shadow), "border-color");
}

/**
 * @brief Gets the color of the border of the shadow container
 * @param shadow a shadow container
 * @param r the location where to store the red component of the border
 * @param g the location where to store the green component of the border
 * @param b the location where to store the blue component of the border
 * @param a the location where to store the alpha component of the border
 */
void etk_shadow_border_color_get(Etk_Shadow *shadow, int *r, int *g, int *b, int *a)
{
   if (!shadow)
      return;

   if (r)   *r = shadow->border_color.r;
   if (g)   *g = shadow->border_color.g;
   if (b)   *b = shadow->border_color.b;
   if (a)   *a = shadow->border_color.a;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the shadow container */
static void _etk_shadow_constructor(Etk_Shadow *shadow)
{
   int i;

   if (!shadow)
      return;

   shadow->type = ETK_SHADOW_NONE;
   shadow->edges = ETK_SHADOW_ALL;
   shadow->offset_x = 0;
   shadow->offset_y = 0;
   shadow->radius = 20;
   shadow->color.r = -1;
   shadow->color.g = -1;
   shadow->color.b = -1;
   shadow->color.a = -1;
   shadow->border_width = 0;
   shadow->border_color.r = -1;
   shadow->border_color.g = -1;
   shadow->border_color.b = -1;
   shadow->border_color.a = -1;

   shadow->shadow_need_recalc = ETK_FALSE;
   shadow->border_need_recalc = ETK_FALSE;

   shadow->clip = NULL;
   for (i = 0; i < 4; i++)
   {
      shadow->shadow_objs[i] = NULL;
      shadow->border_objs[i] = NULL;
   }

   ETK_WIDGET(shadow)->size_request = _etk_shadow_size_request;
   ETK_WIDGET(shadow)->size_allocate = _etk_shadow_size_allocate;

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(shadow), ETK_CALLBACK(_etk_shadow_realized_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_UNREALIZED_SIGNAL, ETK_OBJECT(shadow), ETK_CALLBACK(_etk_shadow_unrealized_cb), NULL);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_shadow_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Shadow *shadow;

   if (!(shadow = ETK_SHADOW(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_SHADOW_SHADOW_TYPE_PROPERTY:
         etk_shadow_shadow_set(shadow, etk_property_value_int_get(value), shadow->edges,
            shadow->radius, shadow->offset_x, shadow->offset_y, shadow->color.a);
         break;
      case ETK_SHADOW_SHADOW_EDGES_PROPERTY:
         etk_shadow_shadow_set(shadow, shadow->type, etk_property_value_int_get(value),
            shadow->radius, shadow->offset_x, shadow->offset_y, shadow->color.a);
         break;
      case ETK_SHADOW_SHADOW_OFFSET_X_PROPERTY:
         etk_shadow_shadow_set(shadow, shadow->type, shadow->edges, shadow->radius,
            etk_property_value_int_get(value), shadow->offset_y, shadow->color.a);
         break;
      case ETK_SHADOW_SHADOW_OFFSET_Y_PROPERTY:
         etk_shadow_shadow_set(shadow, shadow->type, shadow->edges, shadow->radius,
            shadow->offset_x, etk_property_value_int_get(value), shadow->color.a);
         break;
      case ETK_SHADOW_SHADOW_RADIUS_PROPERTY:
         etk_shadow_shadow_set(shadow, shadow->type, shadow->edges, etk_property_value_int_get(value),
            shadow->offset_x, shadow->offset_y, shadow->color.a);
         break;
      case ETK_SHADOW_SHADOW_OPACITY_PROPERTY:
         etk_shadow_shadow_set(shadow, shadow->type, shadow->edges, shadow->radius,
            shadow->offset_x, shadow->offset_y, etk_property_value_int_get(value));
         break;
      case ETK_SHADOW_BORDER_PROPERTY:
         etk_shadow_border_set(shadow, etk_property_value_int_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_shadow_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Shadow *shadow;

   if (!(shadow = ETK_SHADOW(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_SHADOW_SHADOW_TYPE_PROPERTY:
         etk_property_value_int_set(value, shadow->type);
         break;
      case ETK_SHADOW_SHADOW_EDGES_PROPERTY:
         etk_property_value_int_set(value, shadow->edges);
         break;
      case ETK_SHADOW_SHADOW_OFFSET_X_PROPERTY:
         etk_property_value_int_set(value, shadow->offset_x);
         break;
      case ETK_SHADOW_SHADOW_OFFSET_Y_PROPERTY:
         etk_property_value_int_set(value, shadow->offset_y);
         break;
      case ETK_SHADOW_SHADOW_RADIUS_PROPERTY:
         etk_property_value_int_set(value, shadow->radius);
         break;
      case ETK_SHADOW_SHADOW_OPACITY_PROPERTY:
         etk_property_value_int_set(value, shadow->color.a);
         break;
      case ETK_SHADOW_BORDER_PROPERTY:
         etk_property_value_int_set(value, shadow->border_width);
         break;
      default:
         break;
   }
}

/* Calculates the ideal size of the shadow container */
static void _etk_shadow_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_Shadow *shadow;
   Etk_Widget *child;
   int container_width;

   if (!(shadow = ETK_SHADOW(widget)) || !size)
      return;

   if (shadow->type == ETK_SHADOW_OUTSIDE)
   {
      size->w = 2 * shadow->radius;
      size->h = 2 * shadow->radius;
   }
   else
   {
      size->w = 0;
      size->h = 0;
   }

   container_width = etk_container_border_width_get(ETK_CONTAINER(shadow));
   size->w += 2 * (shadow->border_width + container_width);
   size->h += 2 * (shadow->border_width + container_width);

   if ((child = etk_bin_child_get(ETK_BIN(shadow))))
   {
      Etk_Size child_size;

      etk_widget_size_request(child, &child_size);
      size->w += child_size.w;
      size->h += child_size.h;
   }
}

/* Resizes the shadow container to the allocated size */
static void _etk_shadow_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Shadow *shadow;
   Etk_Widget *child;
   Evas_Object *obj;

   if (!(shadow = ETK_SHADOW(widget)))
      return;

   evas_object_move(shadow->clip, geometry.x, geometry.y);
   evas_object_resize(shadow->clip, geometry.w, geometry.h);

   if (shadow->shadow_need_recalc)
      _etk_shadow_shadow_recalc(shadow);
   if (shadow->border_need_recalc)
      _etk_shadow_border_recalc(shadow);

   /* Render the shadow */
   if (shadow->type != ETK_SHADOW_NONE)
   {
      Etk_Geometry geometries[4];
      int offsets[4];
      int i, j;

      offsets[ETK_SHADOW_LEFT_OBJECT] = -shadow->offset_x;
      offsets[ETK_SHADOW_RIGHT_OBJECT] = shadow->offset_x;
      offsets[ETK_SHADOW_TOP_OBJECT] = -shadow->offset_y;
      offsets[ETK_SHADOW_BOTTOM_OBJECT] = shadow->offset_y;
      if (shadow->type == ETK_SHADOW_INSIDE)
      {
         for (i = 0; i < 4; i++)
            offsets[i] = -offsets[i];
      }

      /* Calculate the geometry of the shadow objects */
      for (i = 0; i < 4; i++)
      {
         geometries[i].x = geometry.x;
         geometries[i].y = geometry.y;
         geometries[i].w = 0;
         geometries[i].h = 0;
      }
      for (i = 0; i < 4; i++)
      {
         if (shadow->shadow_objs[i])
         {
            if (IS_HORIZONTAL(i))
            {
               geometries[i].x = geometry.x;
               geometries[i].w = geometry.w;
               geometries[i].h = shadow->radius + offsets[i];
               if (i == ETK_SHADOW_TOP_OBJECT)
                  geometries[i].y = geometry.y;
               else
                  geometries[i].y = geometry.y + geometry.h - geometries[i].h;
            }
            else
            {
               geometries[i].y = geometry.y;
               geometries[i].h = geometry.h;
               geometries[i].w = shadow->radius + offsets[i];
               if (i == ETK_SHADOW_LEFT_OBJECT)
                  geometries[i].x = geometry.x;
               else
                  geometries[i].x = geometry.x + geometry.w - geometries[i].w;
            }
         }
      }
      for (i = 0; i < 4; i++)
      {
         if (shadow->shadow_objs[i])
         {
            j = (i == 0) ? 3 : (i - 1);
            switch (i)
            {
               case ETK_SHADOW_LEFT_OBJECT:
                  geometries[i].h -= geometries[j].h;
                  break;
               case ETK_SHADOW_TOP_OBJECT:
                  geometries[i].x += geometries[j].w;
                  geometries[i].w -= geometries[j].w;
                  break;
               case ETK_SHADOW_RIGHT_OBJECT:
                  geometries[i].y += geometries[j].h;
                  geometries[i].h -= geometries[j].h;
                  break;
               case ETK_SHADOW_BOTTOM_OBJECT:
                  geometries[i].w -= geometries[j].w;
                  break;
               default:
                  break;
            }
         }
      }

      /* Render the shadow objects */
      for (i = 0; i < 4; i++)
      {
         if ((obj = shadow->shadow_objs[i]))
         {
            evas_object_move(obj, geometries[i].x, geometries[i].y);
            evas_object_resize(obj, geometries[i].w, geometries[i].h);
            evas_object_image_fill_set(obj, 0, 0, geometries[i].w, geometries[i].h);
         }
      }

      if (shadow->type == ETK_SHADOW_OUTSIDE)
      {
         geometry.x += shadow->radius - shadow->offset_x;
         geometry.y += shadow->radius - shadow->offset_y;
         geometry.w -= 2 * shadow->radius;
         geometry.h -= 2 * shadow->radius;
      }
   }


   /* Render the border */
   if (shadow->border_width > 0)
   {
      if ((obj = shadow->border_objs[ETK_SHADOW_LEFT_OBJECT]))
      {
         evas_object_move(obj, geometry.x, geometry.y);
         evas_object_resize(obj, shadow->border_width, geometry.h);
      }
      if ((obj = shadow->border_objs[ETK_SHADOW_RIGHT_OBJECT]))
      {
         evas_object_move(obj, geometry.x + geometry.w - shadow->border_width, geometry.y);
         evas_object_resize(obj, shadow->border_width, geometry.h);
      }
      if ((obj = shadow->border_objs[ETK_SHADOW_TOP_OBJECT]))
      {
         evas_object_move(obj, geometry.x + shadow->border_width, geometry.y);
         evas_object_resize(obj, geometry.w - (2 * shadow->border_width), shadow->border_width);
      }
      if ((obj = shadow->border_objs[ETK_SHADOW_BOTTOM_OBJECT]))
      {
         evas_object_move(obj, geometry.x + shadow->border_width, geometry.y + geometry.h - shadow->border_width);
         evas_object_resize(obj, geometry.w - (2 * shadow->border_width), shadow->border_width);
      }

      geometry.x += shadow->border_width;
      geometry.y += shadow->border_width;
      geometry.w -= 2 * shadow->border_width;
      geometry.h -= 2 * shadow->border_width;
   }


   /* Allocate size for the child */
   if ((child = etk_bin_child_get(ETK_BIN(shadow))))
   {
      int container_border;

      container_border = etk_container_border_width_get(ETK_CONTAINER(shadow));
      geometry.x += container_border;
      geometry.y += container_border;
      geometry.w -= 2 * container_border;
      geometry.h -= 2 * container_border;

      etk_widget_size_allocate(child, geometry);
   }
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the shadow container is realized */
static Etk_Bool _etk_shadow_realized_cb(Etk_Object *object, void *data)
{
   Etk_Shadow *shadow;
   Evas *evas;

   if (!(shadow = ETK_SHADOW(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(shadow))))
      return ETK_TRUE;

   shadow->clip = evas_object_rectangle_add(evas);
   evas_object_show(shadow->clip);
   etk_widget_member_object_add(ETK_WIDGET(shadow), shadow->clip);

   if (shadow->color.r == -1 || shadow->color.g == -1 ||
	   shadow->color.b == -1 || shadow->color.a == -1)
   {
       if (etk_widget_theme_data_get(ETK_WIDGET(shadow), "color", "%d %d %d %d",
		   &shadow->color.r, &shadow->color.g,
		   &shadow->color.b, &shadow->color.a) != 4)
       {
	   shadow->color.r = 0;
	   shadow->color.g = 0;
	   shadow->color.b = 0;
	   shadow->color.a = 155;
       }
   }

   if (shadow->border_color.r == -1 || shadow->border_color.g == -1 ||
	   shadow->border_color.b == -1 || shadow->border_color.a == -1)
   {
       if (etk_widget_theme_data_get(ETK_WIDGET(shadow),
		   "border_color", "%d %d %d %d",
		   &shadow->border_color.r, &shadow->border_color.g,
		   &shadow->border_color.b, &shadow->border_color.a) != 4)
       {
	   shadow->border_color.r = 0;
	   shadow->border_color.g = 0;
	   shadow->border_color.b = 0;
	   shadow->border_color.a = 255;
       }
   }

   shadow->shadow_need_recalc = ETK_TRUE;
   shadow->border_need_recalc = ETK_TRUE;
   return ETK_TRUE;
}

/* Called when the shadow container is unrealized */
static Etk_Bool _etk_shadow_unrealized_cb(Etk_Object *object, void *data)
{
   Etk_Shadow *shadow;
   int i;

   if (!(shadow = ETK_SHADOW(object)))
      return ETK_TRUE;

   shadow->clip = NULL;
   for (i = 0; i < 4; i++)
   {
      shadow->shadow_objs[i] = NULL;
      shadow->border_objs[i] = NULL;
   }
   return ETK_TRUE;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Recalculates the shadow objects of the shadow container */
static void _etk_shadow_shadow_recalc(Etk_Shadow *shadow)
{
   Evas *evas;
   Evas_Object *obj;
   uint32_t *pixels;
   uint32_t *data;
   int offsets[4];
   int border_prev, border_next;
   int blur_size;
   int i, j, x, y, c;
   int prev_id, next_id;
   int size, prev_size, next_size;
   Etk_Size shadow_size;
   Etk_Color color;

   if (!shadow || !shadow->shadow_need_recalc)
      return;
   if (!(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(shadow))))
      return;

   for (i = 0; i < 4; i++)
   {
      if (shadow->shadow_objs[i])
      {
         evas_object_del(shadow->shadow_objs[i]);
         shadow->shadow_objs[i] = NULL;
      }
   }

   if ((shadow->edges == ETK_SHADOW_NO_EDGE) || (blur_size = BLUR_SIZE_GET(shadow)) <= 0)
      return;

   /* Calculate the pixels of the shadow */
   pixels = malloc(blur_size * sizeof(uint32_t));
   for (i = 0; i < blur_size; i++)
   {
      j = (i * _etk_shadow_num_gaussian_values) / blur_size;
      pixels[i] = _etk_shadow_gaussian_values[ETK_CLAMP(j, 0, _etk_shadow_num_gaussian_values - 1)];
   }

   offsets[ETK_SHADOW_LEFT_OBJECT] = -shadow->offset_x;
   offsets[ETK_SHADOW_RIGHT_OBJECT] = shadow->offset_x;
   offsets[ETK_SHADOW_TOP_OBJECT] = -shadow->offset_y;
   offsets[ETK_SHADOW_BOTTOM_OBJECT] = shadow->offset_y;
   if (shadow->type == ETK_SHADOW_INSIDE)
   {
      for (i = 0; i < 4; i++)
         offsets[i] = -offsets[i];
   }

   color = shadow->color;
   evas_color_argb_premul(color.a, &color.r, &color.g, &color.b);

   for (i = 0; i < 4; i++)
   {
      if (!_etk_shadow_edge_visible(shadow, i))
         continue;

      prev_id = (i == 0) ? 3 : (i - 1);
      next_id = (i + 1) % 4;

      size = shadow->radius + offsets[i];
      prev_size = shadow->radius + offsets[prev_id];
      next_size = shadow->radius + offsets[next_id];

      /* Calculate the size of the shadow object */
      border_prev = 0;
      border_next = 0;
      if (_etk_shadow_edge_visible(shadow, prev_id))
         border_prev = ETK_MAX(blur_size - prev_size, blur_size);
      if (_etk_shadow_edge_visible(shadow, next_id))
         border_next = ETK_MAX(next_size, blur_size);

      if (IS_HORIZONTAL(i))
      {
         shadow_size.w = border_prev + border_next + EDGE_SIZE;
         shadow_size.h = size;
      }
      else
      {
         shadow_size.w = size;
         shadow_size.h = border_prev + border_next + EDGE_SIZE;
      }

      /* Create the shadow object */
      obj = evas_object_image_add(evas);
      shadow->shadow_objs[i] = obj;
      etk_widget_member_object_add(ETK_WIDGET(shadow), obj);
      evas_object_pass_events_set(obj, 1);
      evas_object_color_set(obj, color.r, color.g, color.b, color.a);
      evas_object_clip_set(obj, shadow->clip);
      evas_object_show(obj);

      evas_object_image_size_set(obj, shadow_size.w, shadow_size.h);
      evas_object_image_alpha_set(obj, 1);

      /* Calculate the pixels of the shadow */
      if ((data = evas_object_image_data_get(obj, 1)))
      {
         switch (i)
         {
            /* Left shadow object */
            case ETK_SHADOW_LEFT_OBJECT:
               evas_object_image_border_set(obj, 0, 0, border_next, border_prev);
               for (y = 0; y < shadow_size.h; y++)
               {
                  for (x = 0; x < shadow_size.w; x++)
                  {
                     c = PIXEL_GET(size, x);
                     if (y < border_next)
                        c = PIXEL_COMBINE(c, PIXEL_GET(next_size, y));
                     else if (y >= shadow_size.h - border_prev)
                        c = PIXEL_COMBINE(c, PIXEL_GET(prev_size, prev_size + shadow_size.h - y - 1));

                     *data = (c << 24) | (c << 16) | (c << 8) | c;
                     data++;
                  }
               }
               break;
            /* Top shadow object */
            case ETK_SHADOW_TOP_OBJECT:
               evas_object_image_border_set(obj, border_prev, border_next, 0, 0);
               for (y = 0; y < shadow_size.h; y++)
               {
                  for (x = 0; x < shadow_size.w; x++)
                  {
                     c = PIXEL_GET(size, y);
                     if (x < border_prev)
                        c = PIXEL_COMBINE(c, PIXEL_GET(prev_size, x + prev_size));
                     else if (x >= shadow_size.w - border_next)
                        c = PIXEL_COMBINE(c, PIXEL_GET(next_size, shadow_size.w - x - 1));

                     *data = (c << 24) | (c << 16) | (c << 8) | c;
                     data++;
                  }
               }
               break;
            /* Right shadow object */
            case ETK_SHADOW_RIGHT_OBJECT:
               evas_object_image_border_set(obj, 0, 0, border_prev, border_next);
               for (y = 0; y < shadow_size.h; y++)
               {
                  for (x = 0; x < shadow_size.w; x++)
                  {
                     c = PIXEL_GET(size, shadow_size.w - x - 1);
                     if (y < border_prev)
                        c = PIXEL_COMBINE(c, PIXEL_GET(prev_size, y + prev_size));
                     else if (y >= shadow_size.h - border_next)
                        c = PIXEL_COMBINE(c, PIXEL_GET(next_size, shadow_size.h - y - 1));

                     *data = (c << 24) | (c << 16) | (c << 8) | c;
                     data++;
                  }
               }
               break;
            /* Bottom shadow object */
            case ETK_SHADOW_BOTTOM_OBJECT:
               evas_object_image_border_set(obj, border_next, border_prev, 0, 0);
               for (y = 0; y < shadow_size.h; y++)
               {
                  for (x = 0; x < shadow_size.w; x++)
                  {
                     c = PIXEL_GET(size, shadow_size.h - y - 1);
                     if (x < border_next)
                        c = PIXEL_COMBINE(c, PIXEL_GET(next_size, x));
                     else if (x >= shadow_size.w - border_prev)
                        c = PIXEL_COMBINE(c, PIXEL_GET(prev_size, prev_size + shadow_size.w - x - 1));

                     *data = (c << 24) | (c << 16) | (c << 8) | c;
                     data++;
                  }
               }
               break;
            default:
               break;
         }
         evas_object_image_data_update_add(obj, 0, 0, shadow_size.w, shadow_size.h);
      }
   }

   free(pixels);
   shadow->shadow_need_recalc = ETK_FALSE;
}

/* Creates the border objects of the shadow container */
static void _etk_shadow_border_recalc(Etk_Shadow *shadow)
{
   Evas *evas;
   Evas_Object *obj;
   int i;

   if (!shadow || !shadow->border_need_recalc)
      return;
   if (!(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(shadow))))
      return;

   /* Create or destroy the border objects */
   for (i = 0; i < 4; i++)
   {
      if (shadow->border_objs[i] && shadow->border_width <= 0)
      {
         evas_object_del(shadow->border_objs[i]);
         shadow->border_objs[i] = NULL;
      }
      else if (!shadow->border_objs[i] && shadow->border_width > 0)
      {
         obj = evas_object_rectangle_add(evas);
         shadow->border_objs[i] = obj;
         etk_widget_member_object_add(ETK_WIDGET(shadow), obj);
         evas_object_pass_events_set(obj, 1);
         evas_object_color_set(obj, shadow->border_color.r, shadow->border_color.g,
            shadow->border_color.b, shadow->border_color.a);
         evas_object_clip_set(obj, shadow->clip);
         evas_object_show(obj);
      }
   }

   shadow->border_need_recalc = ETK_FALSE;
}

/* Gets whether the given edge is visible */
static Etk_Bool _etk_shadow_edge_visible(Etk_Shadow *shadow, Etk_Shadow_Object_Id object_id)
{
   Etk_Shadow_Edges edge;

   if (!shadow)
      return ETK_FALSE;

   switch (object_id)
   {
      case ETK_SHADOW_LEFT_OBJECT:
         edge = ETK_SHADOW_LEFT;
         break;
      case ETK_SHADOW_RIGHT_OBJECT:
         edge = ETK_SHADOW_RIGHT;
         break;
      case ETK_SHADOW_TOP_OBJECT:
         edge = ETK_SHADOW_TOP;
         break;
      case ETK_SHADOW_BOTTOM_OBJECT:
         edge = ETK_SHADOW_BOTTOM;
         break;
      default:
         edge = ETK_SHADOW_NO_EDGE;
         break;
   }

   return (shadow->edges & edge);
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Shadow
 *
 * TODOC: Description
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Bin
 *         - Etk_Shadow
 *
 * \par Properties:
 * @prop_name "propname": Description
 * @prop_type Float
 * @prop_rw
 * @prop_val 0.5
 */
