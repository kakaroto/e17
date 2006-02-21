#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Config Config;
typedef struct _Config_Face Config_Face;
typedef struct _Wlan Wlan;
typedef struct _Wlan_Face Wlan_Face;
typedef enum   _Display_Mode 
{
   NET_DISPLAY_BYTES,
     NET_DISPLAY_KBYTES,
     NET_DISPLAY_MBYTES
} Display_Mode;

struct _Config 
{
   Evas_List *faces;
};

struct _Config_Face 
{
   unsigned char enabled;
   char *device;
   int check_interval;
   int display_mode;
};

struct _Wlan 
{
   E_Menu *config_menu;
   Evas_List *faces;
   Config *conf;
   E_Config_Dialog *cfd;
};

struct _Wlan_Face 
{
   Evas *evas;
   E_Container *con;
   E_Menu *menu;
   Wlan *wlan;

   Config_Face *conf;
   
   Evas_Object *wlan_obj;
   Evas_Object *event_obj;
   Evas_Object *chart_obj;
   
   Ecore_Timer *monitor;
   
   E_Gadman_Client *gmc;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);
EAPI int   e_modapi_info     (E_Module *m);
EAPI int   e_modapi_about    (E_Module *m);
EAPI int   e_modapi_config   (E_Module *m);

#endif
