#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Config Config;
typedef struct _Config_Face Config_Face;
typedef struct _Mem Mem;
typedef struct _Mem_Face Mem_Face;

struct _Config 
{
   int check_interval;
   int show_text;
   int show_graph;
   int show_percent;
   unsigned char real_ignore_buffers;
   unsigned char real_ignore_cached;
};

struct _Config_Face 
{
   unsigned char enabled;
};

struct _Mem 
{
   E_Menu *config_menu;
   Mem_Face *face;
   Config *conf;
   
   E_Config_DD *conf_edd;
   E_Config_Dialog *cfd;
};

struct _Mem_Face 
{
   Evas *evas;
   E_Container *con;
   E_Menu *menu;
   Mem *mem;
   Evas_List *old_real;
   
   Config_Face *conf;
   E_Config_DD *conf_face_edd;
   
   Evas_Object *mem_obj;
   Evas_Object *event_obj;
   Evas_Object *chart_obj;
   Evas_Object *rtxt_obj;
   Evas_Object *stxt_obj;
   
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
