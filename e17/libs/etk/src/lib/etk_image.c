/** @file etk_image.c */
#include "etk_image.h"
#include <Evas.h>
#include <Edje.h>
#include <stdlib.h>
#include <string.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"
#include "etk_theme.h"

/**
 * @addtogroup Etk_Image
 * @{
 */

enum Etk_Image_Property_Id
{
   ETK_IMAGE_FILE_PROPERTY,
   ETK_IMAGE_EDJE_FILE_PROPERTY,
   ETK_IMAGE_EDJE_GROUP_PROPERTY,
   ETK_IMAGE_KEEP_ASPECT_PROPERTY,
   ETK_IMAGE_USE_EDJE_PROPERTY,
   ETK_IMAGE_STOCK_ID_PROPERTY,
   ETK_IMAGE_STOCK_SIZE_PROPERTY
};

static void _etk_image_constructor(Etk_Image *image);
static void _etk_image_destructor(Etk_Image *image);
static void _etk_image_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_image_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_image_realize_cb(Etk_Object *object, void *data);
static void _etk_image_unrealize_cb(Etk_Object *object, void *data);
static void _etk_image_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_image_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_image_load(Etk_Image *image);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Image
 * @return Returns the type of an Etk_Image
 */
Etk_Type *etk_image_type_get()
{
   static Etk_Type *image_type = NULL;

   if (!image_type)
   {
      image_type = etk_type_new("Etk_Image", ETK_WIDGET_TYPE, sizeof(Etk_Image),
         ETK_CONSTRUCTOR(_etk_image_constructor), ETK_DESTRUCTOR(_etk_image_destructor));
      
      etk_type_property_add(image_type, "image_file", ETK_IMAGE_FILE_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(image_type, "edje_file", ETK_IMAGE_EDJE_FILE_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(image_type, "edje_group", ETK_IMAGE_EDJE_GROUP_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(image_type, "keep_aspect", ETK_IMAGE_KEEP_ASPECT_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(image_type, "use_edje", ETK_IMAGE_USE_EDJE_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(image_type, "stock_id", ETK_IMAGE_STOCK_ID_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_STOCK_NO_STOCK));
      etk_type_property_add(image_type, "stock_size", ETK_IMAGE_STOCK_SIZE_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_STOCK_SMALL));

      image_type->property_set = _etk_image_property_set;
      image_type->property_get = _etk_image_property_get;
   }

   return image_type;
}

/**
 * @brief Creates a new image
 * @return Returns the new image widget
 */
Etk_Widget *etk_image_new()
{
   return etk_widget_new(ETK_IMAGE_TYPE, NULL);
}

/**
 * @brief Creates a new image and loads the image from an image file
 * @param filename the name of the file to load
 * @return Returns the new image widget
 */
Etk_Widget *etk_image_new_from_file(const char *filename)
{
   return etk_widget_new(ETK_IMAGE_TYPE, "image_file", filename, NULL);
}

/**
 * @brief Creates a new image and loads the image from an edje file
 * @param edje_filename the name of the edje file to load
 * @param edje_group the name of the edje group to load
 * @return Returns the new image widget
 */
Etk_Widget *etk_image_new_from_edje(const char *edje_filename, const char *edje_group)
{
   return etk_widget_new(ETK_IMAGE_TYPE, "edje_file", edje_filename, "edje_group", edje_group, NULL);
}

/**
 * @brief Creates a new image and loads the image corresponding to the stock id
 * @param stock_id the stock id corresponding to the image
 * @param stock_size the size of the image to load
 * @return Returns the new image widget
 */
Etk_Widget *etk_image_new_from_stock(Etk_Stock_Id stock_id, Etk_Stock_Size stock_size)
{
   Etk_Widget *new_image;
   
   new_image = etk_image_new();
   etk_image_set_from_stock(ETK_IMAGE(new_image), stock_id, stock_size);
   return new_image;
}

/**
 * @brief Loads the image from a file
 * @param image an image
 * @param filename the name of the file to load
 */
void etk_image_set_from_file(Etk_Image *image, const char *filename)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(image)) || (image->filename == filename))
      return;

   free(image->filename);
   image->filename = filename ? strdup(filename) : NULL;
   if (!etk_object_notify(ETK_OBJECT(image), "image_file"))
      return;

   if (image->edje_group)
   {
      free(image->edje_group);
      image->edje_group = NULL;
      if (!etk_object_notify(ETK_OBJECT(image), "edje_group"))
         return;
   }
   if (image->edje_filename)
   {
      free(image->edje_filename);
      image->edje_filename = NULL;
      if (!etk_object_notify(ETK_OBJECT(image), "edje_file"))
         return;
   }
   if (image->use_edje)
   {
      image->use_edje = ETK_FALSE;
      image->object_type_changed = ETK_TRUE;
      if (!etk_object_notify(ETK_OBJECT(image), "use_edje"))
         return;
   }
   if (image->stock_id != ETK_STOCK_NO_STOCK)
   {
      image->stock_id = ETK_STOCK_NO_STOCK;
      if (!etk_object_notify(ETK_OBJECT(image), "stock_id"))
         return;
   }

   _etk_image_load(image);
}

