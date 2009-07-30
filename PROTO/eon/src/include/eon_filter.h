/*
 * eon_image.h
 *
 *  Created on: 26-feb-2009
 *      Author: jl
 */
#ifndef EON_FILTER_H_
#define EON_FILTER_H_

/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_FILTER_X;
extern Ekeko_Property_Id EON_FILTER_Y;
extern Ekeko_Property_Id EON_FILTER_W;
extern Ekeko_Property_Id EON_FILTER_H;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Filter_Private Eon_Filter_Private;
struct _Eon_Filter
{
	Ekeko_Object parent;
	Eon_Filter_Private *private;
	Eina_Bool (*get_pixel)(Eon_Filter *f, Eon_Shape *shape, uint32_t *data, int x, int y);
	Eina_Bool (*get_scanline)(Eon_Filter *f, Eon_Shape *shape, uint32_t *data, unsigned int len, int x, int y);
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_filter_type_get(void);

#endif
