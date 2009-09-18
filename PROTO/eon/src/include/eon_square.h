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
#ifndef EON_SQUARE_H_
#define EON_SQUARE_H_

/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_SQUARE_X_CHANGED "xChanged"
#define EON_SQUARE_Y_CHANGED "yChanged"
#define EON_SQUARE_W_CHANGED "wChanged"
#define EON_SQUARE_H_CHANGED "hChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_SQUARE_X;
extern Ekeko_Property_Id EON_SQUARE_Y;
extern Ekeko_Property_Id EON_SQUARE_W;
extern Ekeko_Property_Id EON_SQUARE_H;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Square_Private Eon_Square_Private;
struct _Eon_Square
{
	Eon_Shape parent;
	Eon_Square_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_square_type_get(void);
EAPI void eon_square_x_rel_set(Eon_Square *r, int x);
EAPI void eon_square_x_set(Eon_Square *r, int x);
EAPI void eon_square_y_set(Eon_Square *r, int y);
EAPI void eon_square_y_rel_set(Eon_Square *r, int y);
EAPI void eon_square_w_set(Eon_Square *r, int w);
EAPI void eon_square_w_rel_set(Eon_Square *r, int w);
EAPI void eon_square_h_set(Eon_Square *r, int h);
EAPI void eon_square_h_rel_set(Eon_Square *r, int h);
EAPI void eon_square_coords_get(Eon_Square *s, Eon_Coord *x, Eon_Coord *y, Eon_Coord *w, Eon_Coord *h);


#endif /* EON_SQUARE_H_ */
