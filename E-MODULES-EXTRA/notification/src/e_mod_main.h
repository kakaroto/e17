#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#define HAVE_EDBUS 1
#include <E_Notification_Daemon.h>
#include <e.h>
#include <Evas.h>

#include "e_mod_macro.h"
#include "e_mod_types.h"
#include "e_mod_config_item.h"
#include "e_mod_box.h"
#include "e_mod_popup.h"
#include "config.h"

enum _Popup_Direction
{
  DIRECTION_UP,
  DIRECTION_DOWN,
  DIRECTION_LEFT,
  DIRECTION_RIGHT
};

struct _Config 
{
   E_Config_Dialog *cfd;

   int version;
   int show_low;
   int show_normal;
   int show_critical;
   Popup_Direction direction;
   int gap;
   struct 
     {
        int x;
        int y;
     } placement;

   E_Notification_Daemon *daemon;

   Evas_List  *instances;
   Evas_List  *n_box;
   Evas_List  *config_dialog;
   E_Menu     *menu;
   Evas_List  *handlers;
   Evas_List  *items;
   Evas_List  *popups;
   float       default_timeout;
   int         next_id;
};

struct _Config_Item 
{
   const char *id;
   int show_label;
   int show_popup;
   int focus_window;
   int store_low;
   int store_normal;
   int store_critical;
};

struct _Instance 
{
   E_Gadcon_Client  *gcc;
   Notification_Box *n_box;
   Config_Item      *ci;
};

EAPI extern E_Module_Api e_modapi;

EAPI void  *e_modapi_init(E_Module *m);
EAPI int    e_modapi_shutdown(E_Module *m);
EAPI int    e_modapi_save(E_Module *m);

void             _gc_orient    (E_Gadcon_Client *gcc);

EAPI E_Config_Dialog *e_int_config_notification_module(E_Container *con, 
                                                       const char *params __UNUSED__);

extern E_Module *notification_mod;
extern Config   *notification_cfg;
extern const E_Gadcon_Client_Class _gc_class;
extern int uuid;

#endif
