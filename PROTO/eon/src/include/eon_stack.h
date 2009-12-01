/* EON - Stack and Toolkit library
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
#ifndef EON_STACK_H_
#define EON_STACK_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_STACK_ORIENTATION_CHANGED "orientationChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
typedef enum _Eon_Stack_Orientation
{
	EON_STACK_VERTICAL,
	EON_STACK_HORIZONTAL,
	EON_STACK_ORIENTATIONS,
} Eon_Stack_Orientation;

extern Ekeko_Property_Id EON_STACK_ORIENTATION;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Stack_Private Eon_Stack_Private;
struct _Eon_Stack
{
	Eon_Layout base;
	Eon_Stack_Private *prv;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Eon_Stack * eon_stack_new(Eon_Document *c);

#endif /* EON_STACK_H_ */
