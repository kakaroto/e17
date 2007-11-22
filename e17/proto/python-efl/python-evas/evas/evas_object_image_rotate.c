#include <stdio.h>
#include <stdlib.h>
#include <Evas.h>

#include "evas_object_image_rotate.h"

static inline int
_calc_stride(int w)
{
   int pad;

   pad = w % 4;
   if (!pad)  return w;
   else return w + 4 - pad;
}

static void
_data8_image_rotate_90(char *dst, const char *src,
                       int dst_stride, int src_stride,
                       int out_x, int out_y, int w, int h)
{
    DATA8 *dp, *sp;
    int x, y;

    sp = (DATA8 *) src;
    dp = ((DATA8 *) dst) + ((out_x +
         (w + out_y - 1) * dst_stride));

    for (y = 0; y < h; y++) {
        DATA8 *dp_itr, *sp_itr;

        sp_itr = sp;
        dp_itr = dp;

        for (x = 0; x < w; x++) {
            *dp_itr = *sp_itr;

            sp_itr++;
            dp_itr -= dst_stride;
        }
        sp += src_stride;
        dp++;
    }
}

static void
_data16_image_rotate_90(char *dst, const char *src,
                        int dst_stride, int src_stride,
                        int out_x, int out_y, int w, int h)
{
    DATA16 *dp, *sp;
    int x, y;

    sp = (DATA16 *) src;
    dp = ((DATA16 *) dst) + ((out_x +
         (w + out_y - 1) * dst_stride));

    for (y = 0; y < h; y++) {
        DATA16 *dp_itr, *sp_itr;

        sp_itr = sp;
        dp_itr = dp;

        for (x = 0; x < w; x++) {
            *dp_itr = *sp_itr;

            sp_itr++;
            dp_itr -= dst_stride;
        }
        sp += src_stride;
        dp++;
    }
}

static void
_data32_image_rotate_90(char *dst, const char *src,
                        int dst_stride, int src_stride,
                        int out_x, int out_y, int w, int h)
{
    DATA32 *dp, *sp;
    int x, y;

    sp = (DATA32 *) src;
    dp = ((DATA32 *) dst) + ((out_x +
         (w + out_y - 1) * dst_stride));

    for (y = 0; y < h; y++) {
        DATA32 *dp_itr, *sp_itr;

        sp_itr = sp;
        dp_itr = dp;

        for (x = 0; x < w; x++) {
            *dp_itr = *sp_itr;

            sp_itr++;
            dp_itr -= dst_stride;
        }
        sp += src_stride;
        dp++;
    }
}

static void
_data8_image_rotate_180(char *dst, const char *src,
                        int dst_stride, int src_stride,
                        int out_x, int out_y, int w, int h)
{
    DATA8 *dp, *sp;
    int x, y;

    sp = (DATA8 *) src;
    dp = ((DATA8 *) dst) + ((w + out_x - 1) +
         (h + out_y - 1) * dst_stride);

    for (y = 0; y < h; y++) {
        DATA8 *dp_itr, *sp_itr;
        sp_itr = sp;
        dp_itr = dp;

        for (x = 0; x < w; x++) {
             *dp_itr = *sp_itr;

             sp_itr++;
             dp_itr--;
        }
        sp += src_stride;
        dp -= dst_stride;
    }
}

static void
_data16_image_rotate_180(char *dst, const char *src,
                         int dst_stride, int src_stride,
                         int out_x, int out_y, int w, int h)
{
    DATA16 *dp, *sp;
    int x, y;

    sp = (DATA16 *) src;
    dp = ((DATA16 *) dst) + ((w + out_x - 1) +
         (h + out_y - 1) * dst_stride);

    for (y = 0; y < h; y++) {
        DATA16 *dp_itr, *sp_itr;
        sp_itr = sp;
        dp_itr = dp;

        for (x = 0; x < w; x++) {
             *dp_itr = *sp_itr;

             sp_itr++;
             dp_itr--;
        }
        sp += src_stride;
        dp -= dst_stride;
    }
}
static void
_data32_image_rotate_180(char *dst, const char *src,
                         int dst_stride, int src_stride,
                         int out_x, int out_y, int w, int h)
{
    DATA32 *dp, *sp;
    int x, y;

    sp = (DATA32 *) src;
    dp = ((DATA32 *) dst) + ((w + out_x - 1) +
         (h + out_y - 1) * dst_stride);

    for (y = 0; y < h; y++) {
        DATA32 *dp_itr, *sp_itr;
        sp_itr = sp;
        dp_itr = dp;

        for (x = 0; x < w; x++) {
             *dp_itr = *sp_itr;

             sp_itr++;
             dp_itr--;
        }
        sp += src_stride;
        dp -= dst_stride;
    }
}

static void
_data8_image_rotate_270(char *dst, const char *src,
                        int dst_stride, int src_stride,
                        int out_x, int out_y, int w, int h)
{
    DATA8 *dp, *sp;
    int x, y;

    sp = (DATA8 *) src;
    dp = ((DATA8 *) dst) + ((h + out_x - 1) +
         out_y * dst_stride);

    for (y = 0; y < h; y++) {
        DATA8 *dp_itr, *sp_itr;
        sp_itr = sp;
        dp_itr = dp;

        for (x = 0; x < w; x++) {
             *dp_itr = *sp_itr;

             sp_itr++;
             dp_itr += dst_stride;
        }
        sp += src_stride;
        dp--;
    }
}

