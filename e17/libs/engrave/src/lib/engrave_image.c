#include <Engrave.h>

Engrave_Image *
engrave_image_new(char *name, Engrave_Image_Type type, double value)
{
  Engrave_Image *image;

  image = NEW(Engrave_Image, 1);
  image->name = (name ? strdup(name) : NULL);
  image->type = type; 
  image->value = value;
  return image;
}

