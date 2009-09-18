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
#ifndef EON_HSWITCH_H_
#define EON_HSWITCH_H_

/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Hswitch_Private Eon_Hswitch_Private;
struct _Eon_Hswitch
{
	Eon_Transition parent;
	Eon_Hswitch_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_hswitch_type_get(void);
EAPI Eon_Hswitch * eon_hswitch_new(void);

/* paint wrappers */
#define eon_hswitch_matrix_set(i, m) eon_paint_matrix_set((Eon_Paint *)(i), m)
#define eon_hswitch_x_rel_set(i, x) eon_paint_x_rel_set((Eon_Paint *)(i), x)
#define eon_hswitch_y_rel_set(i, y) eon_paint_y_rel_set((Eon_Paint *)(i), y)
#define eon_hswitch_w_rel_set(i, w) eon_paint_w_rel_set((Eon_Paint *)(i), w)
#define eon_hswitch_h_rel_set(i, h) eon_paint_h_rel_set((Eon_Paint *)(i), h)
#define eon_hswitch_x_set(i, x) eon_paint_x_set((Eon_Paint *)(i), x)
#define eon_hswitch_y_set(i, y) eon_paint_y_set((Eon_Paint *)(i), y)
#define eon_hswitch_w_set(i, w) eon_paint_w_set((Eon_Paint *)(i), w)
#define eon_hswitch_h_set(i, h) eon_paint_h_set((Eon_Paint *)(i), h)

#endif /* EON_HSWITCH_H_ */
