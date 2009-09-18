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
#ifndef EON_TEXT_H_
#define EON_TEXT_H_

/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_TEXT_STR_CHANGED "strChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_TEXT_STR;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Text_Private Eon_Text_Private;
struct _Eon_Text
{
	Eon_Square parent;
	Eon_Text_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_text_type_get(void);
EAPI void eon_text_string_set(Eon_Text *t, const char *str);
EAPI const char * eon_text_string_get(Eon_Text *t);

#endif /* EON_TEXT_H_ */
