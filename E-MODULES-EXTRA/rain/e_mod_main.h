#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Config Config;
typedef struct _Rain Rain;
typedef struct _Rain_Drop Rain_Drop;

struct _Config
{
   int cloud_count;
   int drop_count;
   int show_clouds;
};

struct _Rain
{
   E_Module *module;
   Evas_List *cons;
   Evas *canvas;
   Ecore_Animator *animator;
   Evas_List *clouds;
   Evas_List *drops;

   E_Config_DD *conf_edd;
   Config *conf;
   Evas_Coord width, height;
   E_Config_Dialog *config_dialog;
};

struct _Rain_Drop
{
   Evas_Object *drop;
   double start_time;
   int speed;
};

extern E_Module *rain_module;

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

void _rain_cb_config_updated(void *data);

#endif
