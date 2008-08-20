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
#ifndef _READER_H
#define _READER_H

/**
 *
 */
struct _Enesim_Component_Reader
{
	Enesim_Point 		*points;
	char 			*cmds;
	
	int 			pos;

	Enesim_Component 	*from; 	/**< Component the reader reads from */
	Enesim_Component 	*to; 	/**< In case the reader is used as a pipe */
	
};

void enesim_reader_notify(Enesim_Component_Reader *r);
void enesim_reader_reference_update(Enesim_Component_Reader *r);

#endif
