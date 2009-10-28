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
#ifndef EON_GRID_H_
#define EON_GRID_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_GRID_COLOR1_CHANGED "color1Changed"
#define EON_GRID_COLOR2_CHANGED "color2Changed"
#define EON_GRID_VSPACE_CHANGED "vspaceChanged"
#define EON_GRID_HSPACE_CHANGED "hspaceChanged"
#define EON_GRID_VTHICK_CHANGED "vthickChanged"
#define EON_GRID_HTHICK_CHANGED "hthickChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_GRID_COLOR1;
extern Ekeko_Property_Id EON_GRID_COLOR2;
extern Ekeko_Property_Id EON_GRID_VSPACE;
extern Ekeko_Property_Id EON_GRID_HSPACE;
extern Ekeko_Property_Id EON_GRID_VTHICK;
extern Ekeko_Property_Id EON_GRID_HTHICK;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Grid_Private Eon_Grid_Private;
struct _Eon_Grid
{
	Eon_Paint_Square parent;
	Eon_Grid_Private *prv;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Eon_Grid * eon_grid_new(Eon_Document *d);
EAPI Eon_Color eon_grid_color1_get(Eon_Grid *sq);
EAPI Eon_Color eon_grid_color2_get(Eon_Grid *sq);
EAPI void eon_grid_color1_set(Eon_Grid *sq, Eon_Color color);
EAPI void eon_grid_color2_set(Eon_Grid *sq, Eon_Color color);

#endif /* EON_GRID_H_ */
