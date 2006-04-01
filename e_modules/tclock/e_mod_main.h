#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#define RESOLUTION_MINUTE 0
#define RESOLUTION_SECOND 1

#define TIME_BUF 1024
#define FORMAT_BUF_SIZE 1024
#define TCLOCK_DEBUG(X) (printf("tclock_debug: %s\n",X))
#undef TCLOCK_DEBUG
#define TCLOCK_DEBUG(X)

typedef struct _Config {
   Evas_List          *faces;
   double              poll_time;

} Config;

typedef struct _Config_Face {
   unsigned char       enabled;
   unsigned int        resolution;
   const char         *format;
} Config_Face;

typedef struct _TClock {
   Evas_List          *faces;
   E_Menu             *config_menu;
   Config             *conf;

   Ecore_Timer        *tclock_check_timer;
} TClock;

typedef struct _TClock_Face {
   E_Container        *con;
   E_Menu             *menu;
   E_Menu             *resolution_menu;
   Config_Face        *conf;
   TClock             *tclock;

   struct {
      Evas_Coord          l, r, t, b;
   } inset;

   Evas_Object        *tclock_object;
   Evas_Object        *event_object;
   E_Gadman_Client    *gmc;
} TClock_Face;

EAPI extern E_Module_Api e_modapi;

EAPI void          *e_modapi_init(E_Module * m);
EAPI int            e_modapi_shutdown(E_Module * m);
EAPI int            e_modapi_save(E_Module * m);
EAPI int            e_modapi_info(E_Module * m);
EAPI int            e_modapi_about(E_Module * m);
EAPI int            e_modapi_config(E_Module * m);

void                _tclock_cb_config_updated(void *data);

#endif
