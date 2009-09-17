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

/** @file etk_statusbar.h */
#ifndef _ETK_STATUSBAR_H_
#define _ETK_STATUSBAR_H_

#include <Evas.h>

#include "etk_types.h"
#include "etk_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Statusbar Etk_Statusbar
 * @brief A statusbar is a horizontal bar that can display various messages, usually about the status of the program
 * @{
 */

/** Gets the type of a status bar */
#define ETK_STATUSBAR_TYPE       (etk_statusbar_type_get())
/** Casts the object to an Etk_Statusbar */
#define ETK_STATUSBAR(obj)       (ETK_OBJECT_CAST((obj), ETK_STATUSBAR_TYPE, Etk_Statusbar))
/** Checks if the object is an Etk_Statusbar */
#define ETK_IS_STATUSBAR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_STATUSBAR_TYPE))


/**
 * @brief @widget A horizontal bar that can display various messages
 * @structinfo
 */
struct Etk_Statusbar
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Eina_List *msg_stack;
   int next_message_id;
   int next_context_id;
   int window_width;
   int window_height;
   Etk_Bool has_resize_grip:1;
};


Etk_Type   *etk_statusbar_type_get(void);
Etk_Widget *etk_statusbar_new(void);

int         etk_statusbar_context_id_get(Etk_Statusbar *statusbar, const char *context);
int         etk_statusbar_message_push(Etk_Statusbar *statusbar, const char *message, int context_id);
void        etk_statusbar_message_pop(Etk_Statusbar *statusbar, int context_id);
void        etk_statusbar_message_remove(Etk_Statusbar *statusbar, int message_id);
void        etk_statusbar_message_get(Etk_Statusbar *statusbar, const char **message, int *message_id, int *context_id);

void        etk_statusbar_has_resize_grip_set(Etk_Statusbar *statusbar, Etk_Bool has_resize_grip);
Etk_Bool    etk_statusbar_has_resize_grip_get(Etk_Statusbar *statusbar);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
