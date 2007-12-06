#ifndef __EVAS_OBJ_IMAGE_MASK__
#define __EVAS_OBJ_IMAGE_MASK__

#define IMG_BYTE_SIZE_ARGB8888(stride, height, has_alpha)                     \
            ((stride) * (height) * 4)

#define IMG_BYTE_SIZE_RGB565(stride, height, has_alpha)                       \
            ((stride) * (height) * (!(has_alpha) ? 2 : 3))

int evas_object_image_mask_fill(Evas_Object *src, Evas_Object *mask,
    Evas_Object *surface, int x_mask, int y_mask, int x_surface, int y_surface);

#endif
