/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
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

/** @file etk_dnd.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_dnd.h"

#include <stdlib.h>
#include <string.h>

#include <Ecore.h>

#include "etk_engine.h"

/* Some notes for when we're developing this:
 * Signals:
 * - source:
 * drag_begin: drag has started
 * drag_end: drag has ended
 *
 * - destination:
 * drag_leave: mouse leaves widget
 * drag_motion: mouse is moving on widget
 * drag_drop: object dropped on widget
 *
 * selection_received: when our widget gets the selection it has asked for
 *                     (ie, we want to get a selection)
 * selection_get: when a request for a selection has been made to our widget
 *                (ie, someone is getting our selection from us)
 * selection_clear_event: the selection has been cleared
 *
 * clipboard_received: when our widget gets the clipboard data it has asked for
 *                     (ie, we want to get a clipboard's text / image)
 *
 */

/**************************
 *
 * Implementation
 *
 **************************/

/** @} */
