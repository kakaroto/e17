#ifndef ITASK_MENU_H
#define ITASK_MENU_H

EAPI void itask_menu_init(Itask *it);
EAPI void itask_menu_button(Itask *it);

EAPI E_Menu *itask_menu_items_menu(Eina_List *items);
#endif 
