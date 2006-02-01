#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#define FSTAB "/etc/fstab"
#define PROCMOUNTS "/proc/mounts"
#define MTAB "/etc/mtab"

#define MOUNT_ORIENT_HORIZ 1
#define MOUNT_ORIENT_VERT 0

typedef struct _Config Config;
typedef struct _Config_Box Config_Box;
typedef struct _Mount Mount;
typedef struct _Mount_Box Mount_Box;
typedef struct _Mount_Icon Mount_Icon;
typedef struct _Mount_Point Mount_Point;

typedef enum eject_mode
{
   NO_EJECT,
   MANUAL_EJECT,
   AUTO_EJECT
} Eject_Mode;

struct _Config
{
   int icon_size;
   int orientation;
   unsigned char show_labels;
};

struct _Config_Box
{
   unsigned char enabled;
};

struct _Mount
{
   E_Container *con;
   Mount_Box *box;
   E_Menu *config_menu;
   E_Menu *config_menu_size;
   E_Menu *config_menu_options;

   Config *conf;
   E_Config_DD *conf_edd;
   Evas_List *mntpoints;
   E_Config_Dialog *config_dialog;
};

struct _Mount_Box
{
   Mount *mnt;
   E_Container *con;
   Evas *evas;
   E_Menu *menu;
   E_Gadman_Client *gmc;

   Config_Box *conf;
   E_Config_DD *conf_box_edd;

   Evas_Object *bar_object;
   Evas_Object *box_object;
   Evas_Object *event_object;

   Evas_List *icons;

   Evas_Coord x, y, w, h;
   double align, align_req;
   struct
   {
      Evas_Coord l, r, t, b;
   } icon_inset;
};

struct _Mount_Icon
{
   Mount_Box *mntbox;
   Mount_Point *mntpoint;

   Evas_Object *bg_object;
   Evas_Object *icon_object;
   Evas_Object *overlay_object;
   Evas_Object *txt_object;
};

struct _Mount_Point
{
   char *device;
   char *path;
   char *part;

   int mounted;
   Eject_Mode eject_mode;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);
EAPI int e_modapi_info(E_Module *m);
EAPI int e_modapi_about(E_Module *m);
EAPI int e_modapi_config(E_Module *m);

void _mount_cb_config_updated(void *data);

#endif
