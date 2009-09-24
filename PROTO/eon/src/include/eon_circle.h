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
#ifndef EON_CIRCLE_H_
#define EON_CIRCLE_H_

/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_CIRCLE_X_CHANGED "xChanged"
#define EON_CIRCLE_Y_CHANGED "yChanged"
#define EON_CIRCLE_RADIUS_CHANGED "radiusChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_CIRCLE_X;
extern Ekeko_Property_Id EON_CIRCLE_Y;
extern Ekeko_Property_Id EON_CIRCLE_RADIUS;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Circle_Private Eon_Circle_Private;
struct _Eon_Circle
{
	Eon_Shape parent;
	Eon_Circle_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Eon_Circle * eon_circle_new(Eon_Document *d);
EAPI void eon_circle_x_get(Eon_Circle *c, Eon_Coord *coord);
EAPI void eon_circle_x_rel_set(Eon_Circle *c, int x);
EAPI void eon_circle_x_set(Eon_Circle *c, int x);
EAPI void eon_circle_y_get(Eon_Circle *c, Eon_Coord *coord);
EAPI void eon_circle_y_set(Eon_Circle *c, int y);
EAPI void eon_circle_y_rel_set(Eon_Circle *c, int y);
EAPI float eon_circle_radius_get(Eon_Circle *c);
EAPI void eon_circle_radius_set(Eon_Circle *c, float r);
EAPI void eon_circle_radius_rel_set(Eon_Circle *c, int r);

/* shape wrappers */
#define eon_circle_color_set(r, c) eon_shape_color_set((Eon_Shape *)(r), c)
#define eon_circle_color_get(r) eon_shape_color_get((Eon_Shape *)(r))
#define eon_circle_rop_set(r, o) eon_shape_rop_set((Eon_Shape *)(r), o)
#define eon_circle_rop_get(r) eon_shape_rop_get((Eon_Shape *)(r))
/* renderable wrappers */
#define eon_circle_show(r) ekeko_renderable_show(EKEKO_RENDERABLE((r)))
#define eon_circle_hide(r) ekeko_renderable_hide(EKEKO_RENDERABLE((r)))

#endif /* EON_CIRCLE_H_ */
