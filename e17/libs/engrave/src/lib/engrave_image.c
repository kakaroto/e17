#include <Engrave.h>

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
 * engrave_image_new - create a new Engrave_Image object.
 * @param name: The name of the given image
 * @param type: The Engrave_Image_Type of the given image.
 * @param value: A compression value for the given image (if applicable)
 *
 * @return Returns a pointer to a newly allocated Engrave_Image object on
 * success or NULL on failure.
 */
Engrave_Image *
engrave_image_new(char *name, Engrave_Image_Type type, double value)
{
  Engrave_Image *image;

  image = NEW(Engrave_Image, 1);
  if (!image) return NULL;

  image->name = (name ? strdup(name) : NULL);
  image->type = type; 
  image->value = value;
  return image;
}

/**
 * @}
 */

