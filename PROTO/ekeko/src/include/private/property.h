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
#ifndef PROPERTY_H_
#define PROPERTY_H_

Ekeko_Property * property_new(Ekeko_Type *type, char *prop_name, Type_Property_Type prop_type,
		Ekeko_Value_Type value_type, ssize_t curr_offset, ssize_t prev_offset,
		ssize_t changed_offset);
Ekeko_Type * property_type_get(Ekeko_Property *p);
ssize_t property_curr_offset_get(Ekeko_Property *p);
ssize_t property_prev_offset_get(Ekeko_Property *p);
ssize_t property_changed_offset_get(Ekeko_Property *p);
Type_Property_Type property_ptype_get(Ekeko_Property *p);

#endif /* PROPERTY_H_ */