/**
 * @brief Gets the name of the file used for the image
 * @param image an image
 * @return Returns the name of the file used by the image (NULL on failure)
 */
const char *etk_image_file_get(Etk_Image *image)
{
   if (!image || image->use_edje)
      return NULL;
   return image->filename;
}

/**
 * @brief Loads the image from an edje file
 * @param image an image
 * @param edje_filename the name of the edje file to load
 * @param edje_group the name of the edje group to load
 */
void etk_image_set_from_edje(Etk_Image *image, const char *edje_filename, const char *edje_group)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(image)))
      return;

   if (image->edje_filename != edje_filename)
   {
      free(image->edje_filename);
      image->edje_filename = edje_filename ? strdup(edje_filename) : NULL;
      if (!etk_object_notify(ETK_OBJECT(image), "edje_file"))
         return;
   }
   if (image->edje_group != edje_group)
   {
      free(image->edje_group);
      image->edje_group = edje_group ? strdup(edje_group) : NULL;
      if (!etk_object_notify(ETK_OBJECT(image), "edje_group"))
         return;
   }

   if (image->filename)
   {
      free(image->filename);
      image->filename = NULL;
      if (!etk_object_notify(ETK_OBJECT(image), "image_file"))
         return;
   }
   if (!image->use_edje)
   {
      image->use_edje = ETK_TRUE;
      image->object_type_changed = ETK_TRUE;
      if (!etk_object_notify(ETK_OBJECT(image), "use_edje"))
         return;
   }
   if (image->stock_id != ETK_STOCK_NO_STOCK)
   {
      image->stock_id = ETK_STOCK_NO_STOCK;
      if (!etk_object_notify(ETK_OBJECT(image), "stock_id"))
         return;
   }

   _etk_image_load(image);
}

/**
 * @brief Gets the filename and the group of the edje object used for the image
 * @param image an image
 * @param edje_filename the location to store the filename of the edje object
 * @param edje_group the location to store the filename of the edje group
 */
void etk_image_edje_get(Etk_Image *image, char **edje_filename, char **edje_group)
{
   if (!image || !image->use_edje)
   {
      if (edje_filename)
         *edje_filename = NULL;
      if (edje_group)
         *edje_group = NULL;
      return;
   }

   if (edje_filename)
      *edje_filename = image->edje_filename;
   if (edje_group)
      *edje_group = image->edje_group;
}

/**
 * @brief Loads the image corresponding to the stock id
 * @param image an image
 * @param stock_id the stock id corresponding to the image
 * @param stock_size the size of the stock icon
 */
