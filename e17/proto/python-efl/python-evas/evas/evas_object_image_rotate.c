/*
 * Copyright (C) 2007-2008 Gustavo Sverzut Barbieri, Leonardo Sobral, Ulisses Furquim
 *
 * This file is part of Python-Evas.
 *
 * Python-Evas is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Python-Evas is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this Python-Evas.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <Evas.h>

#include "evas/evas_object_image_python_extras.h"

static inline int
_calc_stride(int w)
{
   int pad;

   pad = w % 4;
   if (!pad) return w;
   else return w + 4 - pad;
}

static inline int
_calc_image_byte_size(Evas_Colorspace colorspace, Evas_Object_Image_Rotation rotation, int stride, int w, int h, unsigned char has_alpha)
{
   int dst_stride, dst_height;
   int image_byte_size;

   if ((rotation == ROTATE_90) || (rotation == ROTATE_270))
     {
        dst_stride = _calc_stride(h);
        dst_height = w;
     }
   else
     {
        dst_stride = stride;
        dst_height = h;
     }

   switch (colorspace)
     {
      case EVAS_COLORSPACE_ARGB8888:
	 image_byte_size = IMG_BYTE_SIZE_ARGB8888(dst_stride, dst_height,
						  has_alpha);
	 break;
      case EVAS_COLORSPACE_RGB565_A5P:
	 image_byte_size = IMG_BYTE_SIZE_RGB565(dst_stride, dst_height,
						has_alpha);
	 break;
      default:
	 image_byte_size = -1;
	 break;
     }

   return image_byte_size;
}

static void
_data8_image_rotate_90(DATA8 *dst, const DATA8 *src, int dst_stride, int src_stride, int out_x, int out_y, int w, int h)
{
   const DATA8 *sp;
   DATA8 *dp;
   int y;

   sp = src;
   dp = dst + ((out_x + (w + out_y - 1) * dst_stride));

   for (y = 0; y < h; y++)
     {
	const DATA8 *sp_itr;
        DATA8 *dp_itr;
	int x;

        sp_itr = sp;
        dp_itr = dp;

        for (x = 0; x < w; x++)
	  {
	     *dp_itr = *sp_itr;

	     sp_itr++;
	     dp_itr -= dst_stride;
	  }
        sp += src_stride;
        dp++;
     }
}

static void
_data16_image_rotate_90(DATA16 *dst, const DATA16 *src, int dst_stride, int src_stride, int out_x, int out_y, int w, int h)
{
   const DATA16 *sp;
   DATA16 *dp;
   int y;

   sp = src;
   dp = dst + ((out_x + (w + out_y - 1) * dst_stride));

   for (y = 0; y < h; y++)
     {
	const DATA16 *sp_itr;
        DATA16 *dp_itr;
	int x;

        sp_itr = sp;
        dp_itr = dp;

        for (x = 0; x < w; x++)
	  {
	     *dp_itr = *sp_itr;

	     sp_itr++;
	     dp_itr -= dst_stride;
	  }
        sp += src_stride;
        dp++;
     }
}

static void
_data32_image_rotate_90(DATA32 *dst, const DATA32 *src, int dst_stride, int src_stride, int out_x, int out_y, int w, int h)
{
   const DATA32 *sp;
   DATA32 *dp;
   int y;

   sp = src;
   dp = dst + ((out_x + (w + out_y - 1) * dst_stride));

   for (y = 0; y < h; y++)
     {
	const DATA32 *sp_itr;
        DATA32 *dp_itr;
	int x;

        sp_itr = sp;
        dp_itr = dp;

        for (x = 0; x < w; x++)
	  {
	     *dp_itr = *sp_itr;

	     sp_itr++;
	     dp_itr -= dst_stride;
	  }
        sp += src_stride;
        dp++;
     }
}

static void
_data8_image_rotate_180(DATA8 *dst, const DATA8 *src, int dst_stride, int src_stride, int out_x, int out_y, int w, int h)
{
   const DATA8 *sp;
   DATA8 *dp;
   int y;

   sp = src;
   dp = dst + ((w + out_x - 1) + (h + out_y - 1) * dst_stride);

   for (y = 0; y < h; y++)
     {
	const DATA8 *sp_itr;
        DATA8 *dp_itr;
	int x;

        sp_itr = sp;
        dp_itr = dp;

        for (x = 0; x < w; x++)
	  {
             *dp_itr = *sp_itr;

             sp_itr++;
             dp_itr--;
	  }
        sp += src_stride;
        dp -= dst_stride;
     }
}

static void
_data16_image_rotate_180(DATA16 *dst, const DATA16 *src, int dst_stride, int src_stride, int out_x, int out_y, int w, int h)
{
   const DATA16 *sp;
   DATA16 *dp;
   int y;

   sp = src;
   dp = dst + ((w + out_x - 1) + (h + out_y - 1) * dst_stride);

   for (y = 0; y < h; y++)
     {
	const DATA16 *sp_itr;
        DATA16 *dp_itr;
	int x;

        sp_itr = sp;
        dp_itr = dp;

        for (x = 0; x < w; x++)
	  {
             *dp_itr = *sp_itr;

             sp_itr++;
             dp_itr--;
	  }
        sp += src_stride;
        dp -= dst_stride;
     }
}

static void
_data32_image_rotate_180(DATA32 *dst, const DATA32 *src, int dst_stride, int src_stride, int out_x, int out_y, int w, int h)
{
   const DATA32 *sp;
   DATA32 *dp;
   int y;

   sp = src;
   dp = dst + ((w + out_x - 1) + (h + out_y - 1) * dst_stride);

   for (y = 0; y < h; y++)
     {
	const DATA32 *sp_itr;
        DATA32 *dp_itr;
	int x;

        sp_itr = sp;
        dp_itr = dp;

        for (x = 0; x < w; x++)
	  {
             *dp_itr = *sp_itr;

             sp_itr++;
             dp_itr--;
	  }
        sp += src_stride;
        dp -= dst_stride;
     }
}

static void
_data8_image_rotate_270(DATA8 *dst, const DATA8 *src, int dst_stride, int src_stride, int out_x, int out_y, int w, int h)
{
   const DATA8 *sp;
   DATA8 *dp;
   int y;

   sp = src;
   dp = dst + ((h + out_x - 1) + out_y * dst_stride);

   for (y = 0; y < h; y++)
     {
	const DATA8 *sp_itr;
        DATA8 *dp_itr;
	int x;

        sp_itr = sp;
        dp_itr = dp;

        for (x = 0; x < w; x++)
	  {
             *dp_itr = *sp_itr;

             sp_itr++;
             dp_itr += dst_stride;
	  }
        sp += src_stride;
        dp--;
     }
}

static void
_data16_image_rotate_270(DATA16 *dst, const DATA16 *src, int dst_stride, int src_stride, int out_x, int out_y, int w, int h)
{
   const DATA16 *sp;
   DATA16 *dp;
   int y;

   sp = src;
   dp = dst + ((h + out_x - 1) + out_y * dst_stride);

   for (y = 0; y < h; y++)
     {
	const DATA16 *sp_itr;
        DATA16 *dp_itr;
	int x;

        sp_itr = sp;
        dp_itr = dp;

        for (x = 0; x < w; x++)
	  {
             *dp_itr = *sp_itr;

             sp_itr++;
             dp_itr += dst_stride;
	  }
        sp += src_stride;
        dp--;
     }
}

static void
_data32_image_rotate_270(DATA32 *dst, const DATA32 *src, int dst_stride, int src_stride, int out_x, int out_y, int w, int h)
{
   const DATA32 *sp;
   DATA32 *dp;
   int y;

   sp = src;
   dp = dst + ((h + out_x - 1) + out_y * dst_stride);

   for (y = 0; y < h; y++)
     {
	const DATA32 *sp_itr;
        DATA32 *dp_itr;
	int x;

        sp_itr = sp;
        dp_itr = dp;

        for (x = 0; x < w; x++)
	  {
             *dp_itr = *sp_itr;

             sp_itr++;
             dp_itr += dst_stride;
	  }
        sp += src_stride;
        dp--;
     }
}

static void
_rgb565_image_rotate(Evas_Object_Image_Rotation rotation, DATA16 *dst, const DATA16 *src, int src_stride, unsigned char has_alpha, int out_x, int out_y, int w, int h)
{
   int dst_stride;
   DATA8 *dst_alpha;
   const DATA8 *src_alpha;

   if (has_alpha)
     src_alpha = (const DATA8*)(src + src_stride * h);

   switch (rotation)
     {
      case ROTATE_90:
	 dst_stride = _calc_stride(h);
	 _data16_image_rotate_90(dst, src,
				 dst_stride, src_stride,
				 out_x, out_y, w, h);
	 if (has_alpha)
	   {
	      dst_alpha = (DATA8 *)(dst + dst_stride * w);
	      _data8_image_rotate_90(dst_alpha, src_alpha,
				     dst_stride, src_stride,
				     out_x, out_y, w, h);
	   }
	 break;
      case ROTATE_180:
	 dst_stride = src_stride;
	 _data16_image_rotate_180(dst, src,
				  dst_stride, src_stride,
				  out_x, out_y, w, h);
	 if (has_alpha)
	   {
	      dst_alpha = (DATA8 *)(dst + dst_stride * h);
	      _data8_image_rotate_180(dst_alpha, src_alpha,
				      dst_stride, src_stride,
				      out_x, out_y, w, h);
	   }
	 break;
      case ROTATE_270:
	 dst_stride = _calc_stride(h);
	 _data16_image_rotate_270(dst, src,
				  dst_stride, src_stride,
				  out_x, out_y, w, h);
	 if (has_alpha)
	   {
	      dst_alpha = (DATA8 *)(dst + dst_stride * w);
	      _data8_image_rotate_270(dst_alpha, src_alpha,
				      dst_stride, src_stride,
				      out_x, out_y, w, h);
	   }
	 break;
      case ROTATE_NONE:
	 break;
     }
}

static void
_argb8888_image_rotate(Evas_Object_Image_Rotation rotation, DATA32 *dst, const DATA32 *src, int src_stride, int out_x, int out_y, int w, int h)
{
   int dst_stride;

   switch(rotation)
     {
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
      case ROTATE_NONE:
	 break;
     }
}

void
evas_object_image_rotate(Evas_Object *image, Evas_Object_Image_Rotation rotation)
{
   Evas_Colorspace colorspace = evas_object_image_colorspace_get(image);
   int image_byte_size;
   int stride, width, height;
   unsigned char has_alpha;
   void *new_buffer, *src_data;

   evas_object_image_size_get(image, &width, &height);
   stride = evas_object_image_stride_get(image);
   has_alpha = evas_object_image_alpha_get(image);

   image_byte_size = _calc_image_byte_size(colorspace, rotation,
					   stride, width,
					   height, has_alpha);
   if (image_byte_size <= 0)
     return;

   new_buffer = malloc(image_byte_size);
   if (!new_buffer)
     return;

   src_data = evas_object_image_data_get(image, 0);

   switch (colorspace)
     {
      case EVAS_COLORSPACE_ARGB8888:
	 _argb8888_image_rotate(rotation, new_buffer, src_data,
				stride, 0, 0, width, height);
	 break;
      case EVAS_COLORSPACE_RGB565_A5P:
	 _rgb565_image_rotate(rotation, new_buffer, src_data,
			      stride, has_alpha, 0, 0, width, height);
	 break;
      case EVAS_COLORSPACE_YCBCR422P601_PL:
	 fputs("Colorspace YCBCR422P601_PL not handled\n", stderr);
	 break;
      case EVAS_COLORSPACE_YCBCR422P709_PL:
	 fputs("Colorspace YCBCR422P709_PL not handled\n", stderr);
	 break;
     }

   if ((rotation == ROTATE_90) || (rotation == ROTATE_270))
     evas_object_image_size_set(image, height, width);

   evas_object_image_data_update_add(image, 0, 0, width, height);
   evas_object_image_data_copy_set(image, new_buffer);

   free(new_buffer);
}
