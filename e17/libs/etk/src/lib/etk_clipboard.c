/** @file etk_clipboard.c */
#include "etk_clipboard.h"
#include <stdlib.h>
#include <string.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include "etk_window.h"
#include "config.h"

extern Etk_Widget *_etk_selection_widget;

/**
 * @brief Requests text from clipboard
 * @param widget a widget
 * @param callback the callback to call when we get the text
 * @param data a pointer to data we would like the callback to have
 */
void etk_clipboard_text_request(Etk_Widget *widget)
{
#if HAVE_ECORE_X
   Ecore_X_Window win;
   
   if (!widget || !ETK_IS_WINDOW(widget->toplevel_parent))
     return;
   
   win = ETK_WINDOW(widget->toplevel_parent)->x_window;
   _etk_selection_widget = widget;
   ecore_x_selection_clipboard_request(win, ECORE_X_SELECTION_TARGET_UTF8_STRING);
#endif
}

/**
 * @brief Sets the text of the clipboard
 * @param widget a widget
 * @param text the text to set
 * @param length the length of the text
 */
void etk_clipboard_text_set(Etk_Widget *widget, const char *text, int length)
{
#if HAVE_ECORE_X
   Ecore_X_Window win;
   
   if (!widget || !ETK_IS_WINDOW(widget->toplevel_parent) || !text || length < 1)
      return;
   
   win = ETK_WINDOW(widget->toplevel_parent)->x_window;
   ecore_x_selection_clipboard_set(win, (char *)text, length);
#endif
}
