#ifndef E_MENU_BUILD_H
#define E_MENU_BUILD_H

#include "e.h"
#include "object.h"

typedef struct _E_Build_Menu          E_Build_Menu;

struct _E_Build_Menu
{
   E_Object   o;

   char      *file;
   time_t     mod_time;

  /* kjb adds */
  int x,y;
  int visible;
  int edit_target;
   
   E_Menu    *menu;
   
   Evas_List *  menus;
   Evas_List *  commands;
};

void          e_build_menu_cleanup(E_Build_Menu *bm);
E_Build_Menu *e_build_menu_new_from_db(char *file);
E_Build_Menu *e_build_menu_new_from_gnome_apps(char *dir);

#endif
