/** @file etk_window.h */
#ifndef _ETK_WINDOW_H_
#define _ETK_WINDOW_H_

#include <Ecore.h>
#include <Ecore_Evas.h>
#include "etk_toplevel_widget.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Window Etk_Window
 * @{
 */

/** @brief Gets the type of a window */
#define ETK_WINDOW_TYPE       (etk_window_type_get())
/** @brief Casts the object to an Etk_Window */
#define ETK_WINDOW(obj)       (ETK_OBJECT_CAST((obj), ETK_WINDOW_TYPE, Etk_Window))
/** @brief Check if the object is an Etk_Window */
#define ETK_IS_WINDOW(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_WINDOW_TYPE))

struct _Etk_Window
{
   /* private: */
   /* Inherit from Etk_Toplevel_Widget */
   Etk_Toplevel_Widget toplevel_widget;

   Ecore_Evas *ecore_evas;

   Etk_Bool (*delete_event)(Etk_Window *window);
};

Etk_Type *etk_window_type_get();
Etk_Widget *etk_window_new();

void etk_window_title_set(Etk_Window *window, const char *title);
void etk_window_wmclass_set(Etk_Window *window, const char *window_name, const char *window_class);

/** @} */

#endif