void etk_image_set_from_stock(Etk_Image *image, Etk_Stock_Id stock_id, Etk_Stock_Size stock_size)
{
   char *key;

   if (!image || ((image->stock_id == stock_id) && (image->stock_size == stock_size)))
      return;
   
   if ((key = etk_stock_key_get(stock_id, stock_size)))
      etk_image_set_from_edje(image, etk_theme_icon_theme_get(), key);
   image->stock_id = stock_id;
   image->stock_size = stock_size;
   
   if (!etk_object_notify(ETK_OBJECT(image), "stock_id"))
      return;
   if (!etk_object_notify(ETK_OBJECT(image), "stock_size"))
      return;
}

/**
 * @brief Gets the stock id used by the image
 * @param image an image
 * @param stock_id the location where to store the stock id used by the image
 * @param stock_size the location where to store the stock size used by the image
 */
void etk_image_stock_get(Etk_Image *image, Etk_Stock_Id *stock_id, Etk_Stock_Size *stock_size)
{
   if (stock_id)
      *stock_id = image ? image->stock_id : ETK_STOCK_NO_STOCK;
   if (stock_size)
      *stock_size = image ? image->stock_size : ETK_STOCK_SMALL;
}

/**
 * @brief Gets the native size of the image
 * @param image an image
 * @param width the location where to set the native width of the image
 * @param height the location where to set the native height of the image
 */
void etk_image_size_get(Etk_Image *image, int *width, int *height)
{
   if (!image)
      return;
   
   if (!image)
   {
      if (width)
         *width = 0;
      if (height)
         *height = 0;
   }
   else
   {
      if (!image->use_edje)
         evas_object_image_size_get(image->image_object, width, height);
      else
         edje_object_size_min_get(image->image_object, width, height);
   }
}

/**
 * @brief Sets if the image should keep its aspect ratio when it is resized
 * @param image an image
 * @param keep_aspect if keep_aspect == ETK_TRUE, the image will keep its aspect ratio when itis resized
 */
void etk_image_keep_aspect_set(Etk_Image *image, Etk_Bool keep_aspect)
{
   if (!image)
      return;

   image->keep_aspect = keep_aspect;
   etk_widget_size_recalc_queue(ETK_WIDGET(image));
   etk_object_notify(ETK_OBJECT(image), "keep_aspect");
}

/**
 * @brief Get whether the image keeps its aspect ratio when it is resized
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
 * @brief Copies the image @a src_image to @a dest_image
 * @param dest_image the destination image
 * @param src_image the image to copy
 * @note If @a src_image is an edje image, the state of the edje animation won't be copied
 */
void etk_image_copy(Etk_Image *dest_image, Etk_Image *src_image)
{
   if (!dest_image || !src_image || dest_image == src_image)
      return;
   
   free(dest_image->filename);
   free(dest_image->edje_filename);
   free(dest_image->edje_group);
   
   dest_image->filename = src_image->filename ? strdup(src_image->filename) : NULL;
   dest_image->edje_group = src_image->edje_group ? strdup(src_image->edje_group) : NULL;
   dest_image->edje_filename = src_image->edje_group ? strdup(src_image->edje_filename) : NULL;
   dest_image->stock_id = src_image->stock_id;
   dest_image->stock_size = src_image->stock_size;
   dest_image->keep_aspect = src_image->keep_aspect;
   dest_image->object_type_changed = (dest_image->use_edje != src_image->use_edje);
   dest_image->use_edje = src_image->use_edje;
   
   _etk_image_load(dest_image);
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

   image->image_object = NULL;
   image->filename = NULL;
   image->edje_filename = NULL;
   image->edje_group = NULL;
   image->stock_id = ETK_STOCK_NO_STOCK;
   image->stock_size = ETK_STOCK_SMALL;
   image->keep_aspect = ETK_TRUE;
   image->use_edje = ETK_FALSE;
   image->object_type_changed = ETK_FALSE;

   widget->size_request = _etk_image_size_request;
   widget->size_allocate = _etk_image_size_allocate;

   etk_signal_connect("realize", ETK_OBJECT(image), ETK_CALLBACK(_etk_image_realize_cb), NULL);
   etk_signal_connect("unrealize", ETK_OBJECT(image), ETK_CALLBACK(_etk_image_unrealize_cb), NULL);
}

