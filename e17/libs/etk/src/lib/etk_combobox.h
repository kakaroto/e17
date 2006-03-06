/** @file etk_combobox.h */
#ifndef _ETK_COMBOBOX_H_
#define _ETK_COMBOBOX_H_

#include "etk_widget.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Combobox Etk_Combobox
 * @{
 */

/** @brief Gets the type of a combobox */
#define ETK_COMBOBOX_TYPE       (etk_combobox_type_get())
/** @brief Casts the object to an Etk_Combobox */
#define ETK_COMBOBOX(obj)       (ETK_OBJECT_CAST((obj), ETK_COMBOBOX_TYPE, Etk_Combobox))
/** @brief Check if the object is an Etk_Combobox */
#define ETK_IS_COMBOBOX(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_COMBOBOX_TYPE))

/**
 * @struct Etk_Combobox
 * @param A combobox is a button that pops up a window with a list of options when you click on it
*/
struct _Etk_Combobox
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;
   
   Etk_Widget *button;
   Etk_Menu_Window *window;
};

Etk_Type *etk_combobox_type_get();
Etk_Widget *etk_combobox_new();

/** @} */

#endif
