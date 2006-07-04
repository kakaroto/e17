/** @file etk_selection.c */
#include "etk_selection.h"
#include <stdlib.h>
#include <string.h>
#include <Ecore.h>

#include "config.h"

#if HAVE_ECORE_X
#include <Ecore_X.h>
#endif

#include "etk_window.h"

Etk_Widget *_etk_selection_widget = NULL;

/**
 * @brief Requests text from selection
 * @param widget a widget
 */
void etk_selection_text_request(Etk_Widget *widget)
{
#if HAVE_ECORE_X
   Ecore_X_Window win;
   
   if (!widget || !ETK_IS_WINDOW(widget->toplevel_parent))
     return;
   
   win = ETK_WINDOW(widget->toplevel_parent)->x_window;
   _etk_selection_widget = widget;
   ecore_x_selection_primary_request(win, ECORE_X_SELECTION_TARGET_UTF8_STRING);
#endif
}

/**
 * @brief Sets the text of the selection
 * @param widget a widget
 * @param text the text to set
 * @param length the length of the text
 */
void etk_selection_text_set(Etk_Widget *widget, const char *text, int length)
{
#if HAVE_ECORE_X
   Ecore_X_Window win;
   
   if (!widget || !ETK_IS_WINDOW(widget->toplevel_parent) || !text || length < 1)
      return;
   
   win = ETK_WINDOW(widget->toplevel_parent)->x_window;
   ecore_x_selection_primary_set(win, (char *)text, length);
#endif
}

/**
 * @brief Clears the selection
 */
void etk_selection_clear()
{
#if HAVE_ECORE_X
   ecore_x_selection_primary_clear();
#endif
}