/* Destroys the image */
static void _etk_image_destructor(Etk_Image *image)
{
   if (!image)
      return;

   free(image->filename);
   free(image->edje_filename);
   free(image->edje_group);
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
         etk_image_set_from_file(image, etk_property_value_string_get(value));
         break;
      case ETK_IMAGE_EDJE_FILE_PROPERTY:
         etk_image_set_from_edje(image, etk_property_value_string_get(value), image->edje_group);
         break;
      case ETK_IMAGE_EDJE_GROUP_PROPERTY:
         etk_image_set_from_edje(image, image->edje_filename, etk_property_value_string_get(value));
         break;
      case ETK_IMAGE_KEEP_ASPECT_PROPERTY:
         etk_image_keep_aspect_set(image, etk_property_value_bool_get(value));
         break;
      case ETK_IMAGE_STOCK_ID_PROPERTY:
         etk_image_set_from_stock(image, etk_property_value_int_get(value), image->stock_size);
         break;
      case ETK_IMAGE_STOCK_SIZE_PROPERTY:
         etk_image_set_from_stock(image, image->stock_id, etk_property_value_int_get(value));
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
      case ETK_IMAGE_FILE_PROPERTY:
         etk_property_value_string_set(value, image->filename);
         break;
      case ETK_IMAGE_EDJE_FILE_PROPERTY:
         etk_property_value_string_set(value, image->edje_filename);
         break;
      case ETK_IMAGE_EDJE_GROUP_PROPERTY:
         etk_property_value_string_set(value, image->edje_group);
         break;
      case ETK_IMAGE_KEEP_ASPECT_PROPERTY:
         etk_property_value_bool_set(value, image->keep_aspect);
         break;
      case ETK_IMAGE_USE_EDJE_PROPERTY:
         etk_property_value_bool_set(value, image->use_edje);
         break;
      case ETK_IMAGE_STOCK_ID_PROPERTY:
         etk_property_value_int_set(value, image->stock_id);
         break;
      case ETK_IMAGE_STOCK_SIZE_PROPERTY:
         etk_property_value_int_set(value, image->stock_size);
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

   if (image->image_object)
   {
      if (image->use_edje)
      {
         Evas_Coord min_x, min_y, calc_x, calc_y;

         edje_object_size_min_get(image->image_object, &min_x, &min_y);
         edje_object_size_min_calc(image->image_object, &calc_x, &calc_y);
         size->w = ETK_MAX(min_x, calc_x);
         size->h = ETK_MAX(min_y, calc_y);
      }
      else
         evas_object_image_size_get(image->image_object, &size->w, &size->h);
   }
   else
   {
      size->w = 0;
      size->h = 0;
   }
}

