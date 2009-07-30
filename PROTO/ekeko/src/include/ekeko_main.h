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
#ifndef EKEKO_MAIN_H_
#define EKEKO_MAIN_H_

#define EKEKO_CAST_CHECK

#ifdef EKEKO_CAST_CHECK
#define EKEKO_CAST(o, e_type, c_type) ((c_type *)(ekeko_object_cast((Ekeko_Object *)o, (e_type))))
#else
#define EKEKO_CAST(o, e_type, c_type) ((c_type *)(o))
#endif

EAPI int ekeko_init(void);
EAPI int ekeko_shutdown(void);

#endif /* EKEKO_MAIN_H_ */
