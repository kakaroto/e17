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

/** @file etk_image.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_image.h"

#include <stdlib.h>
#include <string.h>

#include <Edje.h>

#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_theme.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Image
 * @{
 */

enum Etk_Image_Property_Id
{
   ETK_IMAGE_SOURCE_PROPERTY,
   ETK_IMAGE_FILE_PROPERTY,
   ETK_IMAGE_KEY_PROPERTY,
   ETK_IMAGE_STOCK_ID_PROPERTY,
   ETK_IMAGE_STOCK_SIZE_PROPERTY,
   ETK_IMAGE_EVAS_OBJECT_PROPERTY,
   ETK_IMAGE_KEEP_ASPECT_PROPERTY,
   ETK_IMAGE_ASPECT_RATIO_PROPERTY
};

static void _etk_image_constructor(Etk_Image *image);
static void _etk_image_destructor(Etk_Image *image);
static void _etk_image_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_image_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static Etk_Bool _etk_image_realized_cb(Etk_Object *object, void *data);
static void _etk_image_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_image_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_image_source_set(Etk_Image *image, Etk_Image_Source source);
static void _etk_image_load(Etk_Image *image);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Image
 * @return Returns the type of an Etk_Image
 */
Etk_Type *etk_image_type_get(void)
{
   static Etk_Type *image_type = NULL;

   if (!image_type)
   {
      image_type = etk_type_new("Etk_Image", ETK_WIDGET_TYPE, sizeof(Etk_Image),
         ETK_CONSTRUCTOR(_etk_image_constructor),
         ETK_DESTRUCTOR(_etk_image_destructor), NULL);

      etk_type_property_add(image_type, "source", ETK_IMAGE_SOURCE_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE, etk_property_value_int(ETK_IMAGE_FILE));
      etk_type_property_add(image_type, "file", ETK_IMAGE_FILE_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(image_type, "key", ETK_IMAGE_KEY_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(image_type, "stock-id", ETK_IMAGE_STOCK_ID_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_STOCK_NO_STOCK));
      etk_type_property_add(image_type, "stock-size", ETK_IMAGE_STOCK_SIZE_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_STOCK_MEDIUM));
      etk_type_property_add(image_type, "evas-object", ETK_IMAGE_EVAS_OBJECT_PROPERTY,
         ETK_PROPERTY_POINTER, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_pointer(NULL));
      etk_type_property_add(image_type, "keep-aspect", ETK_IMAGE_KEEP_ASPECT_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(image_type, "aspect-ratio", ETK_IMAGE_ASPECT_RATIO_PROPERTY,
         ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_double(0.0));

      image_type->property_set = _etk_image_property_set;
      image_type->property_get = _etk_image_property_get;
   }

   return image_type;
}

/**
 * @brief Creates a new empty image
 * @return Returns the new image widget
 */
Etk_Widget *etk_image_new(void)
{
   return etk_widget_new(ETK_IMAGE_TYPE, NULL);
}

/**
 * @brief Creates a new image and loads the image from an image file
 * @param filename the path of the file to load
 * @param key the key to load (only used if the file is an Eet file, otherwise you can set it to NULL)
 * @return Returns the new image widget
 */
Etk_Widget *etk_image_new_from_file(const char *filename, const char *key)
{
   Etk_Widget *image;

   image = etk_image_new();
   etk_image_set_from_file(ETK_IMAGE(image), filename, key);
   return image;
}

/**
 * @brief Creates a new image and loads the image from an edje-file
 * @param filename the name of the edje-file to load
 * @param group the name of the edje-group to load
 * @return Returns the new image widget
 */
Etk_Widget *etk_image_new_from_edje(const char *filename, const char *group)
{
   Etk_Widget *image;

   image = etk_image_new();
   etk_image_set_from_edje(ETK_IMAGE(image), filename, group);
   return image;
}

/**
 * @brief Creates a new image and loads the image corresponding to the stock id
 * @param stock_id the stock id corresponding to the image
 * @param stock_size the size of the image to load
 * @return Returns the new image widget
 */
Etk_Widget *etk_image_new_from_stock(Etk_Stock_Id stock_id, Etk_Stock_Size stock_size)
{
   Etk_Widget *image;

   image = etk_image_new();
   etk_image_set_from_stock(ETK_IMAGE(image), stock_id, stock_size);
   return image;
}

/**
 * @brief Creates a new image from the given evas object
 * @param evas_object the evas object to use for the image
 * @return Returns the new image widget
 */
Etk_Widget *etk_image_new_from_evas_object(Evas_Object *evas_object)
{
   Etk_Widget *image;

   image = etk_image_new();
   etk_image_set_from_evas_object(ETK_IMAGE(image), evas_object);
   return image;
}

/**
 * @brief Creates a new image from the given pixel data
 * @param width the width of the image
 * @param height the height of the image
 * @param data a pointer to the pixels: the pixels have to be stored in the premul'ed ARGB format
 * @param copy whether the pixels should be copied or not. If you decide not to copy the pixels, you have to make sure
 * the memory area where the pixels are stored is valid during all the lifetime of the image
 * @return Returns the new image widget
 */
Etk_Widget *etk_image_new_from_data(int width, int height, void *data, Etk_Bool copy)
{
   Etk_Widget *image;

   image = etk_image_new();
   etk_image_set_from_data(ETK_IMAGE(image), width, height, data, copy);
   return image;
}

/**
 * @brief Loads the image from a file
 * @param image an image
 * @param filename the path to the file to load
 * @param key the key to load (only used if the file is an Eet file, otherwise you can set it to NULL)
 */
void etk_image_set_from_file(Etk_Image *image, const char *filename, const char *key)
{
   if (!image)
      return;

   _etk_image_source_set(image, ETK_IMAGE_FILE);
   if (image->info.file.filename != filename)
   {
      free(image->info.file.filename);
      image->info.file.filename = filename ? strdup(filename) : NULL;
      etk_object_notify(ETK_OBJECT(image), "file");
   }
   if (image->info.file.key != key)
   {
      free(image->info.file.key);
      image->info.file.key = key ? strdup(key) : NULL;
      etk_object_notify(ETK_OBJECT(image), "key");
   }

   _etk_image_load(image);
}

/**
 * @brief Gets the path to the file used by the image
 * @param image an image
 * @param filename the location where to store the path to the loaded file
 * @param key the location where to store the key of the loaded image (NULL if the file is not loaded from an Eet file)
 */
void etk_image_file_get(Etk_Image *image, char **filename, char **key)
{
   if (!image || image->source != ETK_IMAGE_FILE)
   {
      if (filename)
         *filename = NULL;
      if (key)
         *key = NULL;
   }
   else
   {
      if (filename)
         *filename = image->info.file.filename;
      if (key)
         *key = image->info.file.key;
   }
}

/**
 * @brief Loads the image from an edje file
 * @param image an image
 * @param filename the path to the edje-file to load
 * @param group the name of the edje-group to load
 */
void etk_image_set_from_edje(Etk_Image *image, const char *filename, const char *group)
{
   if (!image)
      return;

   _etk_image_source_set(image, ETK_IMAGE_EDJE);
   if (image->info.edje.filename != filename)
   {
      free(image->info.edje.filename);
      image->info.edje.filename = filename ? strdup(filename) : NULL;
      etk_object_notify(ETK_OBJECT(image), "file");
   }
   if (image->info.edje.group != group)
   {
      free(image->info.edje.group);
      image->info.edje.group = group ? strdup(group) : NULL;
      etk_object_notify(ETK_OBJECT(image), "key");
   }

   _etk_image_load(image);
}

/**
 * @brief Gets the filename and the group of the edje-object used for the image
 * @param image an image
 * @param filename the location to store the path to the edje-file used
 * @param group the location to store the name of the edje-group used
 */
void etk_image_edje_get(Etk_Image *image, char **filename, char **group)
{
   if (!image || image->source != ETK_IMAGE_EDJE)
   {
      if (filename)
         *filename = NULL;
      if (group)
         *group = NULL;
      return;
   }

   if (filename)
      *filename = image->info.edje.filename;
   if (group)
      *group = image->info.edje.group;
}

/**
 * @brief Loads the image corresponding to the given stock-id
 * @param image an image
 * @param stock_id the stock-id corresponding to the icon to load
 * @param stock_size the size of the stock-icon
 */
void etk_image_set_from_stock(Etk_Image *image, Etk_Stock_Id stock_id, Etk_Stock_Size stock_size)
{
   if (!image)
      return;
   if (image->source == ETK_IMAGE_STOCK && image->info.stock.id == stock_id && image->info.stock.size == stock_size)
      return;

   _etk_image_source_set(image, ETK_IMAGE_STOCK);
   if (image->info.stock.id != stock_id)
   {
      image->info.stock.id = stock_id;
      etk_object_notify(ETK_OBJECT(image), "stock-id");
   }
   if (image->info.stock.size != stock_size)
   {
      image->info.stock.size = stock_size;
      etk_object_notify(ETK_OBJECT(image), "stock-size");
   }

   _etk_image_load(image);
}

/**
 * @brief Gets the stock-id and the stock-size used by the image
 * @param image an image
 * @param stock_id the location where to store the stock id used by the image
 * @param stock_size the location where to store the stock size used by the image
 */
void etk_image_stock_get(Etk_Image *image, Etk_Stock_Id *stock_id, Etk_Stock_Size *stock_size)
{
   if (!image || image->source != ETK_IMAGE_STOCK)
   {
      if (stock_id)
         *stock_id = ETK_STOCK_NO_STOCK;
      if (stock_size)
         *stock_size = ETK_STOCK_MEDIUM;
   }
   else
   {
      if (stock_id)
         *stock_id = image->info.stock.id;
      if (stock_size)
         *stock_size = image->info.stock.size;
   }
}

/**
 * @brief Loads the image from an Evas object
 * @param image an image
 * @param evas_object the Evas object to use. The object can be anything (image, edje object, emotion object, ...)
 */
void etk_image_set_from_evas_object(Etk_Image *image, Evas_Object *evas_object)
{
   if (!image)
      return;

   _etk_image_source_set(image, ETK_IMAGE_EVAS_OBJECT);
   if (image->object != evas_object)
   {
      image->object = evas_object;
      etk_object_notify(ETK_OBJECT(image), "evas-object");
   }

   _etk_image_load(image);
}

/**
 * @brief Gets the Evas object used by the image. You can call this function even if you have not explicitly set the
 * Evas object used by this image. For example, if you have loaded the image from a file, this function will return the
 * corresponding Evas image object. You should just be careful by manipulating it (don't use Edje functions on an image
 * object for example).
 * @param image an image
 * @return Returns the Evas object of the image
 */
Evas_Object *etk_image_evas_object_get(Etk_Image *image)
{
   if (!image)
      return NULL;
   return image->object;
}

/**
 * @brief Sets the pixels of the image
 * @param image an image
 * @param width the width of the image
 * @param height the height of the image
 * @param data a pointer to the pixels: the pixels have to be stored in the premul'ed ARGB format
 * @param copy whether the pixels should be copied or not. If you decide not to copy the pixels, you have to make sure
 * the memory area where the pixels are stored is valid during all the lifetime of the image
 * @return Returns the new image widget
 */
void etk_image_set_from_data(Etk_Image *image, int width, int height, void *data, Etk_Bool copy)
{
   if (!image)
      return;

   _etk_image_source_set(image, ETK_IMAGE_DATA);

   if (data && width > 0 && height > 0)
   {
      image->info.data.size.w = width;
      image->info.data.size.h = height;
      image->info.data.copied = copy;
      if (copy)
      {
         image->info.data.pixels = malloc(sizeof(width * height * 4));
         memcpy(image->info.data.pixels, data, width * height * 4);
      }
      else
         image->info.data.pixels = data;
   }

   _etk_image_load(image);
}

/**
 * @brief Gets a pointer to the image pixels. This function only works if the image has been loaded from a file or if
 * you have explicitely set its pixels with etk_image_set_from_data().
 * @param image an image
 * @param for_writing whether or not you want to be able to modify the pixels of the image. If so, call
 * etk_image_update() once you have finished.
 * @return Returns a pointer to the location of the pixels (stored in premul'ed ARGB format)
 * @note If the image is loaded from a file, it has to be realized. Otherwise it will return NULL
 */
void *etk_image_data_get(Etk_Image *image, Etk_Bool for_writing)
{
   if (!image)
      return NULL;

   if (image->source == ETK_IMAGE_FILE)
   {
      if (image->object)
         return evas_object_image_data_get(image->object, for_writing);
   }
   else if (image->source == ETK_IMAGE_DATA)
   {
      if (image->object)
         return evas_object_image_data_get(image->object, for_writing);
      else
         return image->info.data.pixels;
   }

   return NULL;
}

/**
 * @brief Gets the source of the image (file, edje-file, stock, Evas object or pixel data)
 * @param image an image
 * @return Returns the source of the image
 */
Etk_Image_Source etk_image_source_get(Etk_Image *image)
{
   if (!image)
      return ETK_IMAGE_FILE;
   return image->source;
}

/**
 * @brief Updates all the pixels of the image (to be called after you have modified the pixel buffer for example).
 * Same as etk_image_update_rect(image, 0, 0, image_width, image_height)
 * @param image an image
 */
void etk_image_update(Etk_Image *image)
{
   int w, h;

   if (!image)
      return;

   etk_image_size_get(image, &w, &h);
   etk_image_update_rect(image, 0, 0, w, h);
}

/**
 * @brief Updates a rectangle of the pixels of the image (to be called after you have modified the pixel buffer
 * for example). It only has effect on image loaded from a pixel buffer
 * @param image an image
 * @param x the x position of the top-left corner of the rectangle to update
 * @param y the y position of the top-left corner of the rectangle to update
 * @param w the width of the rectangle to update
 * @param h the height of the rectangle to update
 */
void etk_image_update_rect(Etk_Image *image, int x, int y, int w, int h)
{
   if (!image || image->source != ETK_IMAGE_DATA || !image->object)
      return;
   evas_object_image_data_update_add(image->object, x, y, w, h);
}

/**
 * @brief Copies the image @a src_image to @a dest_image
 * @param dest_image the destination image
 * @param src_image the image to copy
 * @note If @a src_image is an edje image, the current state of the edje animation won't be copied
 */
void etk_image_copy(Etk_Image *dest_image, Etk_Image *src_image)
{
   if (!dest_image || !src_image || dest_image == src_image)
      return;

   switch (src_image->source)
   {
      case ETK_IMAGE_FILE:
         etk_image_set_from_file(dest_image, src_image->info.file.filename, src_image->info.file.key);
         break;
      case ETK_IMAGE_EDJE:
         etk_image_set_from_edje(dest_image, src_image->info.edje.filename, src_image->info.edje.group);
         break;
      case ETK_IMAGE_STOCK:
         etk_image_set_from_stock(dest_image, src_image->info.stock.id, src_image->info.stock.size);
         break;
      case ETK_IMAGE_EVAS_OBJECT:
         ETK_WARNING("You can't copy an image that is set from an Evas Object");
         break;
      case ETK_IMAGE_DATA:
         etk_image_set_from_data(dest_image, src_image->info.data.size.w, src_image->info.data.size.h,
            src_image->info.data.pixels, ETK_TRUE);
         break;
      default:
         break;
   }

   etk_image_aspect_ratio_set(dest_image, src_image->aspect_ratio);
   etk_image_keep_aspect_set(dest_image, src_image->keep_aspect);
}

/**
 * @brief Gets the native size of the image. If the image is loaded from a file or from given pixels, it returns the
 * native size of the image. If the image is loaded from an Edje file or a stock-id, it returns the minimum size of the
 * Edje object (a stock image is an Edje object). Otherwise, the returned size is 0x0
 * @param image an image
 * @param width the location where to set the native width of the image
 * @param height the location where to set the native height of the image
 */
void etk_image_size_get(Etk_Image *image, int *width, int *height)
{
   if (width)
      *width = 0;
   if (height)
      *height = 0;

   if (!image || !image->object)
      return;

   if (image->source == ETK_IMAGE_FILE || image->source == ETK_IMAGE_DATA)
      evas_object_image_size_get(image->object, width, height);
   else if (image->source == ETK_IMAGE_EDJE || image->source == ETK_IMAGE_STOCK)
   {
      Evas_Coord min_x, min_y, calc_x, calc_y;

      edje_object_size_min_get(image->object, &min_x, &min_y);
      edje_object_size_min_calc(image->object, &calc_x, &calc_y);
      if (width)
         *width = ETK_MAX(min_x, calc_x);
      if (height)
         *height = ETK_MAX(min_y, calc_y);
   }
}

/**
 * @brief Sets if the image should keep its aspect ratio when it is resized
 * @param image an image
 * @param keep_aspect if @a keep_aspect is ETK_TRUE, the image will keep its aspect ratio when it is resized
 */
void etk_image_keep_aspect_set(Etk_Image *image, Etk_Bool keep_aspect)
{
   if (!image || image->keep_aspect == keep_aspect)
      return;

   image->keep_aspect = keep_aspect;
   etk_widget_redraw_queue(ETK_WIDGET(image));
   etk_object_notify(ETK_OBJECT(image), "keep-aspect");
}

/**
 * @brief Gets whether the image keeps its aspect ratio when it is resized
 * @param image an image
 * @return Returns ETK_TRUE if the image keeps its aspect ratio when it is resized
 */
Etk_Bool etk_image_keep_aspect_get(Etk_Image *image)
{
   if (!image)
      return ETK_TRUE;
   return image->keep_aspect;
}

/**
 * @brief Sets (forces) the aspect ratio of the image. You can use this function for example to set the aspect-ratio
 * when you set the image from an Evas object with etk_image_set_from_evas_object().
 * @param image an image
 * @param aspect_ratio the aspect ratio to set, or 0.0 to make Etk calculates automatically the aspect ratio
 */
void etk_image_aspect_ratio_set(Etk_Image *image, double aspect_ratio)
{
   if (!image || image->aspect_ratio)
      return;

   image->aspect_ratio = aspect_ratio;
   etk_widget_redraw_queue(ETK_WIDGET(image));
   etk_object_notify(ETK_OBJECT(image), "aspect-ratio");
}

/**
 * @brief Gets the aspect ratio you set to the image. If no aspect ratio has been set, it will return 0.0.
 * To know the native aspect ratio, call etk_image_size_get() to get the native size of the image and calculate the
 * aspect ratio from these values.
 * @param image an image
 * @return Returns the aspect ratio you set to the image, or 0.0 if no aspect ratio has been set
 */
double etk_image_aspect_ratio_get(Etk_Image *image)
{
   if (!image)
      return 0.0;
   return image->aspect_ratio;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the image */
static void _etk_image_constructor(Etk_Image *image)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(image)))
      return;

   image->object = NULL;
   image->source = ETK_IMAGE_FILE;
   image->info.file.filename = NULL;
   image->info.file.key = NULL;
   image->keep_aspect = ETK_TRUE;
   image->aspect_ratio = 0.0;

   widget->size_request = _etk_image_size_request;
   widget->size_allocate = _etk_image_size_allocate;

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(image), ETK_CALLBACK(_etk_image_realized_cb), NULL);
   etk_signal_connect_swapped_by_code(ETK_WIDGET_UNREALIZED_SIGNAL, ETK_OBJECT(image), ETK_CALLBACK(etk_callback_set_null), &image->object);
}

