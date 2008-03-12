#ifndef __EVAS_OBJECT_IMAGE_PYTHON_EXTRAS__
#define __EVAS_OBJECT_IMAGE_PYTHON_EXTRAS__

typedef unsigned char DATA8;
typedef unsigned short int DATA16;
typedef unsigned int DATA32;

typedef enum {
    ROTATE_NONE,
    ROTATE_90,
    ROTATE_180,
    ROTATE_270
} Evas_Object_Image_Rotation;

#define IMG_BYTE_SIZE_ARGB8888(stride, height, has_alpha)	\
  ((stride) * (height) * 4)

#define IMG_BYTE_SIZE_RGB565(stride, height, has_alpha)	\
  ((stride) * (height) * (!(has_alpha) ? 2 : 3))

void evas_object_image_rotate(Evas_Object *image, Evas_Object_Image_Rotation rotation);
int evas_object_image_mask_fill(Evas_Object *src, Evas_Object *mask, Evas_Object *surface, int x_mask, int y_mask, int x_surface, int y_surface);


#endif /* __EVAS_OBJECT_IMAGE_PYTHON_EXTRAS__ */
