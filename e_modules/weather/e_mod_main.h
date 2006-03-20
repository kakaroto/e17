#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#include "config.h"

#define SIMPLE_DISPLAY 0
#define DETAILED_DISPLAY 1
#define DEGREES_F 0
#define DEGREES_C 1

typedef struct _Config Config;
typedef struct _Config_Face Config_Face;
typedef struct _Weather Weather;
typedef struct _Weather_Face Weather_Face;

struct _Config 
{
   Evas_List *faces;
};

struct _Config_Face 
{
   double poll_time;
   int display, degrees;
   const char *host;
   const char *location;
   const char *code;
   unsigned char enabled;   
};

struct _Weather 
{
   Evas_List *faces;
   E_Menu *config_menu;
   Config *conf;
   E_Config_Dialog *cfd;
   Ecore_Event_Handler *add_handler;
   Ecore_Event_Handler *del_handler;
   Ecore_Event_Handler *data_handler;   
   struct {
      char *host;
      int port;
   } proxy;
};

struct _Weather_Face 
{
   Evas *evas;
   E_Container *con;
   E_Menu *menu;
   Weather *weather;
   Config_Face *conf;
   Ecore_Timer *check_timer;
   
   Evas_Object *weather_obj;
   Evas_Object *icon_obj;
   Evas_Object *event_obj;
   
   Ecore_Con_Server *server;
   E_Gadman_Client *gmc;
   
   int temp;
   char degrees;
   char conditions[256];
   char icon[256];
   char *buffer;
   char location[256];
   int bufsize;
   int cursize;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);
EAPI int   e_modapi_info     (E_Module *m);
EAPI int   e_modapi_about    (E_Module *m);
EAPI int   e_modapi_config   (E_Module *m);

void _weather_display_set(Weather_Face * wf, int ok);

#endif
