/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Config      Config;
typedef struct _Config_Bar  Config_Bar;
typedef struct _Engage        Engage;
typedef struct _Engage_Bar    Engage_Bar;
typedef struct _Engage_Tray   Engage_Tray;
typedef struct _Engage_Icon   Engage_Icon;
typedef struct _Engage_App_Icon Engage_App_Icon;

struct _Config
{
   const char   *appdir;
   Eina_List    *bars;
   unsigned char click_focus;
   /*
   double        handle;
   char          autohide;
   */
};

struct _Config_Bar
{
   unsigned char enabled;
   int           iconsize;
   int           zoom;
   double        zoom_factor, zoom_duration;
   int           zoom_stretch;
   int           tray;
};

struct _Engage
{
   E_App       *apps;
   Eina_List   *bars;
   E_Menu      *config_menu;

   Config      *conf;
   E_Module    *module;
};

struct _Engage_Tray
{
   Evas_Object *tray;
   int          w, h;
   int          icons;
   Eina_List   *wins;
   Ecore_X_Window win;
   Ecore_X_Time select_time;

   Ecore_Event_Handler *msg_handler;
   Ecore_Event_Handler *dst_handler;
};

struct _Engage_Bar
{
   Engage      *engage;
   E_Container *con;
   Evas        *evas;
   E_Menu      *menu;
   E_Menu      *zoom_size_menu;
   E_Menu      *zoom_speed_menu;
   E_Menu      *icon_menu;
   E_Menu      *context_menu;   
   
   Evas_Object *bar_object;
   Evas_Object *box_object;
   Evas_Object *event_object;
   Evas_Coord   bw, bh;
   Evas_Coord   mouse_out;
   
   Eina_List   *icons;   
   Eina_List   *contexts;
   
   double       align, align_req;
   int          loaded;
   
   Evas_Coord   x, y, w, h;
   double       zoom;
   int          cancel_zoom_in:1, cancel_zoom_out:1;
   double       zoom_start_time;
   enum {ENGAGE_NORMAL, ENGAGE_ZOOMING, ENGAGE_ZOOMED, ENGAGE_UNZOOMING} state;

   E_Gadman_Client *gmc;
   E_Config_Dialog *cfd;

   Config_Bar  *conf;
   Ecore_Event_Handler *add_handler;
   Ecore_Event_Handler *remove_handler;
   Ecore_Event_Handler *iconify_handler;
   Ecore_Event_Handler *uniconify_handler;

   Engage_Tray *tray;
   Engage_Icon *selected_ic;
};

struct _Engage_Icon
{
   Engage_Bar      *eb;
   E_App         *app;
   Evas_Object   *bg_object;
   Evas_Object   *icon_object;
   Evas_Object   *event_object;
   Eina_List     *extra_icons, *selected_app;

   double         scale;
   int            dotorder;
};

struct _Engage_App_Icon
{
   Engage_Icon   *ic;
   Evas_Object   *bg_object;
   Evas_Object   *icon_object;
   Evas_Object   *event_object;

   int            min;
   E_Border      *border;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);
EAPI int   e_modapi_about    (E_Module *m);
EAPI int   e_modapi_config   (E_Module *m);

void _engage_cb_config_updated(void *data);
#endif
