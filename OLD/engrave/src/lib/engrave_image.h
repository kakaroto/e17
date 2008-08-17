#ifndef ENGRAVE_IMAGE_H
#define ENGRAVE_IMAGE_H

/**
 * @file engrave_image.h Engrave_Image object functions.
 * @brief Contains all of the functions to manipulate Engrave_Image objects.
 */

/**
 * @defgroup Engrave_Image Engrave_Image: Functions to work with Engrave_Image blocks.
 *
 * @{
 */

/**
 * The Engrave_Image typedef
 */
typedef struct _Engrave_Image Engrave_Image;

/**
 * @brief Contains the needed image values
 */
struct _Engrave_Image
{
  char *name;               /**< The image basename */
  Engrave_Image_Type type;  /**< The image storage type */
  double value;             /**< Compression value for image */

  void *parent;     /**< Pointer to parent */
};

EAPI Engrave_Image *engrave_image_new(const char *name, 
                                      Engrave_Image_Type type, double value);
EAPI Engrave_Image *engrave_image_dup(Engrave_Image *from);
EAPI void engrave_image_free(Engrave_Image *ef);

EAPI void engrave_image_parent_set(Engrave_Image *ei, void *ef);
EAPI void *engrave_image_parent_get(Engrave_Image *ei);

EAPI const char *engrave_image_name_get(Engrave_Image *ei);
EAPI Engrave_Image_Type engrave_image_type_get(Engrave_Image *ei);
EAPI double engrave_image_compression_value_get(Engrave_Image *ei);

/**
 * @}
 */

#endif

