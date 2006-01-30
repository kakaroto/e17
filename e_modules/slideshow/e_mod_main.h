#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Config Config;
typedef struct _Config_Face Config_Face;
typedef struct _Slide Slide;
typedef struct _Slide_Face Slide_Face;

struct _Config {
   int                 disable_timer;
   double              cycle_time;
#ifdef WANT_OSIRIS
   char               *theme;
#endif
};

struct _Config_Face {
   unsigned char       enabled;
};

struct _Slide {
   E_Menu             *config_menu;

   char               *display;
   Slide_Face         *face;
   Ecore_Timer        *cycle_timer;

   Config             *conf;
   E_Config_DD        *conf_edd;
   E_Config_Dialog    *config_dialog;
};

struct _Slide_Face {
   Evas               *evas;
   E_Container        *con;
   Slide              *slide;

   E_Menu             *menu;
   Config_Face        *conf;
   E_Config_DD        *conf_face_edd;

   Evas_Object        *slide_object;
   Evas_Object        *event_object;

   E_Gadman_Client    *gmc;
};

EAPI extern E_Module_Api e_modapi;

EAPI void          *e_modapi_init(E_Module * m);
EAPI int            e_modapi_shutdown(E_Module * m);
EAPI int            e_modapi_save(E_Module * m);
EAPI int            e_modapi_info(E_Module * m);
EAPI int            e_modapi_about(E_Module * m);
EAPI int            e_modapi_config(E_Module * m);

void                _slide_cb_config_updated(void *data);

#endif
