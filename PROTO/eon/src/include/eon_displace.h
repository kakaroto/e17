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
#ifndef EON_DISPLACE_H_
#define EON_DISPLACE_H_

/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_DISPLACE_SRC;
extern Ekeko_Property_Id EON_DISPLACE_MAP;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Displace_Private Eon_Displace_Private;
struct _Eon_Displace
{
	Eon_Filter parent;
	Eon_Displace_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_displace_type_get(void);
EAPI Eon_Displace * eon_displace_new(Eon_Canvas *c);

#endif /* EON_DISPLACE_H_ */
