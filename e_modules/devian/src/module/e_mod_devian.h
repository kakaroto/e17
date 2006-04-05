#ifdef E_MOD_DEVIAN_TYPEDEFS
typedef struct _dEvian_Main DEVIAN_MAIN;
typedef struct _dEvian DEVIANN;

#else

#ifndef E_MOD_DEVIAN_H_INCLUDED
#define E_MOD_DEVIAN_H_INCLUDED

#ifdef HAVE_ALL
#define DEVIAN_VERSION "0.1.6"
#else
#ifdef HAVE_PICTURE
#define DEVIAN_VERSION "0.1.6"
#else
#ifdef HAVE_RSS
#define DEVIAN_VERSION "0.1.6"
#endif
#endif
#endif

#define DEVIAN_THEME_DEFAULT "minimal"
#define DEVIAN_THEME_TYPE_POPUP 0
#ifdef HAVE_PICTURE
#define DEVIAN_THEME_TYPE_PICTURE 1     /* Must be the same than SOURCE_PICTURE */
#endif
#ifdef HAVE_RSS
#define DEVIAN_THEME_TYPE_RSS 2 /* Must be the same than SOURCE_RSS */
#endif
#ifdef HAVE_FILE
#define DEVIAN_THEME_TYPE_FILE 3        /* Must be the same than SOURCE_FILE */
#endif

#define DEVIAN_MAX_PATH 200
#define DEVIAN_ID_LEN 50

#define DEVIAN_VIEWER_HTTP_DEFAULT "firefox"
#define DEVIAN_VIEWER_IMAGE_DEFAULT "exhibit"
#define DEVIAN_VIEWER_FILE_DEFAULT "emacs"

#define SIZE_POLICY_DEFAULT 1
#define SIZE_POLICY_AUTO 0
#define SIZE_POLICY_USER 1

/* Name of the global main dEvian is strcat(DEVIAN_NAME,"M") */

/*
#ifdef HAVE_ALL
#define DEVIANM dEvianM
#define DEVIANN dEvian
#define DEVIANF(x) devian_ ## x
#else
#ifdef HAVE_PICTURE
#define DEVIANM dEpictureM
#define DEVIANN dEpicture
#define DEVIANF(x) depicture_ ## x
#else
#ifdef HAVE_RSS
#define DEVIANM dErssM
#define DEVIANN dErss
#define DEVIANF(x) derss_ ## x
#endif
#endif
#endif
*/

DEVIAN_MAIN *DEVIANM;

/**
 *Struct for dEvian module
 */
struct _dEvian_Main
{
   E_Module *module;
   E_Container *container;
   char *display;

   //Evas_Coord canvas_w, canvas_h;
   int canvas_w, canvas_h;
   Evas_List *devians;

   int container_box_count;
   int container_bar_count;
#ifdef HAVE_PICTURE
   int source_picture_count;
#endif

   Evas_List *bars;

#ifdef HAVE_PICTURE
   Picture_List_Local *picture_list_local;
   Picture_List_Net *picture_list_net;
   Picture_Cache *picture_cache;
#endif

   /* Panel */
   E_Config_Dialog *dialog_conf;
   E_Config_Dialog *dialog_conf_theme;
   /* Module config */
   DEVIAN_MAIN_CONF *conf;
};

/**
 * Struct for each dEvian
 * Each one has a source and a container
 * One dEvian is one information displayed somewhere
 */
struct _dEvian
{
   char *id;

   void *source;
   /* Source actions wich dEvian handles */
   /* ...TODO: Add other actions */
   struct
   {
      void (*timer_change) (DEVIANN *devian, int active, int time);
      int (*refresh) (DEVIANN *devian, int option);
      int (*set_bg) (DEVIANN *devian);
      int (*viewer) (DEVIANN *devian);
      int (*gui_update) (DEVIANN *devian);
   } source_func;
   struct
   {
      int provide_double_buf;
      int provide_previous;
      int provide_set_bg;
      int allow_info_panel;
      int paused;
   } source_info;

   void *container;
   /* Container actions wich dEvian handles */
   /* ...TODO: Add other actions */
   struct
   {
      void (*update_actions) (DEVIANN *devian);
      void (*resize_auto) (DEVIANN *devian);
      void (*alpha_set) (void *container, int alpha);
      int (*is_in_transition) (DEVIANN *devian);
   } container_func;

   int size_policy;
   Popup_Warn *popup_warn;

   /* The devian is going to die */
   int dying;

   DEVIAN_CONF *conf;
   E_Config_Dialog *dialog_conf;
   E_Config_Dialog *dialog_conf_rss;
   E_Menu *menu;
};

/* Functions about dEvian */

int DEVIANF(devian_main_init) (E_Module *m);
void DEVIANF(devian_main_shutdown) (void);

DEVIANN *DEVIANF(devian_add) (int source_type, DEVIAN_CONF *cfg_devian);
void DEVIANF(devian_del) (DEVIANN *devian, int now);
void DEVIANF(devian_del_all) (void);

void DEVIANF(devian_set_id) (DEVIANN *devian, int source, char *c2);

/* Some wrappers for librairies/functionalities used by dEvian */

char *DEVIANF(display_init) (void);
void DEVIANF(display_shutdown) (void);

const char *DEVIANF(devian_edje_load) (Evas_Object *edje_obj, char *part, int type);

const char *DEVIANF(ss_utf8_add) (char *text, int tfree);

#endif
#endif
