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
#ifndef EON_BUTTON_H_
#define EON_BUTTON_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_BUTTON_LABEL_CHANGED "labelChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_BUTTON_LABEL;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Button_Private Eon_Button_Private;
struct _Eon_Button
{
	Ekeko_Object parent;
	Eon_Button_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_button_type_get(void);

#endif /* EON_BUTTON_H_ */
