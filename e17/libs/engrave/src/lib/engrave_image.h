#ifndef ENGRAVE_IMAGE_H
#define ENGRAVE_IMAGE_H

typedef struct _Engrave_Image Engrave_Image;
struct _Engrave_Image
{
  char *name; /* basename */
  char *path; /* dir path */
  Engrave_Image_Type type;
  double value;
};

Engrave_Image * engrave_image_new(char *name, 
                    Engrave_Image_Type type, double value);

#endif

