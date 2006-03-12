/** @file etk_dialog.h */
#ifndef _ETK_DIALOG_H_
#define _ETK_DIALOG_H_

#include "etk_window.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Dialog Etk_Dialog
 * @{
 */

/** @brief Gets the type of a dialog */
#define ETK_DIALOG_TYPE       (etk_dialog_type_get())
/** @brief Casts the object to an Etk_Dialog */
#define ETK_DIALOG(obj)       (ETK_OBJECT_CAST((obj), ETK_DIALOG_TYPE, Etk_Dialog))
/** @brief Checks if the object is an Etk_Dialog */
#define ETK_IS_DIALOG(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_DIALOG_TYPE))

/**
 * @enum Etk_Dialog_Response_ID
 * @brief Some common response IDs to use with an Etk_Dialog
 */
enum _Etk_Dialog_Response_ID
{
   ETK_RESPONSE_NONE = -1,
   ETK_RESPONSE_REJECT = -2,
   ETK_RESPONSE_ACCEPT = -3,
   ETK_RESPONSE_DELETE_EVENT = -4,
   ETK_RESPONSE_OK = -5,
   ETK_RESPONSE_CANCEL = -6,
   ETK_RESPONSE_CLOSE = -7,
   ETK_RESPONSE_YES = -8,
   ETK_RESPONSE_NO = -9,
   ETK_RESPONSE_APPLY = -10,
   ETK_RESPONSE_HELP = -11
};

/**
 * @enum Etk_Dialog
 * @brief A dialog is a window with buttons in its bottom area to allow the user to respond to a request. @n
 * A horizontal separator can also separate the button area from the upper area. It's enabled by default
 */
struct _Etk_Dialog
{
   /* private: */
   /* Inherit from Etk_Window */
   Etk_Window window;

   Etk_Widget *dialog_vbox;
   Etk_Widget *main_area_vbox;
   Etk_Widget *action_area_hbox;
   Etk_Widget *separator;

   Etk_Bool has_separator;
};

Etk_Type *etk_dialog_type_get();
Etk_Widget *etk_dialog_new();

void etk_dialog_pack_in_main_area(Etk_Dialog *dialog, Etk_Widget *widget, Etk_Bool expand, Etk_Bool fill, int padding, Etk_Bool pack_at_end);
void etk_dialog_pack_in_action_area(Etk_Dialog *dialog, Etk_Widget *widget, Etk_Bool expand, Etk_Bool fill, int padding, Etk_Bool pack_at_end);

void etk_dialog_pack_button_in_action_area(Etk_Dialog *dialog, Etk_Button *button, int response_id, Etk_Bool expand, Etk_Bool fill, int padding, Etk_Bool pack_at_end);
Etk_Widget *etk_dialog_button_add(Etk_Dialog *dialog, const char *label, int response_id);
Etk_Widget *etk_dialog_button_add_from_stock(Etk_Dialog *dialog, int stock_id, int response_id);

void etk_dialog_has_separator_set(Etk_Dialog *dialog, Etk_Bool has_separator);
Etk_Bool etk_dialog_has_separator_get(Etk_Dialog *dialog);

/** @} */

#endif
