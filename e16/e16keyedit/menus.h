#ifndef E_MENUS_H
#define E_MENUS_H 1

typedef struct _e_menu      E_Menu;
typedef struct _e_menu_item E_Menu_Item;
typedef struct _e_menu_list E_Menu_List;

struct _e_menu_item
{
   int           state;
   char         *icon;
   char         *text;
   char         *submenu;
   void        (*func)      (void *data);
   void        (*func_free) (void *data);
   void         *data;   
   E_Menu_Item  *next;
};

struct _e_menu
{
   char        *name;
   Window       win;
   Pixmap       pmap, mask;
   int          x, y, w, h;
   int          visible;
   double       map_time;
   E_Menu_Item *items;
   E_Menu      *next;
   int          needs_redraw;
};

struct _e_menu_list
{
   E_Menu      *menu;
   E_Menu_List *next;
};

#define MENU_ITEM_STATE_NORMAL   0x00
#define MENU_ITEM_STATE_SELECTED 0x01
#define MENU_ITEM_STATE_OFF      0x02
#define MENU_ITEM_STATE_ON       0x04

#define MENU_ITEM_IS_BOOLEAN(item) \
((item)->state & (MENU_ITEM_STATE_OFF | MENU_ITEM_STATE_ON))
#define MENU_ITEM_SET_NORMAL(item) \
(item)->state = (item)->state & (~MENU_ITEM_STATE_SELECTED)
#define MENU_ITEM_SET_SELECTED(item) \
(item)->state = (item)->state | MENU_ITEM_STATE_SELECTED
#define MENU_ITEM_SET_OFF(item) \
(item)->state = ((item)->state | MENU_ITEM_STATE_OFF) & (~MENU_ITEM_STATE_ON)
#define MENU_ITEM_SET_ON(item) \
(item)->state = ((item)->state | MENU_ITEM_STATE_ON) & (~MENU_ITEM_STATE_OFF)

E_Menu *e_menu_new(void);
void    e_menu_handle_event(E_Menu *m, Eevent * ev);
void    e_menu_show_at(E_Menu *m, int x, int y);
void    e_menu_show_at_mouse_xy(E_Menu *m, int x, int y);
void    e_menu_show_at_submen(E_Menu *m, E_Menu *parent_m, int entry);
void    e_menu_hide(E_Menu *m);
void    e_menu_free(E_Menu *m);
E_Menu *e_menu_find(char *name);
void    e_menu_add_entry(E_Menu *m, char *text, char *icon, char *submenu, int onoff, void (*func) (void *data), void *data, void (*func_free) (void *data));
void    e_menu_del_entry(E_Menu *m, E_Menu_Item *mi);
void    e_menu_redraw(E_Menu *m);

#endif
