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
#ifndef RENDERABLE_H_
#define RENDERABLE_H_

Eina_Bool renderable_appended_get(Ekeko_Renderable *r);
void renderable_appended_set(Ekeko_Renderable *r, Eina_Bool appended);
int ekeko_renderable_zindex_get(Ekeko_Renderable *r);
void ekeko_renderable_zindex_set(Ekeko_Renderable *r, int zindex);
Eina_Bool ekeko_renderable_intersect(Ekeko_Renderable *r, int x, int y);

#endif /* RENDERABLE_H_ */
