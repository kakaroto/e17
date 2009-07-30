/*
 * eon_rect.h
 *
 *  Created on: 04-feb-2009
 *      Author: jl
 */

#ifndef EON_RECT_H_
#define EON_RECT_H_
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Rect_Private Eon_Rect_Private;
struct _Eon_Rect
{
	Eon_Square parent;
	Eon_Rect_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_rect_type_get(void);
EAPI Eon_Rect * eon_rect_new(Eon_Canvas *c);

/* square wrappers */
#define eon_rect_x_rel_set(r, x) eon_square_x_rel_set((Eon_Square *)(r), x)
#define eon_rect_y_rel_set(r, y) eon_square_y_rel_set((Eon_Square *)(r), y)
#define eon_rect_w_rel_set(r, w) eon_square_w_rel_set((Eon_Square *)(r), w)
#define eon_rect_h_rel_set(r, h) eon_square_h_rel_set((Eon_Square *)(r), h)
#define eon_rect_x_set(r, x) eon_square_x_set((Eon_Square *)(r), x)
#define eon_rect_y_set(r, y) eon_square_y_set((Eon_Square *)(r), y)
#define eon_rect_w_set(r, w) eon_square_w_set((Eon_Square *)(r), w)
#define eon_rect_h_set(r, h) eon_square_h_set((Eon_Square *)(r), h)
/* shape wrappers */
#define eon_rect_color_set(r, c) eon_shape_color_set((Eon_Shape *)(r), c)
#define eon_rect_color_get(r) eon_shape_color_get((Eon_Shape *)(r))
#define eon_rect_rop_set(r, o) eon_shape_rop_set((Eon_Shape *)(r), o)
#define eon_rect_rop_get(r) eon_shape_rop_get((Eon_Shape *)(r))
/* renderable wrappers */
#define eon_rect_show(r) ekeko_renderable_show(EKEKO_RENDERABLE((r)))
#define eon_rect_hide(r) ekeko_renderable_hide(EKEKO_RENDERABLE((r)))

#endif /* EON_RECT_H_ */
