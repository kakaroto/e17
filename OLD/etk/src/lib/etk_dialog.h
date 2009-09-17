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

/** @file etk_dialog.h */
#ifndef _ETK_DIALOG_H_
#define _ETK_DIALOG_H_

#include "etk_box.h"
#include "etk_types.h"
#include "etk_window.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TODO/FIXME list:
 * - We should certainly set some X hints to make dialogs not only simple windows
 * - Find a way to make modal dialogs
 */

/**
 * @defgroup Etk_Dialog Etk_Dialog
 * @brief A dialog is a window with buttons in its bottom area to allow the user to respond to a request
 * @{
 */

/** Gets the type of a dialog */
#define ETK_DIALOG_TYPE       (etk_dialog_type_get())
/** Casts the object to an Etk_Dialog */
#define ETK_DIALOG(obj)       (ETK_OBJECT_CAST((obj), ETK_DIALOG_TYPE, Etk_Dialog))
/** Checks if the object is an Etk_Dialog */
#define ETK_IS_DIALOG(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_DIALOG_TYPE))


/** @brief Some common response IDs to use with an Etk_Dialog */
typedef enum
{
   ETK_RESPONSE_NONE = -1,              /**< Response: None */
   ETK_RESPONSE_REJECT = -2,            /**< Response: Reject */
   ETK_RESPONSE_ACCEPT = -3,            /**< Response: Accept */
   ETK_RESPONSE_DELETE_EVENT = -4,      /**< Response: Delete Event */
   ETK_RESPONSE_OK = -5,                /**< Response: Ok */
   ETK_RESPONSE_CANCEL = -6,            /**< Response: Cancel */
   ETK_RESPONSE_CLOSE = -7,             /**< Response: Close */
   ETK_RESPONSE_YES = -8,               /**< Response: Yes */
   ETK_RESPONSE_NO = -9,                /**< Response: No */
   ETK_RESPONSE_APPLY = -10,            /**< Response: Apply */
   ETK_RESPONSE_HELP = -11              /**< Response: Help */
} Etk_Dialog_Response_ID;

extern int ETK_DIALOG_RESPONSE_SIGNAL;

/**
 * @brief @widget A window with buttons in its bottom area to allow the user to respond to a request
 * @structinfo
 */
struct Etk_Dialog
{
   /* private: */
   /* Inherit from Etk_Window */
   Etk_Window window;

   Etk_Widget *dialog_vbox;
   Etk_Widget *main_area_vbox;
   Etk_Widget *action_area_alignment;
   Etk_Widget *action_area_hbox;
   Etk_Widget *separator;

   Etk_Bool has_separator:1;
};


Etk_Type   *etk_dialog_type_get(void);
Etk_Widget *etk_dialog_new(void);

void        etk_dialog_pack_in_main_area(Etk_Dialog *dialog, Etk_Widget *widget, Etk_Box_Group group, Etk_Box_Fill_Policy fill_policy, int padding);
void        etk_dialog_pack_widget_in_action_area(Etk_Dialog *dialog, Etk_Widget *widget, Etk_Box_Group group, Etk_Box_Fill_Policy fill_policy, int padding);
void        etk_dialog_pack_button_in_action_area(Etk_Dialog *dialog, Etk_Button *button, int response_id, Etk_Box_Group group, Etk_Box_Fill_Policy fill_policy, int padding);

Etk_Widget *etk_dialog_button_add(Etk_Dialog *dialog, const char *label, int response_id);
Etk_Widget *etk_dialog_button_add_from_stock(Etk_Dialog *dialog, int stock_id, int response_id);

void        etk_dialog_action_area_homogeneous_set(Etk_Dialog *dialog, Etk_Bool homogeneous);
Etk_Bool    etk_dialog_action_area_homogeneous_get(Etk_Dialog *dialog);
void        etk_dialog_action_area_alignment_set(Etk_Dialog *dialog, float align);
float       etk_dialog_action_area_alignment_get(Etk_Dialog *dialog);

Etk_Widget *etk_dialog_main_area_vbox_get(Etk_Dialog *dialog);
Etk_Widget *etk_dialog_action_area_hbox_get(Etk_Dialog *dialog);
void        etk_dialog_button_response_id_set(Etk_Dialog *dialog, Etk_Button *button, int response_id);
int         etk_dialog_button_response_id_get(Etk_Button *button);

void        etk_dialog_has_separator_set(Etk_Dialog *dialog, Etk_Bool has_separator);
Etk_Bool    etk_dialog_has_separator_get(Etk_Dialog *dialog);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
