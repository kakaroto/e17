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
#ifndef EKEKO_INPUT_H_
#define EKEKO_INPUT_H_

EAPI void ekeko_input_feed_mouse_in(Ekeko_Input *i);
EAPI void ekeko_input_feed_mouse_move(Ekeko_Input *i, unsigned int x, unsigned int y);
EAPI void ekeko_input_feed_mouse_out(Ekeko_Input *i);
EAPI void ekeko_input_feed_mouse_down(Ekeko_Input *i);
EAPI void ekeko_input_feed_mouse_up(Ekeko_Input *i);

#endif /* ETK2_INPUT_H_ */
