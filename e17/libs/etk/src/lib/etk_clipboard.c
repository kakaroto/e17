/** @file etk_clipboard.c */
#include "etk_clipboard.h"

#include "config.h"
#include "etk_window.h"
#include "etk_engine.h"

extern Etk_Widget *_etk_selection_widget;

/**
 * @brief Requests text from clipboard
 * @param widget a widget
 * @param callback the callback to call when we get the text
 * @param data a pointer to data we would like the callback to have
 */
void etk_clipboard_text_request(Etk_Widget *widget)
{
   if (!widget || !ETK_IS_WINDOW(widget->toplevel_parent))
     return;
   
   etk_engine_clipboard_text_request(widget);
}

/**
 * @brief Sets the text of the clipboard
 * @param widget a widget
 * @param text the text to set
 * @param length the length of the text
 */
void etk_clipboard_text_set(Etk_Widget *widget, const char *text, int length)
{
   if (!widget || !ETK_IS_WINDOW(widget->toplevel_parent) || !text || length < 1)
      return;
   
   etk_engine_clipboard_text_set(widget, text, length);
}
