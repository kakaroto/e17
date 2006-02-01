#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Config Config;
typedef struct _Config_Face Config_Face;
typedef struct _Screen Screen;
typedef struct _Screen_Face Screen_Face;

struct _Config
{
   int delay_time;
   unsigned char use_import;
   unsigned char use_scrot;
   char *location;
   char *filename;
   struct
   {
      unsigned char use_img_border;
      unsigned char use_dither;
      unsigned char use_frame;
      unsigned char use_mono;
      unsigned char use_window;
      unsigned char use_silent;
      unsigned char use_trim;
   } import;
   struct
   {
      unsigned char use_img_border;
      unsigned char use_count;
      unsigned char use_window;
      unsigned char use_thumb;
   } scrot;
};

struct _Config_Face
{
   unsigned char enabled;
};

struct _Screen
{
   E_Menu *config_menu;
   Screen_Face *face;
   Config *conf;

   E_Config_DD *conf_edd;
   E_Config_Dialog *config_dialog;
};

struct _Screen_Face
{
   Evas *evas;
   E_Container *con;
   E_Menu *menu;
   Screen *screen;

   Config_Face *conf;
   E_Config_DD *conf_face_edd;

   Evas_Object *screen_object;
   Evas_Object *event_object;

   E_Gadman_Client *gmc;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);
EAPI int e_modapi_info(E_Module *m);
EAPI int e_modapi_about(E_Module *m);
EAPI int e_modapi_config(E_Module *m);

#endif
