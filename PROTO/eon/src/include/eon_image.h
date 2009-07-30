/*
 * eon_image.h
 *
 *  Created on: 26-feb-2009
 *      Author: jl
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
	Eon_Paint parent;
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
