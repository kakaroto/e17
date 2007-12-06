#include <stdio.h>
#include <stdlib.h>
#include <Evas.h>

#include "evas_object_image_mask.h"

typedef unsigned char DATA8;
typedef short int DATA16;
typedef int DATA32;

enum {FALSE, TRUE};


#define BPIXEL(base, x, y, stride) (base + (y * stride + x))

#define MEDPIXEL(src, msk, srf) (((src) * (msk)) + (srf) * (255 - (msk)))/255


static void
_argb8888_image_mask_fill(char *dst, const char *src,
    const char *msk, const char *srf,
    int x_msk, int y_msk, int x_srf, int y_srf,
    int src_stride, int src_width, int src_height,
    int msk_stride, int msk_width, int msk_height,
    int srf_stride, int srf_width, int srf_height)
{
    int x, y, xp, yp, xs, ys;
    int r, g, b, a;
    DATA32 pixel, pxa, pxb;
    DATA32 *sp = (DATA32 *) src;
    DATA32 *mp = (DATA32 *) msk;
    DATA32 *fp = (DATA32 *) srf;
    DATA32 *dp = (DATA32 *) dst;

    for (y = 0; y < src_height; y++)
        for (x = 0; x < src_width; x++)
        {
            xp = x - x_msk;
            yp = y - y_msk;
            xs = x - x_srf;
            ys = y - y_srf;

            pixel = *(BPIXEL(sp, x, y, src_stride));

            if (xp >= 0 && xp < msk_width && yp >= 0 && yp < msk_height
                && xs >= 0 && xs < srf_width && ys >= 0 && ys < srf_height) {
                pxa = *(BPIXEL(mp, xp, yp, msk_stride));
                pxb = *(BPIXEL(fp, xs, ys, srf_stride));
                if(pxa != 0) {
                    a = MEDPIXEL((pixel >> 24) & 0xFF,
                                 (pxa >> 24) & 0xFF, (pxb >> 24) & 0xFF);
                    r = MEDPIXEL((pixel >> 16) & 0xFF,
                                 (pxa >> 16) & 0xFF, (pxb >> 16) & 0xFF);
                    g = MEDPIXEL((pixel >> 8) & 0xFF,
                                 (pxa >> 8) & 0xFF, (pxb >> 8) & 0xFF);
                    b = MEDPIXEL(pixel & 0xFF, pxa & 0xFF, pxb & 0xFF);
                    pixel = (a << 24) | (r << 16) | (g << 8) | b;
                }
            }

            *(BPIXEL(dp, x, y, src_stride)) = pixel;
        }
}


int
evas_object_image_mask_fill(Evas_Object *src, Evas_Object *mask,
    Evas_Object *surface, int x_mask, int y_mask, int x_surface, int y_surface)
{
    int alloc_size;
    unsigned char has_alpha;
    char *src_data;
    char *msk_data;
    char *srf_data;
    char *new_buffer;
    int src_stride, src_width, src_height;
    int msk_stride, msk_width, msk_height;
    int srf_stride, srf_width, srf_height;

    Evas_Colorspace src_colorspace = evas_object_image_colorspace_get(src);
    Evas_Colorspace msk_colorspace = evas_object_image_colorspace_get(mask);
    Evas_Colorspace srf_colorspace = evas_object_image_colorspace_get(surface);

    if (src_colorspace != msk_colorspace || src_colorspace != srf_colorspace) {
        /* force use the same colorspace */
        return 1;
    }

    evas_object_image_size_get(src, &src_width, &src_height);
    src_stride = evas_object_image_stride_get(src);
    evas_object_image_size_get(mask, &msk_width, &msk_height);
    msk_stride = evas_object_image_stride_get(mask);
    evas_object_image_size_get(surface, &srf_width, &srf_height);
    srf_stride = evas_object_image_stride_get(surface);

    has_alpha = evas_object_image_alpha_get(src);

    switch(src_colorspace) {
        case EVAS_COLORSPACE_ARGB8888:
            alloc_size = IMG_BYTE_SIZE_ARGB8888(src_stride, src_height, src_stride);
            new_buffer = (char *) malloc(alloc_size);
            if(new_buffer == NULL)
                return 3;

            src_data = (char *) evas_object_image_data_get(src, FALSE);
            msk_data = (char *) evas_object_image_data_get(mask, FALSE);
            srf_data = (char *) evas_object_image_data_get(surface, FALSE);

            _argb8888_image_mask_fill(new_buffer, src_data, msk_data,
                srf_data, x_mask, y_mask, x_surface, y_surface,
                src_stride, src_width, src_height,
                msk_stride, msk_width, msk_height,
                srf_stride, srf_width, srf_height);
            break;
        case EVAS_COLORSPACE_RGB565_A5P:
            /* TODO */
            return 2;
        default:
            /* invalid colorspace */
            return 2;
    }

    evas_object_image_data_update_add(src, 0, 0, src_width, src_height);
    evas_object_image_data_copy_set(src, new_buffer);

    free(new_buffer);

    return 0;
}
