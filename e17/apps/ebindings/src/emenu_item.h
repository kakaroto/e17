/**************************************************************************
 * Name: emenu_item.h
 * Project: Ebindings 
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * Date: October 10, 2001
 * Description: data representation for items in an emenu 
 *************************************************************************/
#ifndef __E_MENU_ITEM_
#define __E_MENU_ITEM_

#include<Edb.h>
#include<Ewd.h>

/* 
 * enumerated type to represent the differnt types of menuitems that exist
 */
enum _e_menu_type
{
   E_MENU_SCRIPT,
   E_MENU_SUBMENU,
   E_MENU_SEPARATOR,
   E_MENU_EXECUTABLE
};

struct __emenu_item
{
   char *text, *icon, *exec;
   enum _e_menu_type type;
   Ewd_List *children;
   /* submenus */
};
typedef struct __emenu_item emenu_item;

/* create a new emenu_item */
emenu_item *emenu_item_new(void);

/* free an emenu_item */
void emenu_item_free(emenu_item *);

/* 
 * ewd requires a void function with a void pointer, this just typecasts and
 * calls the real free function
 */
void _emenu_item_free(void *);

#endif
