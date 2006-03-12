#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Config Config;
typedef struct _Config_Face Config_Face;
typedef struct _Net Net;
typedef struct _Net_Face Net_Face;

struct _Config 
{
   Evas_List *faces;
};

struct _Config_Face 
{
   unsigned char enabled;
   char *device;
   int check_interval;
   int show_text;
   int show_graph;
};

struct _Net 
{
   E_Menu *config_menu;
   Evas_List *faces;
   Config *conf;
   E_Config_Dialog *cfd;
};

struct _Net_Face 
{
   Evas *evas;
   E_Container *con;
   E_Menu *menu;
   Net *net;
   Evas_List *old_rx, *old_tx;
   Config_Face *conf;
   
   Evas_Object *net_obj;
   Evas_Object *event_obj;
   Evas_Object *chart_obj;
   Evas_Object *rtxt_obj;
   Evas_Object *ttxt_obj;
   
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

void      _net_face_graph_clear(Net_Face *nf);

#endif
