/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Config      Config;
typedef struct _Config_Bar  Config_Bar;
typedef struct _Engage        Engage;
typedef struct _Engage_Bar    Engage_Bar;
typedef struct _Engage_Icon   Engage_Icon;

struct _Config
{
   char         *appdir;
   int           iconsize;
   Evas_List    *bars;
   /*
   double        handle;
   char          autohide;
   */
};

struct _Config_Bar
{
   unsigned char enabled;
   int           zoom;
   double        zoom_factor;
};

struct _Engage
{
   E_App       *apps;
   Evas_List   *bars;
   E_Menu      *config_menu;
   
   Config      *conf;
   Evas_Coord   iconbordersize;
};

struct _Engage_Bar
{
   Engage        *engage;
   E_Container *con;
   Evas        *evas;
   E_Menu      *menu;
   
   Evas_Object *bar_object;
   Evas_Object *box_object;
   Evas_Object *event_object;
   
   Evas_List   *icons;
   
   double          align, align_req;
   
   Evas_Coord      x, y, w, h;
   double          zoom;
   
   E_Gadman_Client *gmc;

   Config_Bar     *conf;
};

struct _Engage_Icon
{
   Engage_Bar      *eb;
   E_App         *app;
   Evas_Object   *bg_object;
   Evas_Object   *overlay_object;
   Evas_Object   *icon_object;
   Evas_Object   *event_object;
   Evas_List     *extra_icons;

   double         scale;
};

EAPI void *init     (E_Module *m);
EAPI int   shutdown (E_Module *m);
EAPI int   save     (E_Module *m);
EAPI int   info     (E_Module *m);
EAPI int   about    (E_Module *m);

#endif
