/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#define VALUE_FMT_STR 0
#define VALUE_FMT_NUM 1

typedef struct _Config      Config;
typedef struct _Config_Face Config_Face;
typedef struct _Moon        Moon;
typedef struct _Moon_Face   Moon_Face;

struct _Config
{
   int        show_phase_value;
   int        value_format;
   int        show_border;
   int        show_cloud_anim;
   double     poll_time;
   Eina_List *faces;

   E_Config_Dialog *config_dialog;
   E_Menu          *menu;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);

void moon_config_init(E_Module *m);
void moon_config_shutdown();
void moon_config_dialog_show(Evas_Object *o);
void moon_reset();

extern Config *moon_config;

#endif

