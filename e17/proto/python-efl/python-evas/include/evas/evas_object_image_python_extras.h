/*
 * Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
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
