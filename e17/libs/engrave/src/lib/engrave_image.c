#include <Engrave.h>
#include "engrave_macros.h"

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
 * engrave_image_dup - duplicate an Engrave_Image
 * @param from: The Engrave_Image to duplicate
 *
 * @return Returns a pointer to the newly allocated Engrave_Image on success
 * or NULL on failure
 */
Engrave_Image *
engrave_image_dup(Engrave_Image *from)
{
  Engrave_Image *to;
  to = engrave_image_new(from->name, from->type, from->value);
  return to;
}

/**
 * engrave_image_free - free the image data
 * @param ef: The Engrave_Image to free
 *
 * @return Returns no value.
 */
void
engrave_image_free(Engrave_Image *ef)
{
  if (!ef) return;

  if (ef->name) free(ef->name);
  ef->name = NULL;

  if (ef->path) free(ef->path);
  ef->path = NULL;

  free(ef);
  ef = NULL;
}

/** 
 * engrave_image_name_get - get the image name
 * @param ei: The Engrave_Image to get the name from
 * 
 * @return Returns a pointer to the name of the image or NULL
 * on error. This pointer must be free'd by the user.
 */
char *
engrave_image_name_get(Engrave_Image *ei)
{
  if (!ei) return NULL;
  return (ei->name ? strdup(ei->name) : NULL);
}

/**
 * engrave_image_path_get - get the image path
 * @param ei: The Engrave_Image to get the path from
 * 
 * @return Returns a pointer to the path of the image or NULL
 * on error. This pointer must be free'd by the user.
 */
char *
engrave_image_path_get(Engrave_Image *ei)
{
  if (!ei) return NULL;
  return (ei->path ? strdup(ei->path) : NULL);
}

/**
 * engrave_image_type_get - get the type of the image
 * @param ei: The Engrave_Image to get the type from
 * 
 * @return Returns the Engrave_Image_Type associated with @a ei or
 * ENGRAVE_IMAGE_TYPE_NUM if the type is not set.
 */
Engrave_Image_Type
engrave_image_type_get(Engrave_Image *ei)
{
  return (ei ? ei->type : ENGRAVE_IMAGE_TYPE_NUM);
}

/**
 * engrave_image_compression_value_get - get the image compression value
 * @param ei: The Engrave_Image to get the value from
 *
 * @return Returns the compression value of the image or 0 if not set.
 */
double
engrave_image_compression_value_get(Engrave_Image *ei)
{
  return (ei ? ei->value : 0.0);
}


