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
#ifndef EON_SETTER_H_
#define EON_SETTER_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_SETTER_VALUE_CHANGED "valueChanged"
#define EON_SETTER_PROPERTY_CHANGED "propertyChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_SETTER_VALUE;
extern Ekeko_Property_Id EON_SETTER_PROPERTY;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Setter_Private Eon_Setter_Private;
struct _Eon_Setter
{
	Ekeko_Object parent;
	Eon_Setter_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Eon_Setter * eon_setter_new(Eon_Document *d);
EAPI void eon_setter_property_set(Eon_Setter *k, const char *name);
EAPI void eon_setter_value_set(Eon_Setter *k, Ekeko_Value *val);

#endif /* EON_SETTER_H_ */
