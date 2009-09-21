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
#ifndef EON_RECT_H_
#define EON_RECT_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_RECT_CORNERS_CHANGED "cornersChanged"
#define EON_RECT_CORNER_RADIUS_CHANGED "cornerRadiusChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_RECT_CORNERS;
extern Ekeko_Property_Id EON_RECT_CORNER_RADIUS;
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
EAPI void eon_rect_corner_radius_set(Eon_Rect *r, float rad);
EAPI float eon_rect_corner_radius_get(Eon_Rect *r);

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
#define eon_rect_color_set(r, c) eon_paint_color_set((Eon_Paint *)(r), c)
#define eon_rect_color_get(r) eon_paint_color_get((Eon_Paint *)(r))
#define eon_rect_rop_set(r, o) eon_shape_rop_set((Eon_Shape *)(r), o)
#define eon_rect_rop_get(r) eon_shape_rop_get((Eon_Shape *)(r))
#define eon_rect_fill_paint_set(r, p) eon_shape_fill_paint_set((Eon_Shape *)r, p)
#define eon_rect_fill_paint_get(r) eon_shape_fill_paint_get((Eon_Shape *)r)
#define eon_rect_fill_color_set(r, c) eon_shape_fill_color_set((Eon_Shape *)r, c)
#define eon_rect_fill_color_get(r) eon_shape_fill_color_get((Eon_Shape *)r)
/* renderable wrappers */
#define eon_rect_show(r) ekeko_renderable_show(EKEKO_RENDERABLE((r)))
#define eon_rect_hide(r) ekeko_renderable_hide(EKEKO_RENDERABLE((r)))

#endif /* EON_RECT_H_ */
