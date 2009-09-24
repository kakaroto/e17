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
#ifndef EON_STRIPES_H_
#define EON_STRIPES_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_STRIPES_COLOR1;
extern Ekeko_Property_Id EON_STRIPES_COLOR2;
extern Ekeko_Property_Id EON_STRIPES_THICKNESS1;
extern Ekeko_Property_Id EON_STRIPES_THICKNESS2;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Stripes_Private Eon_Stripes_Private;
struct _Eon_Stripes
{
	Eon_Paint_Square parent;
	Eon_Stripes_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Eon_Stripes * eon_stripes_new(Eon_Document *d);
EAPI Eon_Color eon_stripes_color1_get(Eon_Stripes *sq);
EAPI Eon_Color eon_stripes_color2_get(Eon_Stripes *sq);
EAPI void eon_stripes_color1_set(Eon_Stripes *sq, Eon_Color color);
EAPI void eon_stripes_color2_set(Eon_Stripes *sq, Eon_Color color);

EAPI void eon_stripes_thickness1_set(Eon_Stripes *s, float th);
EAPI float eon_stripes_thickness1_get(Eon_Stripes *s);
EAPI void eon_stripes_thickness2_set(Eon_Stripes *s, float th);
EAPI float eon_stripes_thickness2_get(Eon_Stripes *s);

#endif /* EON_STRIPES_H_ */
