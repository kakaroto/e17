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

#ifndef EVENT_H_
#define EVENT_H_

void event_mutation_init(Ekeko_Event_Mutation *em, const char *type, const Ekeko_Object *o,
		const Ekeko_Object *rel, const Ekeko_Property *prop, Ekeko_Value *prev, Ekeko_Value *curr,
		Ekeko_Event_Mutation_State state);
void event_ui_init(Ekeko_Event_Ui *eui, const char *type, const Ekeko_Object *o,
		const Ekeko_Object *related, const Ekeko_Input *i);
void event_mouse_move_init(Ekeko_Event_Mouse *em, const Ekeko_Object *o, const Ekeko_Object *related,
		const Ekeko_Input *i, unsigned int sx, unsigned int sy);
void event_mouse_in_init(Ekeko_Event_Mouse *em, const Ekeko_Object *o, const Ekeko_Object *related,
		const Ekeko_Input *i);
void event_mouse_out_init(Ekeko_Event_Mouse *em, const Ekeko_Object *o, const Ekeko_Object *related,
		const Ekeko_Input *i);
void event_mouse_down_init(Ekeko_Event_Mouse *em, const Ekeko_Object *o, const Ekeko_Object *related,
		const Ekeko_Input *i);
void event_mouse_up_init(Ekeko_Event_Mouse *em, const Ekeko_Object *o, const Ekeko_Object *related,
		const Ekeko_Input *i);
void event_mouse_click_init(Ekeko_Event_Mouse *em, const Ekeko_Object *o, const Ekeko_Object *related,
		const Ekeko_Input *i);

#endif /* EVENT_H_ */
