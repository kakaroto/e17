/** @file etk_text_view.h */
#ifndef _ETK_TEXT_VIEW_H_
#define _ETK_TEXT_VIEW_H_

#include "etk_widget.h"
#include <Evas.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Text_View Etk_Text_View
 * @{
 */

/** @brief Gets the type of a text view */
#define ETK_TEXT_VIEW_TYPE       (etk_text_view_type_get())
/** @brief Casts the object to an Etk_Text_View */
#define ETK_TEXT_VIEW(obj)       (ETK_OBJECT_CAST((obj), ETK_TEXT_VIEW_TYPE, Etk_Text_View))
/** @brief Checks if the object is an Etk_Text_View */
#define ETK_IS_TEXT_VIEW(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TEXT_VIEW_TYPE))

/**
 * @struct Etk_Text_View
 * @brief An Etk_Text_View is a widget that allows the user to edit multi-line text 
 */
struct _Etk_Text_View
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Etk_Text_Buffer *text_buffer;
   Evas_Object *textblock_object;
};

Etk_Type *etk_text_view_type_get();
Etk_Widget *etk_text_view_new();

/** @} */

#endif
