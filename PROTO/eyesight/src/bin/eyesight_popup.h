/*
 * Eyesight - EFL-based document renderer
 * Copyright (C) 2010-2011 Vincent Torri <vtorri at univ-evry dot fr>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef EYESIGHT_POPUP_H_
#define EYESIGHT_POPUP_H_

typedef struct _Popup Popup;

Popup        *popup_new(Evas *evas);
void          popup_free(Popup *p);
unsigned char popup_background_set(Popup *p, const char *filename);
void          popup_text_set(Popup *p, const char *text);
void          popup_timer_start(Popup *p);

#endif /* EYESIGHT_POPUP_H_ */
