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
#ifndef EON_ANIM_BASIC_H_
#define EON_ANIM_BASIC_H_
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Animation_Basic_Private Eon_Animation_Basic_Private;
struct _Eon_Animation_Basic
{
	Eon_Animation parent;
	Eon_Animation_Basic_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type * eon_animation_basic_type_get(void);
EAPI Eon_Animation_Basic * eon_animation_basic_new(void);

#endif /* EON_ANIM_BASIC_H_ */
