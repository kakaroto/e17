/* ENESIM - Direct Rendering Library
 * Copyright (C) 2007-2008 Jorge Luis Zapata
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
#ifndef _COMPONENT_H
#define _COMPONENT_H

/**
 * @file
 * @brief Components
 * @defgroup Component_Internal_Group Component
 * @ingroup Enesim_Internal_Group
 * @{
 */

/**
 * To be documented
 * FIXME: To be fixed
 */
struct _Enesim_Component
{
	Enesim_Container		*path; 		/**< Main temporal data */
	Enesim_Component_Reader 	*src; 		/**< Source data */
	Eina_List		*readers;
	int 			has_changed; 	/**< Forward notification */
	int 			type;
	/* component specific data */
	const char 		*name; 		/**< Component's name */
	void 			*data; 		/**< Specific component data */
	void 			(*generate)(void *data, int *num);
	void 			(*free)(void *data);
};

/**
 * To be documented
 * FIXME: To be fixed
 */
enum
{
	ENESIM_COMPONENT_O, 	/**< Output Only Component */
	ENESIM_COMPONENT_IO,	/**< Input/Output Component */
	ENESIM_COMPONENT_TYPES
};

Enesim_Component * enesim_component_new(int num);
void enesim_component_notify(Enesim_Component *c);
int enesim_component_generate(Enesim_Component *c, int *num);

/** @} */
#endif
