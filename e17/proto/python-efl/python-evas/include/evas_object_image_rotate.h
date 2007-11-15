#ifndef __EVAS_OBJ_IMAGE_ROTATE__
#define __EVAS_OBJ_IMAGE_ROTATE__

typedef unsigned char DATA8;
typedef short int DATA16;
typedef int DATA32;

enum {
    FALSE,
    TRUE
};

typedef enum {
    ROTATE_NONE,
    ROTATE_90,
    ROTATE_180,
    ROTATE_270
} Rotation;

#define IMG_BYTE_SIZE_ARGB8888(stride, height, has_alpha)                     \
            ((stride) * (height) * 4)

#define IMG_BYTE_SIZE_RGB565(stride, height, has_alpha)                       \
            ((stride) * (height) * (!(has_alpha) ? 2 : 3))

void evas_object_image_rotate(Evas_Object *image, Rotation rotation);

#endif
