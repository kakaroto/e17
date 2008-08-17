/*
 * Copyright (C) 2007-2008 Gustavo Sverzut Barbieri, Adriano Rezende
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

#define BPIXEL(base, x, y, stride) (base + (y * stride + x))
#define MEDPIXEL(src, msk, srf) (((src) * (msk)) + (srf) * (255 - (msk)))/255


static void
_argb8888_image_mask_fill(DATA32 *dst, const DATA32 *src, const DATA32 *msk, const DATA32 *srf, int x_msk, int y_msk, int x_srf, int y_srf, int src_stride, int src_width, int src_height, int msk_stride, int msk_width, int msk_height, int srf_stride, int srf_width, int srf_height)
{
   int y;

   for (y = 0; y < src_height; y++)
     {
	int x;

	for (x = 0; x < src_width; x++)
	  {
	     int xp, yp, xs, ys;
	     DATA32 pixel;

	     xp = x - x_msk;
	     yp = y - y_msk;
	     xs = x - x_srf;
	     ys = y - y_srf;

	     pixel = *(BPIXEL(src, x, y, src_stride));

	     if ((xp >= 0) && (xp < msk_width) &&
		 (yp >= 0) && (yp < msk_height) &&
		 (xs >= 0) && (xs < srf_width) &&
		 (ys >= 0) && (ys < srf_height))
	       {
		  DATA32 pxa, pxb;

		  pxa = *(BPIXEL(msk, xp, yp, msk_stride));
		  pxb = *(BPIXEL(srf, xs, ys, srf_stride));
		  if (pxa != 0)
		    {
		       int r, g, b, a;

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

	     *(BPIXEL(dst, x, y, src_stride)) = pixel;
	  }
     }
}

int
evas_object_image_mask_fill(Evas_Object *src, Evas_Object *mask, Evas_Object *surface, int x_mask, int y_mask, int x_surface, int y_surface)
{
   int alloc_size;
   unsigned char has_alpha;
   void *src_data, *msk_data, *srf_data, *new_buffer;
   int src_stride, src_width, src_height;
   int msk_stride, msk_width, msk_height;
   int srf_stride, srf_width, srf_height;

   Evas_Colorspace src_colorspace = evas_object_image_colorspace_get(src);
   Evas_Colorspace msk_colorspace = evas_object_image_colorspace_get(mask);
   Evas_Colorspace srf_colorspace = evas_object_image_colorspace_get(surface);

   if ((src_colorspace != msk_colorspace) || (src_colorspace != srf_colorspace))
     /* force use the same colorspace */
     return 1;

   evas_object_image_size_get(src, &src_width, &src_height);
   src_stride = evas_object_image_stride_get(src);
   src_data = evas_object_image_data_get(src, 0);

   evas_object_image_size_get(mask, &msk_width, &msk_height);
   msk_stride = evas_object_image_stride_get(mask);
   msk_data = evas_object_image_data_get(mask, 0);

   evas_object_image_size_get(surface, &srf_width, &srf_height);
   srf_stride = evas_object_image_stride_get(surface);
   srf_data = evas_object_image_data_get(surface, 0);

   has_alpha = evas_object_image_alpha_get(src);

   switch (src_colorspace)
     {
      case EVAS_COLORSPACE_ARGB8888:
	 alloc_size = IMG_BYTE_SIZE_ARGB8888(src_stride, src_height, src_stride);
	 new_buffer = malloc(alloc_size);
	 if (!new_buffer)
	   return 3;

	 _argb8888_image_mask_fill(new_buffer, src_data, msk_data,
				   srf_data, x_mask, y_mask,
				   x_surface, y_surface,
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