static void
_data16_image_rotate_270(char *dst, const char *src,
                         int dst_stride, int src_stride,
                         int out_x, int out_y, int w, int h)
{
    DATA16 *dp, *sp;
    int x, y;

    sp = (DATA16 *) src;
    dp = ((DATA16 *) dst) + ((h + out_x - 1) +
         out_y * dst_stride);

    for (y = 0; y < h; y++) {
        DATA16 *dp_itr, *sp_itr;
        sp_itr = sp;
        dp_itr = dp;

        for (x = 0; x < w; x++) {
             *dp_itr = *sp_itr;

             sp_itr++;
             dp_itr += dst_stride;
        }
        sp += src_stride;
        dp--;
    }
}

static void
_data32_image_rotate_270(char *dst, const char *src,
                         int dst_stride, int src_stride,
                         int out_x, int out_y, int w, int h)
{
    DATA32 *dp, *sp;
    int x, y;

    sp = (DATA32 *) src;
    dp = ((DATA32 *) dst) + ((h + out_x - 1) +
         out_y * dst_stride);

    for (y = 0; y < h; y++) {
        DATA32 *dp_itr, *sp_itr;
        sp_itr = sp;
        dp_itr = dp;

        for (x = 0; x < w; x++) {
             *dp_itr = *sp_itr;

             sp_itr++;
             dp_itr += dst_stride;
        }
        sp += src_stride;
        dp--;
    }
}

static void
_rgb565_image_rotate(Rotation rotation,
                     char *dst, const char *src,
                     int src_stride, unsigned char has_alpha,
                     int out_x, int out_y, int w, int h)
{
    int dst_stride;

    char *dst_alpha;
    const char *src_alpha;

    if(has_alpha) {
        dst_alpha = &dst[src_stride * h * 2];
        src_alpha = &src[h * w * 2];
    }

    switch(rotation) {
        case ROTATE_90:
            dst_stride = _calc_stride(h);
            _data16_image_rotate_90(dst, src,
                                    dst_stride, src_stride,
                                    out_x, out_y, w, h);
            if(has_alpha)
                _data8_image_rotate_90(dst_alpha, src_alpha,
                                       dst_stride, src_stride,
                                       out_x, out_y, w, h);
            break;
        case ROTATE_180:
            dst_stride = _calc_stride(src_stride);
            _data16_image_rotate_180(dst, src,
                                     dst_stride, src_stride,
                                     out_x, out_y, w, h);
            if(has_alpha)
                _data8_image_rotate_180(dst_alpha, src_alpha,
                                        dst_stride, src_stride,
                                        out_x, out_y, w, h);
            break;
        case ROTATE_270:
            dst_stride = _calc_stride(h);
            _data16_image_rotate_270(dst, src,
                                     dst_stride, src_stride,
                                     out_x, out_y, w, h);
            if(has_alpha)
                _data8_image_rotate_270(dst_alpha, src_alpha,
                                        dst_stride, src_stride,
                                        out_x, out_y, w, h);
            break;
    }
}

static void
_argb8888_image_rotate(Rotation rotation,
                       char *dst, const char *src,
                       int src_stride,
                       int out_x, int out_y, int w, int h)
{
    int dst_stride;

    switch(rotation) {
        case ROTATE_90:
            dst_stride = h;
            _data32_image_rotate_90(dst, src,
                                    dst_stride, src_stride,
                                    out_x, out_y, w, h);
            break;
        case ROTATE_180:
            dst_stride = src_stride;
            _data32_image_rotate_180(dst, src,
                                     dst_stride, src_stride,
                                     out_x, out_y, w, h);
            break;
        case ROTATE_270:
            dst_stride = h;
            _data32_image_rotate_270(dst, src,
                                     dst_stride, src_stride,
                                     out_x, out_y, w, h);
            break;
    }
}

void
evas_object_image_rotate(Evas_Object *image, Rotation rotation)
{
    Evas_Colorspace colorspace = evas_object_image_colorspace_get(image);
    int image_byte_size;
    int stride, width, height;
    unsigned char has_alpha;
    char *new_buffer;
    char *src_data;

    evas_object_image_size_get(image, &width, &height);
    stride = evas_object_image_stride_get(image);
    has_alpha = evas_object_image_alpha_get(image);

    switch(colorspace) {
        case EVAS_COLORSPACE_ARGB8888:
            image_byte_size = IMG_BYTE_SIZE_ARGB8888(stride, height, stride);
            if(image_byte_size <= 0)
                return;

            new_buffer = (char*) malloc(image_byte_size);
            src_data = (char*) evas_object_image_data_get(image, FALSE);

            /* dst_stride set to original height */
            _argb8888_image_rotate(rotation, new_buffer, src_data,
                                   stride, 0, 0, width, height);
            break;
        case EVAS_COLORSPACE_RGB565_A5P:
            image_byte_size = IMG_BYTE_SIZE_RGB565(stride, height, stride);
            if(image_byte_size <= 0)
                return;

            new_buffer = (char*) malloc(image_byte_size);
            src_data = (char*) evas_object_image_data_get(image, FALSE);

            /* dst_stride set to original height */
            _rgb565_image_rotate(rotation, new_buffer, src_data,
                                 stride, has_alpha, 0, 0, width, height);
            break;
        default:
            return;
    }

    if (rotation == ROTATE_90 || rotation == ROTATE_270)
        evas_object_image_size_set(image, height, width);

    evas_object_image_data_update_add(image, 0, 0, width, height);
    evas_object_image_data_copy_set(image, new_buffer);

    free(new_buffer);
}
