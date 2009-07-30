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
#ifndef VALUE_H_
#define VALUE_H_

void ekeko_value_init(void);
void ekeko_value_shutdown(void);
void ekeko_value_create(Ekeko_Value *value, Ekeko_Value_Type type);
void ekeko_value_pointer_double_to(Ekeko_Value *value, Ekeko_Value_Type type, void *ptr,
		void *prev, char *changed);
void ekeko_value_pointer_to(Ekeko_Value *v, Ekeko_Value_Type vtype, void *ptr);

#endif /* VALUE_H_ */
