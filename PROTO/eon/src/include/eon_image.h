/* EON - Canvas and Toolkit library
 * Copyright (C) 2008-2009 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef EON_IMAGE_H_
#define EON_IMAGE_H_

/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_IMAGE_FILE_CHANGED "fileChanged"
#define EON_IMAGE_LOADED_CHANGED "loadedChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_IMAGE_FILE;
extern Ekeko_Property_Id EON_IMAGE_LOADED;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Image_Private Eon_Image_Private;
struct _Eon_Image
{
	Eon_Paint_Square parent;
	Eon_Image_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_image_type_get(void);
EAPI Eon_Image * eon_image_new(void);

EAPI void eon_image_file_set(Eon_Image *i, const char *file);
EAPI const char * eon_image_file_get(Eon_Image *i);
/* paint wrappers */
#define eon_image_matrix_set(i, m) eon_paint_matrix_set((Eon_Paint *)(i), m)
#define eon_image_x_rel_set(i, x) eon_paint_x_rel_set((Eon_Paint *)(i), x)
#define eon_image_y_rel_set(i, y) eon_paint_y_rel_set((Eon_Paint *)(i), y)
#define eon_image_w_rel_set(i, w) eon_paint_w_rel_set((Eon_Paint *)(i), w)
#define eon_image_h_rel_set(i, h) eon_paint_h_rel_set((Eon_Paint *)(i), h)
#define eon_image_x_set(i, x) eon_paint_x_set((Eon_Paint *)(i), x)
#define eon_image_y_set(i, y) eon_paint_y_set((Eon_Paint *)(i), y)
#define eon_image_w_set(i, w) eon_paint_w_set((Eon_Paint *)(i), w)
#define eon_image_h_set(i, h) eon_paint_h_set((Eon_Paint *)(i), h)

#endif /* EON_IMAGE_H_ */
