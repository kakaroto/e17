/** @file etk_image.c */
#include "etk_image.h"
#include <Evas.h>
#include <Edje.h>
#include <stdlib.h>
#include <string.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Image
 * @{
 */

enum _Etk_Image_Property_Id
{
   ETK_IMAGE_FILE_PROPERTY,
   ETK_IMAGE_EDJE_FILE_PROPERTY,
   ETK_IMAGE_EDJE_GROUP_PROPERTY,
   ETK_IMAGE_KEEP_ASPECT_PROPERTY,
   ETK_IMAGE_USE_EDJE_PROPERTY
};

static void _etk_image_constructor(Etk_Image *image);
static void _etk_image_destructor(Etk_Image *image);
static void _etk_image_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_image_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_image_realize_cb(Etk_Object *object, void *data);
static void _etk_image_move_resize(Etk_Widget *widget, int x, int y, int w, int h);
static void _etk_image_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_image_load(Etk_Image *image);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Image
 * @return Returns the type on an Etk_Image
 */
Etk_Type *etk_image_type_get()
{
   static Etk_Type *image_type = NULL;

   if (!image_type)
   {
      image_type = etk_type_new("Etk_Image", ETK_WIDGET_TYPE, sizeof(Etk_Image), ETK_CONSTRUCTOR(_etk_image_constructor), ETK_DESTRUCTOR(_etk_image_destructor), NULL);
      
      etk_type_property_add(image_type, "image_file", ETK_IMAGE_FILE_PROPERTY, ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(image_type, "edje_file", ETK_IMAGE_EDJE_FILE_PROPERTY, ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(image_type, "edje_group", ETK_IMAGE_EDJE_GROUP_PROPERTY, ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(image_type, "keep_aspect", ETK_IMAGE_KEEP_ASPECT_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(TRUE));
      etk_type_property_add(image_type, "use_edje", ETK_IMAGE_USE_EDJE_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE, etk_property_value_bool(FALSE));

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
 * @brief Creates a new image and loads the image from the file
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
 * @brief Loads the image from a file
 * @param image an image
 * @param filename the name of the file to load
 */
void etk_image_set_from_file(Etk_Image *image, const char *filename)
{
   Etk_Widget *widget;
   char *old_filename;

   if (!(widget = ETK_WIDGET(image)))
      return;

   old_filename = image->filename;
   if (filename)
      image->filename = strdup(filename);
   else
      image->filename = NULL;
   free(old_filename);
   etk_object_notify(ETK_OBJECT(image), "image_file");

   if (image->edje_group)
   {
      free(image->edje_group);
      image->edje_group = NULL;
      etk_object_notify(ETK_OBJECT(image), "edje_group");
   }
   if (image->edje_filename)
   {
      free(image->edje_filename);
      image->edje_filename = NULL;
      etk_object_notify(ETK_OBJECT(image), "edje_file");
   }

   if (image->use_edje)
   {
      image->use_edje = FALSE;
      etk_object_notify(ETK_OBJECT(image), "use_edje");
   }

   _etk_image_load(image);
}

/**
 * @brief Gets the name of the file used for the image
 * @param image an image
 * @return Returns the name of the file use for the image (NULL on failure)
 */
const char *etk_image_file_get(Etk_Image *image)
{
   if (!image)
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
   char *previous_filename, *previous_group;

   if (!(widget = ETK_WIDGET(image)))
      return;

   previous_filename = image->edje_filename;
   if (edje_filename)
      image->edje_filename = strdup(edje_filename);
   else
      image->edje_filename = NULL;
   free(previous_filename);
   etk_object_notify(ETK_OBJECT(image), "edje_file");

   previous_group = image->edje_group;
   if (edje_group)
      image->edje_group = strdup(edje_group);
   else
      image->edje_group = NULL;
   free(previous_group);
   etk_object_notify(ETK_OBJECT(image), "edje_group");

   if (image->filename)
   {
      free(image->filename);
      image->filename = NULL;
      etk_object_notify(ETK_OBJECT(image), "image_file");
   }

   if (!image->use_edje)
   {
      image->use_edje = TRUE;
      etk_object_notify(ETK_OBJECT(image), "use_edje");
   }

   _etk_image_load(image);
}

/**
 * @brief Gets the filename and the group of the edje object used for the image
 * @param image an image
 * @param edje_filename the location to store the filename of the edje object
 * @param edje_group the location to store the filename of the edje group
 */
void etk_image_edje_file_get(Etk_Image *image, char **edje_filename, char **edje_group)
{
   if (!image)
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
 * @brief Sets if the image should keep its aspect ratio when it's resized
 * @param image an image
 * @param keep_aspect if keep_aspect == TRUE, the image will keep its aspect ratio when it's resized
 */
void etk_image_keep_aspect_set(Etk_Image *image, Etk_Bool keep_aspect)
{
   if (!image)
      return;

   image->keep_aspect = keep_aspect;
   etk_widget_resize_queue(ETK_WIDGET(image));
   etk_object_notify(ETK_OBJECT(image), "keep_aspect");
}

/**
 * @brief Checks if the image keeps its aspect ratio when it's resized
 * @param image an image
 * @return Returns TRUE if the image keeps its aspect ratio when it's resized
 */
Etk_Bool etk_image_keep_aspect_get(Etk_Image *image)
{
   if (!image)
      return TRUE;

   return image->keep_aspect;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_image_constructor(Etk_Image *image)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(image)))
      return;

   image->image_object = NULL;
   image->filename = NULL;
   image->edje_filename = NULL;
   image->edje_group = NULL;
   image->keep_aspect = TRUE;
   image->use_edje = FALSE;

   widget->size_request = _etk_image_size_request;
   widget->move_resize = _etk_image_move_resize;

   etk_signal_connect_after("realize", ETK_OBJECT(image), ETK_CALLBACK(_etk_image_realize_cb), NULL);
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
      default:
         break;
   }
}

/* Calculates the ideal size of the image */
static void _etk_image_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Etk_Image *image;

   if (!(image = ETK_IMAGE(widget)) || !size_requisition)
      return;

   if (image->image_object)
   {
      if (image->use_edje)
      {
         Evas_Coord min_x, min_y, calc_x, calc_y;

         edje_object_size_min_get(image->image_object, &min_x, &min_y);
         edje_object_size_min_calc(image->image_object, &calc_x, &calc_y);
         size_requisition->w = ETK_MAX(min_x, calc_x);
         size_requisition->h = ETK_MAX(min_y, calc_y);
      }
      else
         evas_object_image_size_get(image->image_object, &size_requisition->w, &size_requisition->h);
   }
   else
   {
      size_requisition->w = 0;
      size_requisition->h = 0;
   }
}

/* Moves and resizes the image */
static void _etk_image_move_resize(Etk_Widget *widget, int x, int y, int w, int h)
{
   Etk_Image *image;

   if (!(image = ETK_IMAGE(widget)))
      return;

   if (!image->image_object)
      return;

   if (!image->use_edje)
   {
      if (image->keep_aspect)
      {
         float aspect_ratio;
         int image_w, image_h;
         int new_size;

         evas_object_image_size_get(image->image_object, &image_w, &image_h);
         aspect_ratio = (float)image_w / image_h;

         if (h * aspect_ratio > w)
         {
            new_size = w / aspect_ratio;
            y += (h - new_size) / 2;
            h = new_size;
         }
         else
         {
            new_size = h * aspect_ratio;
            x += (w - new_size) / 2;
            w = new_size;
         }
         
      }
      evas_object_image_fill_set(image->image_object, 0, 0, w, h);
   }
   evas_object_move(image->image_object, x, y);
   evas_object_resize(image->image_object, w, h);
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

   if (image->image_object)
   {
      etk_widget_member_object_del(widget, image->image_object);
      evas_object_del(image->image_object);
      image->image_object = NULL;
   }
   if (image->filename)
   {
      int error_code;
      Evas *evas;

      if ((evas = etk_widget_toplevel_evas_get(widget)))
      {
         image->image_object = evas_object_image_add(evas);
         evas_object_image_file_set(image->image_object, image->filename, NULL);
         if ((error_code = evas_object_image_load_error_get(image->image_object)))
         {
            ETK_WARNING("Unable to load image from file \"%s\", error %d", image->filename, error_code);
            evas_object_del(image->image_object);
            image->image_object = NULL;
         }
         else
         {
            evas_object_show(image->image_object);
            etk_widget_member_object_add(widget, image->image_object);
         }
      }
   }
   else if (image->edje_filename && image->edje_group)
   {
      int error_code;
      Evas *evas;

      if ((evas = etk_widget_toplevel_evas_get(widget)))
      {
         image->image_object = edje_object_add(evas);
         edje_object_file_set(image->image_object, image->edje_filename, image->edje_group);
         if ((error_code = edje_object_load_error_get(image->image_object)))
         {
            ETK_WARNING("Unable to load image from edje file \"%s\"/\"%s\", error %d", image->edje_filename, image->edje_group, error_code);
            evas_object_del(image->image_object);
            image->image_object = NULL;
         }
         else
            etk_widget_member_object_add(widget, image->image_object);
      }
   }

   etk_widget_resize_queue(widget);
}

/** @} */
