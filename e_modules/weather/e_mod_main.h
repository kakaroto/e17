/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#include "config.h"

#define SIMPLE_DISPLAY   0
#define DETAILED_DISPLAY 1

#define DEGREES_F 0
#define DEGREES_C 1

typedef struct _Config Config;
typedef struct _Config_Face Config_Face;
typedef struct _Weather Weather;
typedef struct _Weather_Face Weather_Face;

struct _Config {
   Evas_List          *faces;

   double              poll_time;
   int                 display;
   int                 degrees;

   char               *host;
};

struct _Config_Face {
   int                 container;

   char               *location;
   char               *url;

   unsigned char       enabled;
};

struct _Weather {
   Evas_List          *faces;
   E_Menu             *config_menu;
   E_Menu             *config_menu_poll;
   E_Menu             *config_menu_degrees;
   E_Menu             *config_menu_display;

   int                 update_menu_faces;
   E_Menu             *config_menu_faces;

   Ecore_Event_Handler *add_handler;
   Ecore_Event_Handler *del_handler;
   Ecore_Event_Handler *data_handler;

   Config             *conf;
   Ecore_Timer        *weather_check_timer;

   struct {
      char               *host;
      int                 port;
   } proxy;
};

struct _Weather_Face {
   Weather            *weather;

   E_Container        *con;
   E_Menu             *menu;
   E_Menu             *menu_location;
   Evas_List          *menu_locations;
   Config_Face        *conf;

   Ecore_Con_Server   *server;
   char               *buffer;
   int                 bufsize;
   int                 cursize;

   int                 temp;
   char                degrees;
   char                conditions[256];
   char                icon[256];

   Evas_Object        *weather_object;
   Evas_Object        *icon_object;
   Evas_Object        *event_object;

   E_Gadman_Client    *gmc;
};

EAPI extern E_Module_Api e_modapi;

EAPI void          *e_modapi_init(E_Module * m);
EAPI int            e_modapi_shutdown(E_Module * m);
EAPI int            e_modapi_save(E_Module * m);
EAPI int            e_modapi_info(E_Module * m);
EAPI int            e_modapi_about(E_Module * m);

#endif
