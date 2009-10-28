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
#ifndef EON_BUFFER_H_
#define EON_BUFFER_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_BUFFER_FORMAT_CHANGED "formatChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_BUFFER_FORMAT;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Buffer_Private Eon_Buffer_Private;
struct _Eon_Buffer
{
	Eon_Paint_Square parent;
	Eon_Buffer_Private *prv;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Eon_Buffer * eon_buffer_new(Eon_Document *d);

#endif /* EON_BUFFER_H_ */