/* Resizes the image to the allocated size */
static void _etk_image_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Image *image;

   if (!(image = ETK_IMAGE(widget)))
      return;

   if (!image->image_object)
      return;

   if (image->keep_aspect)
   {
      double aspect_ratio;
      int image_w, image_h;
      int new_size;

      if (image->use_edje)
         edje_object_size_min_get(image->image_object, &image_w, &image_h);
      else
         evas_object_image_size_get(image->image_object, &image_w, &image_h);
      if (image_w <= 0 || image_h <= 0)
      {
         image_w = 1;
         image_h = 1;
      }
      
      aspect_ratio = (double)image_w / (double)image_h;
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
   
   if (!image->use_edje)
      evas_object_image_fill_set(image->image_object, 0, 0, geometry.w, geometry.h);
   
   evas_object_move(image->image_object, geometry.x, geometry.y);
   evas_object_resize(image->image_object, geometry.w, geometry.h);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the image is realized */
static void _etk_image_realize_cb(Etk_Object *object, void *data)
{
   Etk_Image *image;
   Evas *evas;

   if (!(image = ETK_IMAGE(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(image))))
      return;
   _etk_image_load(image);
}

/* Called when the image is unrealized */
static void _etk_image_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Image *image;

   if (!(image = ETK_IMAGE(object)))
      return;
   image->image_object = NULL;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Load the image from the image or the edje file */
static void _etk_image_load(Etk_Image *image)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(image)))
      return;

   if (image->image_object && image->object_type_changed)
   {
      etk_widget_member_object_del(widget, image->image_object);
      evas_object_del(image->image_object);
      image->image_object = NULL;
      image->object_type_changed = ETK_FALSE;
   }
   if (image->filename)
   {
      int error_code;
      Evas *evas;

      if (!image->image_object && (evas = etk_widget_toplevel_evas_get(widget)))
      {
         image->image_object = evas_object_image_add(evas);
         etk_widget_member_object_add(widget, image->image_object);
      }
      if (image->image_object)
      {
         const char *image_file;
         
         evas_object_image_file_get(image->image_object, &image_file, NULL);
         if (!image_file || strcmp(image_file, image->filename) != 0)
         {
            evas_object_image_file_set(image->image_object, image->filename, NULL);
            if ((error_code = evas_object_image_load_error_get(image->image_object)))
            {
               ETK_WARNING("Unable to load image from file \"%s\", error %d", image->filename, error_code);
               evas_object_hide(image->image_object);
            }
            else
               evas_object_show(image->image_object);
         }
      }
   }
   else if (image->edje_filename && image->edje_group)
   {
      int error_code;
      Evas *evas;

      if (!image->image_object && (evas = etk_widget_toplevel_evas_get(widget)))
      {
         image->image_object = edje_object_add(evas);
         etk_widget_member_object_add(widget, image->image_object);
      }
      if (image->image_object)
      {
         const char *edje_file, *edje_group;
         
         edje_object_file_get(image->image_object, &edje_file, &edje_group);
         if (!edje_file || !edje_group || strcmp(edje_file, image->edje_filename) != 0 || strcmp(edje_group, image->edje_group) != 0)
         {
            edje_object_file_set(image->image_object, image->edje_filename, image->edje_group);
            if ((error_code = edje_object_load_error_get(image->image_object)))
            {
               ETK_WARNING("Unable to load image from edje file \"%s\"/\"%s\", error %d", image->edje_filename, image->edje_group, error_code);
               evas_object_hide(image->image_object);
            }
            else
               evas_object_show(image->image_object);
         }
      }
   }

   etk_widget_size_recalc_queue(widget);
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
 * files (edj), or from stock IDs.
 * 
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Image
 *
 * \par Properties:
 * @prop_name "image_file": The image file (.png, .jpg, ...) which the image is loaded from.
 * Set to NULL if the image is loaded from an edje file (.edj)
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "edje_file": The edje file (.edj) which the image is loaded from.
 * Set to NULL if the image is loaded from an image file (.png, .jpg, ...)
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "edje_group": The edje group of the image.
 * Set to NULL if the image is loaded from an image file (.png, .jpg, ...)
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "keep_aspect": Whether of not the image keeps its aspect ratio when it is resized
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_TRUE
 * \par
 * @prop_name "use_edje": Whether of not the image is loaded from an edje file (.edj)
 * @prop_type Boolean
 * @prop_ro
 * @prop_val ETK_FALSE
 * \par
 * @prop_name "stock_id": The stock ID used by the image.
 * Set to ETK_STOCK_NO_STOCK if the image is not loaded from a stock icon
 * @prop_type Integer
 * @prop_rw
 * @prop_val ETK_STOCK_NO_STOCK
 * \par
 * @prop_name "stock_size": The size of the stock icon used by the image.
 * Set to a random value if the image is not loaded from a stock icon
 * @prop_type Integer
 * @prop_rw
 * @prop_val ETK_STOCK_SMALL
 */