/* Destroys the image */
static void _etk_image_destructor(Etk_Image *image)
{
   if (!image)
      return;

   if (image->source == ETK_IMAGE_FILE)
   {
      free(image->info.file.filename);
      free(image->info.file.key);
   }
   else if (image->source == ETK_IMAGE_EDJE)
   {
      free(image->info.edje.filename);
      free(image->info.edje.group);
   }
   else if (image->source == ETK_IMAGE_DATA)
   {
      if (image->info.data.copied)
         free(image->info.data.pixels);
   }
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_image_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Image *image;

   if (!(image = ETK_IMAGE(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_IMAGE_FILE_PROPERTY:
         if (image->source == ETK_IMAGE_EDJE)
            etk_image_set_from_edje(image, etk_property_value_string_get(value), image->info.edje.group);
         else
            etk_image_set_from_file(image, etk_property_value_string_get(value), image->info.file.key);
         break;
      case ETK_IMAGE_KEY_PROPERTY:
         if (image->source == ETK_IMAGE_FILE)
            etk_image_set_from_file(image, image->info.file.filename, etk_property_value_string_get(value));
         else if (image->source == ETK_IMAGE_EDJE)
            etk_image_set_from_edje(image, image->info.edje.filename, etk_property_value_string_get(value));
         break;
      case ETK_IMAGE_STOCK_ID_PROPERTY:
         if (image->source == ETK_IMAGE_STOCK)
            etk_image_set_from_stock(image, etk_property_value_int_get(value), image->info.stock.size);
         else
            etk_image_set_from_stock(image, etk_property_value_int_get(value), ETK_STOCK_MEDIUM);
         break;
      case ETK_IMAGE_STOCK_SIZE_PROPERTY:
         if (image->source == ETK_IMAGE_STOCK)
            etk_image_set_from_stock(image, image->info.stock.id, etk_property_value_int_get(value));
         else
            etk_image_set_from_stock(image, ETK_STOCK_NO_STOCK, etk_property_value_int_get(value));
         break;
      case ETK_IMAGE_EVAS_OBJECT_PROPERTY:
         etk_image_set_from_evas_object(image, etk_property_value_pointer_get(value));
         break;
      case ETK_IMAGE_KEEP_ASPECT_PROPERTY:
         etk_image_keep_aspect_set(image, etk_property_value_bool_get(value));
         break;
      case ETK_IMAGE_ASPECT_RATIO_PROPERTY:
         etk_image_aspect_ratio_set(image, etk_property_value_double_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_image_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Image *image;

   if (!(image = ETK_IMAGE(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_IMAGE_SOURCE_PROPERTY:
         etk_property_value_int_set(value, image->source);
         break;
      case ETK_IMAGE_FILE_PROPERTY:
         if (image->source == ETK_IMAGE_FILE)
            etk_property_value_string_set(value, image->info.file.filename);
         else if (image->source == ETK_IMAGE_EDJE)
            etk_property_value_string_set(value, image->info.edje.filename);
         else
            etk_property_value_string_set(value, NULL);
         break;
      case ETK_IMAGE_KEY_PROPERTY:
         if (image->source == ETK_IMAGE_FILE)
            etk_property_value_string_set(value, image->info.file.key);
         else if (image->source == ETK_IMAGE_EDJE)
            etk_property_value_string_set(value, image->info.edje.group);
         else
            etk_property_value_string_set(value, NULL);
         break;
      case ETK_IMAGE_STOCK_ID_PROPERTY:
         if (image->source == ETK_IMAGE_STOCK)
            etk_property_value_int_set(value, image->info.stock.id);
         else
            etk_property_value_int_set(value, ETK_STOCK_NO_STOCK);
         break;
      case ETK_IMAGE_STOCK_SIZE_PROPERTY:
         if (image->source == ETK_IMAGE_STOCK)
            etk_property_value_int_set(value, image->info.stock.size);
         else
            etk_property_value_int_set(value, ETK_STOCK_MEDIUM);
         break;
      case ETK_IMAGE_EVAS_OBJECT_PROPERTY:
         etk_property_value_pointer_set(value, image->object);
         break;
      case ETK_IMAGE_KEEP_ASPECT_PROPERTY:
         etk_property_value_bool_set(value, image->keep_aspect);
         break;
      case ETK_IMAGE_ASPECT_RATIO_PROPERTY:
         etk_property_value_double_set(value, image->aspect_ratio);
         break;
      default:
         break;
   }
}

/* Calculates the ideal size of the image */
static void _etk_image_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_Image *image;

   if (!(image = ETK_IMAGE(widget)) || !size)
      return;
   etk_image_size_get(image, &size->w, &size->h);
}

/* Resizes the image to the allocated size */
static void _etk_image_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Image *image;

   if (!(image = ETK_IMAGE(widget)))
      return;
   if (!image->object)
      return;

   if (image->keep_aspect)
   {
      double aspect_ratio;
      int image_w, image_h;
      int new_size;

      if (image->aspect_ratio > 0.0)
         aspect_ratio = image->aspect_ratio;
      else
      {
         etk_image_size_get(image, &image_w, &image_h);
         if (image_w <= 0 || image_h <= 0)
         {
            image_w = 1;
            image_h = 1;
         }
         aspect_ratio = (double)image_w / (double)image_h;
      }

      if (geometry.h * aspect_ratio > geometry.w)
      {
         new_size = geometry.w / aspect_ratio;
         geometry.y += (geometry.h - new_size) / 2;
         geometry.h = new_size;
      }
      else
      {
         new_size = geometry.h * aspect_ratio;
         geometry.x += (geometry.w - new_size) / 2;
         geometry.w = new_size;
      }
   }

   if (image->source == ETK_IMAGE_FILE || image->source == ETK_IMAGE_DATA)
      evas_object_image_fill_set(image->object, 0, 0, geometry.w, geometry.h);

   evas_object_move(image->object, geometry.x, geometry.y);
   evas_object_resize(image->object, geometry.w, geometry.h);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the image is realized */
static Etk_Bool _etk_image_realized_cb(Etk_Object *object, void *data)
{
   Etk_Image *image;

   if (!(image = ETK_IMAGE(object)))
      return ETK_TRUE;
   _etk_image_load(image);
   return ETK_TRUE;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Sets the source of the image */
static void _etk_image_source_set(Etk_Image *image, Etk_Image_Source source)
{
   if (!image || image->source == source)
      return;

   switch (image->source)
   {
      case ETK_IMAGE_FILE:
         free(image->info.file.filename);
         free(image->info.file.key);
         break;
      case ETK_IMAGE_EDJE:
         free(image->info.edje.filename);
         free(image->info.edje.group);
         break;
      case ETK_IMAGE_DATA:
         if (image->info.data.copied)
            free(image->info.data.pixels);
         break;
      default:
         break;
   }

   switch (source)
   {
      case ETK_IMAGE_FILE:
         image->info.file.filename = NULL;
         image->info.file.key = NULL;
         break;
      case ETK_IMAGE_EDJE:
         image->info.edje.filename = NULL;
         image->info.edje.group = NULL;
         break;
      case ETK_IMAGE_STOCK:
         image->info.stock.id = ETK_STOCK_NO_STOCK;
         image->info.stock.size = ETK_STOCK_MEDIUM;
         break;
      case ETK_IMAGE_DATA:
         image->info.data.size.w = 0;
         image->info.data.size.h = 0;
         image->info.data.copied = ETK_FALSE;
         image->info.data.pixels = NULL;
         break;
      default:
         break;
   }

   if (image->object && image->source != ETK_IMAGE_EVAS_OBJECT)
      evas_object_del(image->object);
   image->object = NULL;

   image->source = source;
   etk_object_notify(ETK_OBJECT(image), "source");
}

/* Load the image from the image or the edje file */
static void _etk_image_load(Etk_Image *image)
{
   Evas *evas;

   if (!image || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(image))))
      return;

   /* Create the Evas object */
   if (!image->object)
   {
      switch (image->source)
      {
         case ETK_IMAGE_FILE:
         case ETK_IMAGE_DATA:
            image->object = evas_object_image_add(evas);
            break;
         case ETK_IMAGE_EDJE:
         case ETK_IMAGE_STOCK:
            image->object = edje_object_add(evas);
            break;
         default:
            break;
      }
   }

   if (!image->object)
      return;
   etk_widget_member_object_add(ETK_WIDGET(image), image->object);

   /* Load the image */
   switch (image->source)
   {
      case ETK_IMAGE_FILE:
      {
         const char *image_file;
         int error_code;

         evas_object_image_file_get(image->object, &image_file, NULL);
         if (!image_file || !image->info.file.filename || strcmp(image_file, image->info.file.filename) != 0)
         {
            evas_object_image_file_set(image->object, image->info.file.filename, image->info.file.key);
            if (image->info.file.filename && (error_code = evas_object_image_load_error_get(image->object)))
               ETK_WARNING("Unable to load image from file \"%s\", error %d", image->info.file.filename, error_code);
         }
         break;
      }
      case ETK_IMAGE_EDJE:
      case ETK_IMAGE_STOCK:
      {
         const char *file, *key;
         int error_code;

         if (image->source == ETK_IMAGE_STOCK)
         {
            file = etk_theme_icon_path_get();
            key = etk_stock_key_get(image->info.stock.id, image->info.stock.size);
         }
         else
         {
            file = image->info.edje.filename;
            key = image->info.edje.group;
         }

         edje_object_file_set(image->object, file, key);
         if (file && (error_code = edje_object_load_error_get(image->object)))
            ETK_WARNING("Unable to load image from edje-file  \"%s\"/\"%s\", error %d", file, key, error_code);
         break;
      }
      case ETK_IMAGE_DATA:
         evas_object_image_size_set(image->object, image->info.data.size.w, image->info.data.size.h);
         evas_object_image_data_set(image->object, image->info.data.pixels);
         break;
      default:
         break;
   }

   evas_object_show(image->object);
   etk_widget_size_recalc_queue(ETK_WIDGET(image));
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Image
 *
 * @image html widgets/image.png
 * The image can be loaded from several sources: image files (png, jpg, and the other formats supported by evas), edje
 * files (edj), stock-icons, data pixels or given Evas objects.
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Image
 *
 * \par Properties:
 * @prop_name "source": The source of the image (file, edje, stock, Evas object or pixel data)
 * @prop_type Integer (Etk_Image_Source)
 * @prop_ro
 * @prop_val ETK_IMAGE_FILE
 * \par
 * @prop_name "file": The path of the loaded file (path to the image-file or to the edje-file, or NULL if the image is
 * not loaded from a file)
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "key": The name of the loaded key: it corresponds to the loaded edje-group if the image is loaded from an
 * Edje-file, or to the key corresponding to the image if it is loaded from an Eet-file. Otherwise it is set to NULL
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "stock-id": The stock ID used by the image.
 * It is set to ETK_STOCK_NO_STOCK if the image is not a stock-icon
 * @prop_type Integer
 * @prop_rw
 * @prop_val ETK_STOCK_NO_STOCK
 * \par
 * @prop_name "stock-size": The size of the stock-icon used by the image.
 * It is set to ETK_STOCK_MEDIUM if the image is not a stock-icon
 * @prop_type Integer
 * @prop_rw
 * @prop_val ETK_STOCK_MEDIUM
 * \par
 * @prop_name "evas-object": A pointer to the Evas object corresponding to the image. You must be careful if you
 * manipulate it directly (do not call an Edje function on an Evas image object)
 * @prop_type Pointer (Evas_Object *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "keep-aspect": Whether of not the image keeps its aspect ratio when it is resized
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_TRUE
 * \par
 * @prop_name "aspect-ratio": The aspect-ratio of the image. If it is set to 0.0, Etk calculates it automatically
 * @prop_type Double
 * @prop_rw
 * @prop_val 0.0
 */
