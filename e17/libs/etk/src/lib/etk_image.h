/** @file etk_image.h */
#ifndef _ETK_IMAGE_H_
#define _ETK_IMAGE_H_

#include <Evas.h>
#include "etk_widget.h"
#include "etk_stock.h"

/**
 * @defgroup Etk_Image Etk_Image
 * @{
 */

/** @brief Gets the type of a image */
#define ETK_IMAGE_TYPE        (etk_image_type_get())
/** @brief Casts the object to an Etk_Image */
#define ETK_IMAGE(obj)        (ETK_OBJECT_CAST((obj), ETK_IMAGE_TYPE, Etk_Image))
/** @brief Check if the object is an Etk_Image */
#define ETK_IS_IMAGE(obj)     (ETK_OBJECT_CHECK_TYPE((obj), ETK_IMAGE_TYPE))

/**
 * @struct Etk_Image
 * @brief A image is a simple widget that just displays an image
 */
struct _Etk_Image
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Evas_Object *image_object;
   char *filename;
   char *edje_group;
   char *edje_filename;
   
   Etk_Stock_Id stock_id;
   Etk_Stock_Size stock_size;
   
   Etk_Bool keep_aspect;
   Etk_Bool use_edje;
   Etk_Bool object_type_changed;
};

Etk_Type *etk_image_type_get();
Etk_Widget *etk_image_new();
Etk_Widget *etk_image_new_from_file(const char *filename);
Etk_Widget *etk_image_new_from_edje(const char *edje_filename, const char *edje_group);
Etk_Widget *etk_image_new_from_stock(Etk_Stock_Id stock_id, Etk_Stock_Size size);

void etk_image_set_from_file(Etk_Image *image, const char *filename);
const char *etk_image_file_get(Etk_Image *image);

void etk_image_set_from_edje(Etk_Image *image, const char *edje_filename, const char *edje_group);
void etk_image_edje_get(Etk_Image *image, char **edje_filename, char **edje_group);

void etk_image_set_from_stock(Etk_Image *image, Etk_Stock_Id stock_id, Etk_Stock_Size size);
void etk_image_stock_get(Etk_Image *image, Etk_Stock_Id *stock_id, Etk_Stock_Size *size);

void etk_image_copy(Etk_Image *dest_image, Etk_Image *src_image);

void etk_image_size_get(Etk_Image *image, int *width, int *height);
void etk_image_keep_aspect_set(Etk_Image *image, Etk_Bool keep_aspect);
Etk_Bool etk_image_keep_aspect_get(Etk_Image *image);

/** @} */

#endif
