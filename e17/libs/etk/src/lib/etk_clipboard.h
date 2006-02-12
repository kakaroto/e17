/** @file etk_clipboard.h */
#ifndef _ETK_CLIPBOARD_H_
#define _ETK_CLIPBOARD_H_

#include "etk_types.h"

void etk_clipboard_text_request(Etk_Widget *widget);
void etk_clipboard_text_set(Etk_Widget *widget, const char *data, int length);
  
/** @} */

#endif
