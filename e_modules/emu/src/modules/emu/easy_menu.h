#ifdef E_TYPEDEFS
#else
#ifndef EASY_MENU_H
#define EASY_MENU_H
#include "e.h"

struct _Menu_Item_Data
{
   E_Menu_Item *item;
   char *name;
   char *action;
   char *edje;   /* FIXME: make this similar to the e_modapi_info() shit. */
   void *data;
   struct _Menu_Item_Data *next;
};

struct _Menu_Data
{
   E_Menu *menu;
   int level;
   char *name;
   struct _Menu_Data *next;
   struct _Menu_Item_Data *items;
};

typedef struct _Easy_Menu Easy_Menu;
struct _Easy_Menu
{
   struct _Menu_Data *menu;
   E_Menu_Category_Callback *category_cb;
   char *buffer;

   char *name;
   char *category;
   E_Menu_Item *item;
   int valid; /* Probably just being paranoid.  This prevents this structure from being used when it isn't complete. */
};

EAPI Easy_Menu *easy_add_menus(char *name, char *category, char *input, int length, void (*func) (void *data, E_Menu *m, E_Menu_Item *mi), void *data);

#endif
#endif
