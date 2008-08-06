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

/** @file etk_selection.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_selection.h"

#include <stdlib.h>
#include <string.h>

#include "etk_engine.h"

/**
 * @addtogroup Etk_Selection
 * @{
 */

/**
 * @brief Sets the text of the given selection
 * @param selection the selection type whose text should be set
 * @param text the text to set
 */
void etk_selection_text_set(Etk_Selection_Type selection, const char *text)
{
   etk_engine_selection_text_set(selection, text);
}

/**
 * @brief Requests the text from a selection
 * @param selection the selection you want to request the text from
 * @param target the widget that will receive the selection. When Etk receives the requested text from the selection,
 * it will emit the signal "selection-received" to the widget @a target
 */
void etk_selection_text_request(Etk_Selection_Type selection, Etk_Widget *target)
{
   etk_engine_selection_text_request(selection, target);
}

/**
 * @brief Clears the given selection
 * @param selection the selection to clear
 */
void etk_selection_clear(Etk_Selection_Type selection)
{
   etk_engine_selection_clear(selection);
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Selection
 *
 * The selection system is used to add copy/paste support to Etk widgets. @n
 * There are mainly two types of selections:
 * - The primary selection that is used to copy text when some text is selected, and to paste it when the middle-button
 * of the mouse is clicked
 * - The clipboard selection that is the traditional copy/paste, used when the user presses CTRL+X/C/V
 *
 * There is a third selection type called "secondary selection", but it is almost never used. @n
 *
 * You will almost never have to use directly the etk_selection_*() functions, they are already used in existing widgets.
 * However, you may have to use them if you are creating a new widget to add copy/paste support to it. @n
 * The following example shows how to add clipboard support to a widget:
 * @code
 * //Called when the content of the clipboard is received (after we called etk_selection_text_request())
 * static void _selection_received_cb(Etk_Widget *your_widget, Etk_Selection_Event *event, void *data)
 * {
 *    if (event->type == ETK_SELECTION_TEXT)
 *    {
 *       printf("Pasted text: %s\n", event->data->text);
 *       //Here, you can do whatever you want with the pasted text
 *    }
 * }
 *
 * //Connect the widget to the signal "selection-received" to be notified
 * //when the content of the clipboard is received, after a paste (you only need to call this line once)
 * etk_signal_connect("selection-received", ETK_OBJECT(your_widget), ETK_CALLBACK(_selection_received_cb), NULL);
 *
 * //To paste the content of the clipboard into your widget (on CTRL+V for example)
 * //It will emit the signal "selection-received" when the content of the clipboard is received
 * etk_selection_text_request(ETK_SELECTION_CLIPBOARD, your_widget);
 *
 * //To set the text of the clipboard (on CTRL+C for example)
 * etk_selection_text_set(ETK_SELECTION_CLIPBOARD, "the text to set");
 * @endcode
 */

