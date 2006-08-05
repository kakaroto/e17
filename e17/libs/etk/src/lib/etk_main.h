/** @file etk_main.h */
#ifndef _ETK_MAIN_H_
#define _ETK_MAIN_H_

#include <Evas.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Main The main functions of Etk
 * @brief The main functions of Etk, used to initialize or shutdown Etk, or to control the main loop
 * @{
 */

int etk_init(int *argc, char ***argv);
int etk_shutdown();

void etk_main();
void etk_main_quit();
void etk_main_iterate();
void etk_main_iteration_queue();

void etk_main_toplevel_widget_add(Etk_Toplevel_Widget *widget);
void etk_main_toplevel_widget_remove(Etk_Toplevel_Widget *widget);
Evas_List *etk_main_toplevel_widgets_get();

/** @} */

#endif
