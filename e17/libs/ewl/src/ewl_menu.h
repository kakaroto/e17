#ifndef __EWL_MENU_H__
#define __EWL_MENU_H__

#define Ewl_Menu Ewl_Menu_Base
#define EWL_MENU(menu) ((Ewl_Menu *) menu)

Ewl_Widget     *ewl_menu_new(char *image, char *title);
void            ewl_menu_init(Ewl_Menu * menu, char *image, char *title);

void            ewl_menu_set_title_expandable(Ewl_Menu * m);

#endif				/* __EWL_MENU_H__ */
