/** @file etk_tooltips.h */
#ifndef _ETK_TOOLTIPS_H_
#define _ETK_TOOLTIPS_H_

#include "etk_widget.h"

/**
 * @defgroup Etk_Tooltips Etk_Tooltips
 * @{
 */

void etk_tooltips_init();
void etk_tooltips_shutdown();

void etk_tooltips_enable();
void etk_tooltips_disable();

void etk_tooltips_tip_set(Etk_Widget *widget, const char *text);
const char *etk_tooltips_tip_get(Etk_Widget *widget);
  
/** @} */

#endif
