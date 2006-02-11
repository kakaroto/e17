/** @file etk_main.h */
#ifndef _ETK_MAIN_H_
#define _ETK_MAIN_H_

#include <Evas.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Main Main functions of Etk
 * @{
 */

Etk_Bool etk_init();
void etk_shutdown();

void etk_main();
void etk_main_quit();
void etk_main_iterate();
void etk_main_iteration_queue();

void etk_main_toplevel_widget_add(Etk_Toplevel_Widget *widget);
void etk_main_toplevel_widget_remove(Etk_Toplevel_Widget *widget);
Evas_List *etk_main_toplevel_widgets_get();

/** @} */

#endif
