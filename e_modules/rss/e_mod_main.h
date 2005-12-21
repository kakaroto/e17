#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#include <time.h>

typedef struct _Config Config;
typedef struct _Rss Rss;
typedef struct _Rss_Face Rss_Face;
typedef struct _Rss_Article Rss_Article;

struct _Config 
{
   int update_rate;
   char *browser;
   char *proxy;
   int proxy_port;
};

struct _Rss 
{
   E_Container *con;
   Evas *evas;
   E_Menu *config_menu;
   
   E_Config_DD *conf_edd;
   Config *conf;
   
   Rss_Face *face;
};

struct _Rss_Face 
{
   Rss *rss;
   E_Container *con;
   Evas *evas;
   E_Gadman_Client *gmc;
   
   Evas_Object *box_obj;
   Evas_Object *item_obj;
};

struct _Rss_Article 
{
   char *title;
   char *description;
   char *url;
   time_t ts;
};

extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);
EAPI int e_modapi_info(E_Module *m);
EAPI int e_modapi_about(E_Module *m);
EAPI int e_modapi_config(E_Module *m);

EAPI void _rss_cb_config_updated(void *data);

#endif

