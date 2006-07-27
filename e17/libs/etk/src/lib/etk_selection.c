/** @file etk_selection.c */
#include "etk_selection.h"
#include <stdlib.h>
#include <string.h>

#include "config.h"

#include "etk_window.h"
#include "etk_engine.h"

Etk_Widget *_etk_selection_widget = NULL;

/**
 * @brief Requests text from selection
 * @param widget a widget
 */
void etk_selection_text_request(Etk_Widget *widget)
{
   etk_engine_selection_text_request(widget);
}

/**
 * @brief Sets the text of the selection
 * @param widget a widget
 * @param text the text to set
 * @param length the length of the text
 */
void etk_selection_text_set(Etk_Widget *widget, const char *text, int length)
{
   etk_engine_selection_text_set(widget, text, length);
}

/**
 * @brief Clears the selection
 */
void etk_selection_clear()
{
   etk_engine_selection_clear();
}
