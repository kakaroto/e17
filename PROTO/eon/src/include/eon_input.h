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
#ifndef EON_INPUT_H_
#define EON_INPUT_H_

/*
 * TODO change the name of this
 */

#define EON_EVENT_UI_MOUSE_MOVE "mousemove"
#define EON_EVENT_UI_MOUSE_IN "mousein"
#define EON_EVENT_UI_MOUSE_OUT "mouseout"
#define EON_EVENT_UI_MOUSE_DOWN "mousedown"
#define EON_EVENT_UI_MOUSE_UP "mouseup"
#define EON_EVENT_UI_MOUSE_CLICK "click"
#define EON_EVENT_UI_KEY_DOWN "keydown"
#define EON_EVENT_UI_KEY_UP "keyup"

typedef struct _Eon_Event_Ui
{
	Ekeko_Event event;
	/* Why do we need the related? */
	Eon_Object *related;
	const Eon_Input *i;
	/* TODO for now this isnt needed but whenever the keyboard event
	 * is finished we might find duplicate things on the mouse
	 */
} Eon_Event_Ui;

typedef struct _Eon_Event_Mouse
{
	Eon_Event_Ui ui;
	/* screen */
	struct
	{
		unsigned int x;
		unsigned int y;
		unsigned int prev_x;
		unsigned int prev_y;
	} screen;
} Eon_Event_Mouse;

typedef enum _Eon_Key
{
	EON_KEYS,
} Eon_Key;

typedef enum _Eon_Key_Mod
{
	EON_KEY_MOD_NONE  = 0x0000,
	EON_KEY_MOD_LSHIFT= 0x0001,
	EON_KEY_MOD_RSHIFT= 0x0002,
	EON_KEY_MOD_LCTRL = 0x0040,
	EON_KEY_MOD_RCTRL = 0x0080,
	EON_KEY_MOD_LALT  = 0x0100,
	EON_KEY_MOD_RALT  = 0x0200,
	EON_KEY_MOD_LMETA = 0x0400,
	EON_KEY_MOD_RMETA = 0x0800,
	EON_KEY_MOD_NUM   = 0x1000,
	EON_KEY_MOD_CAPS  = 0x2000,
	EON_KEY_MOD_MODE  = 0x4000,
} Eon_Key_Mod;

#define EON_KEY_MOD_CTRL  (EON_KEY_MOD_LCTRL | EON_KEY_MOD_RCTRL)
#define EON_KEY_MOD_SHIFT (EON_KEY_MOD_LSHIFT | EON_KEY_MOD_RSHIFT)
#define EON_KEY_MOD_ALT   (EON_KEY_MOD_LALT | EON_KEY_MOD_RALT)
#define EON_KEY_MOD_META  (EON_KEY_MOD_LMETA | EON_KEY_MOD_RMETA)

typedef struct _Eon_Event_Key
{
	Eon_Event_Ui ui;
	Eon_Key_Mod mod;
	Eon_Key key;
} Eon_Event_Key;


EAPI void eon_input_feed_mouse_in(Eon_Input *i);
EAPI void eon_input_feed_mouse_move(Eon_Input *i, unsigned int x, unsigned int y);
EAPI void eon_input_feed_mouse_out(Eon_Input *i);
EAPI void eon_input_feed_mouse_down(Eon_Input *i);
EAPI void eon_input_feed_mouse_up(Eon_Input *i);
EAPI void eon_input_feed_key_up(Eon_Input *i, Eon_Key key, Eon_Key_Mod mod);
EAPI void eon_input_feed_key_down(Eon_Input *i, Eon_Key key, Eon_Key_Mod mod);

#endif /* EON_INPUT_H_ */
