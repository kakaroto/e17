/** @file etk_image.h */
#ifndef _ETK_IMAGE_H_
#define _ETK_IMAGE_H_

#include <Evas.h>
#include "etk_widget.h"
#include "etk_stock.h"

/**
 * @defgroup Etk_Image Etk_Image
 * @brief An Etk_Image is a widget that can load and display an image from various sources
 * @{
 */

/** Gets the type of an image */
#define ETK_IMAGE_TYPE        (etk_image_type_get())
/** Casts the object to an Etk_Image */
#define ETK_IMAGE(obj)        (ETK_OBJECT_CAST((obj), ETK_IMAGE_TYPE, Etk_Image))
/** Check if the object is an Etk_Image */
#define ETK_IS_IMAGE(obj)     (ETK_OBJECT_CHECK_TYPE((obj), ETK_IMAGE_TYPE))

/** @brief The different sources that an Etk_Image can use */
typedef enum Etk_Image_Source
{
   ETK_IMAGE_FILE,              /**< The image is loaded from an image file */
   ETK_IMAGE_EDJE,              /**< The image is loaded from an Edje file */
   ETK_IMAGE_STOCK,             /**< The image is loaded from a stock id */
   ETK_IMAGE_EVAS_OBJECT,       /**< The image shows an existing Evas Object */
   ETK_IMAGE_DATA               /**< The image uses pixel data given by the user */
} Etk_Image_Source;

/**
 * @brief @widget A widget that can load and display an image
 * @structinfo
 */
struct Etk_Image
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Evas_Object *object;
   
   Etk_Image_Source source;
   union
   {
      struct
      {
         char *filename;
         char *key;
      } file;
      
      struct
      {
         char *filename;
         char *group;
      } edje;
      
      struct
      {
         Etk_Stock_Id id;
         Etk_Stock_Size size;
      } stock;
      
      struct
      {
         void *pixels;
         Etk_Size size;
         Etk_Bool copied;
      } data;
   } info;
   
   double aspect_ratio;
   Etk_Bool keep_aspect;
};

Etk_Type *etk_image_type_get(void);

Etk_Widget *etk_image_new(void);
Etk_Widget *etk_image_new_from_file(const char *filename, const char *key);
Etk_Widget *etk_image_new_from_edje(const char *filename, const char *group);
Etk_Widget *etk_image_new_from_stock(Etk_Stock_Id stock_id, Etk_Stock_Size stock_size);
Etk_Widget *etk_image_new_from_evas_object(Evas_Object *evas_object);
Etk_Widget *etk_image_new_from_data(int width, int height, void *data, Etk_Bool copy);

void         etk_image_set_from_file(Etk_Image *image, const char *filename, const char *key);
void         etk_image_file_get(Etk_Image *image, char **filename, char **key);
void         etk_image_set_from_edje(Etk_Image *image, const char *filename, const char *group);
void         etk_image_edje_get(Etk_Image *image, char **filename, char **group);
void         etk_image_set_from_stock(Etk_Image *image, Etk_Stock_Id stock_id, Etk_Stock_Size stock_size);
void         etk_image_stock_get(Etk_Image *image, Etk_Stock_Id *stock_id, Etk_Stock_Size *stock_size);
void         etk_image_set_from_evas_object(Etk_Image *image, Evas_Object *evas_object);
Evas_Object *etk_image_evas_object_get(Etk_Image *image);
void         etk_image_set_from_data(Etk_Image *image, int width, int height, void *data, Etk_Bool copy);
void        *etk_image_data_get(Etk_Image *image, Etk_Bool for_writing);

Etk_Image_Source etk_image_source_get(Etk_Image *image);

void etk_image_update(Etk_Image *image);
void etk_image_update_rect(Etk_Image *image, int x, int y, int w, int h);
void etk_image_copy(Etk_Image *dest_image, Etk_Image *src_image);

void     etk_image_size_get(Etk_Image *image, int *width, int *height);
void     etk_image_keep_aspect_set(Etk_Image *image, Etk_Bool keep_aspect);
Etk_Bool etk_image_keep_aspect_get(Etk_Image *image);
void     etk_image_aspect_ratio_set(Etk_Image *image, double aspect_ratio);
double   etk_image_aspect_ratio_get(Etk_Image *image);

/** @} */

#endif
