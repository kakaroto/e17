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
#ifndef EON_POLYGON_H_
#define EON_POLYGON_H_
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Polygon_Private Eon_Polygon_Private;
struct _Eon_Polygon
{
	Eon_Shape parent;
	Eon_Polygon_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_polygon_type_get(void);
EAPI Eon_Polygon * eon_polygon_new(Eon_Canvas *c);

#endif /* EON_POLYGON_H_ */
