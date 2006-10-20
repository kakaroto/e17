/** @file etk_shadow.c */
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
   ETK_SHADOW_HAS_BORDER_PROPERTY,
};

static void _etk_shadow_constructor(Etk_Shadow *shadow);
static void _etk_shadow_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_shadow_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_shadow_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_shadow_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_shadow_realized_cb(Etk_Object *object, void *data);
static void _etk_shadow_unrealized_cb(Etk_Object *object, void *data);
static void _etk_shadow_shadow_recalc(Etk_Shadow *shadow);
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
Etk_Type *etk_shadow_type_get()
{
   static Etk_Type *shadow_type = NULL;

   if (!shadow_type)
   {
      shadow_type = etk_type_new("Etk_Shadow", ETK_BIN_TYPE, sizeof(Etk_Shadow),
         ETK_CONSTRUCTOR(_etk_shadow_constructor), NULL);

      etk_type_property_add(shadow_type, "shadow_type", ETK_SHADOW_SHADOW_TYPE_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_SHADOW_NONE));
      etk_type_property_add(shadow_type, "has_border", ETK_SHADOW_HAS_BORDER_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      
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
   return etk_widget_new(ETK_SHADOW_TYPE, NULL);
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
   
   shadow->type = ETK_SHADOW_OUTSIDE;
   shadow->edges = ETK_SHADOW_ALL;
   shadow->offset_x = 0;
   shadow->offset_y = 0;
   shadow->radius = 30;
   
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
   
   etk_signal_connect("realize", ETK_OBJECT(shadow), ETK_CALLBACK(_etk_shadow_realized_cb), NULL);
   etk_signal_connect("unrealize", ETK_OBJECT(shadow), ETK_CALLBACK(_etk_shadow_unrealized_cb), NULL);
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
         break;
      case ETK_SHADOW_HAS_BORDER_PROPERTY:
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
         break;
      case ETK_SHADOW_HAS_BORDER_PROPERTY:
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
   /* TODO: add border width */

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

   if (!(shadow = ETK_SHADOW(widget)))
      return;
   
   evas_object_move(shadow->clip, geometry.x, geometry.y);
   evas_object_resize(shadow->clip, geometry.w, geometry.h);
   
   if (shadow->shadow_need_recalc)
      _etk_shadow_shadow_recalc(shadow);
   /* TODO:
   if (shadow->border_need_recalc)
      _etk_shadow_border_recalc(shadow);*/
   
   /* Render the shadow objects */
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
      
      /* Render the object */
      for (i = 0; i < 4; i++)
      {
         if (shadow->shadow_objs[i])
         {
            evas_object_move(shadow->shadow_objs[i], geometries[i].x, geometries[i].y);
            evas_object_resize(shadow->shadow_objs[i], geometries[i].w, geometries[i].h);
            evas_object_image_fill_set(shadow->shadow_objs[i], 0, 0, geometries[i].w, geometries[i].h);
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
   
   /* TODO: render border */

   if ((child = etk_bin_child_get(ETK_BIN(shadow))))
      etk_widget_size_allocate(child, geometry);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the shadow container is realized */
static void _etk_shadow_realized_cb(Etk_Object *object, void *data)
{
   Etk_Shadow *shadow;
   Evas *evas;
   
   if (!(shadow = ETK_SHADOW(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(shadow))))
      return;
   
   shadow->clip = evas_object_rectangle_add(evas);
   evas_object_show(shadow->clip);
   etk_widget_member_object_add(ETK_WIDGET(shadow), shadow->clip);
   
   shadow->shadow_need_recalc = ETK_TRUE;
   shadow->border_need_recalc = ETK_TRUE;
}

/* Called when the shadow container is unrealized */
static void _etk_shadow_unrealized_cb(Etk_Object *object, void *data)
{
   Etk_Shadow *shadow;
   int i;
   
   if (!(shadow = ETK_SHADOW(object)))
      return;
   
   shadow->clip = NULL;
   for (i = 0; i < 4; i++)
   {
      shadow->shadow_objs[i] = NULL;
      shadow->border_objs[i] = NULL;
   }
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
   
   if (!shadow || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(shadow))))
      return;
   
   for (i = 0; i < 4; i++)
   {
      if (shadow->shadow_objs[i])
      {
         evas_object_del(shadow->shadow_objs[i]);
         shadow->shadow_objs[i] = NULL;
      }
      if (shadow->border_objs[i])
      {
         evas_object_del(shadow->border_objs[i]);
         shadow->border_objs[i] = NULL;
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
   
   for (i = 0; i < 4; i++)
   {
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
      evas_object_color_set(obj, 0, 0, 0, 255 * 0.60);
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
 * Description
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
