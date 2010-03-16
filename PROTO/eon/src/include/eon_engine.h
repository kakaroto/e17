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
#ifndef EON_ENGINE_H_
#define EON_ENGINE_H_

struct _Eon_Engine
{
	Eina_Bool (*flush)(void *data, Enesim_Surface *s, Eina_Rectangle *rect);
	void * (*setup)(Eon_Document *doc, int w, int h, const char *options);
	void (*cleanup)(void *data);
};

#endif /* EON_ENGINE_H_ */
