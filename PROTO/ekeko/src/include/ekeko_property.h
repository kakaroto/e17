/* EKEKO - Object and property system
 * Copyright (C) 2007-2009 Jorge Luis Zapata
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
#ifndef EKEKO_PROPERTY_H_
#define EKEKO_PROPERTY_H_

typedef struct _Ekeko_Property Ekeko_Property;
typedef uint32_t Ekeko_Property_Id;

EAPI Ekeko_Property_Id ekeko_property_id_get(const Ekeko_Property *p);
EAPI const char * ekeko_property_name_get(const Ekeko_Property *p);
Ekeko_Value_Type ekeko_property_value_type_get(Ekeko_Property *p);

#endif /* EKEKO_PROPERTY_H_ */
