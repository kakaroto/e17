#ifndef _EPHOTO_H_
#define _EPHOTO_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eet.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_File.h>
#include <Efreet_Mime.h>
#include <Elementary.h>
#include <Eina.h>
#include <Edje.h>
#include <Evas.h>
#include <Eio.h>
#include <Ethumb_Client.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define THEME_FILE PACKAGE_DATA_DIR"/themes/default/ephoto.edj"

/*Typedefs*/
typedef struct _Ephoto_Config Ephoto_Config;
typedef struct _Ephoto Ephoto;
typedef struct _Ephoto_Entry Ephoto_Entry;
typedef struct _Ephoto_Event_Entry_Create Ephoto_Event_Entry_Create;

typedef enum _Ephoto_State Ephoto_State;
typedef enum _Ephoto_Orient Ephoto_Orient;

/*Main Gui Functions/Callbacks*/
Evas_Object *ephoto_window_add(const char *path);
void         ephoto_title_set(Ephoto *ephoto, const char *title);
void         ephoto_thumb_size_set(Ephoto *ephoto, int size);
Evas_Object *ephoto_thumb_add(Ephoto *ephoto, Evas_Object *parent, const char *path);
void         ephoto_thumb_path_set(Evas_Object *o, const char *path);
void         ephoto_directory_set(Ephoto *ephoto, const char *path);
void         ephoto_auto_hide_toolbar(void *data __UNUSED__, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__);

/*Get the exif orientation of a JPEG*/
Ephoto_Orient ephoto_file_orient_get(const char *path);

/*Config Functions/Callbacks*/
Eina_Bool    ephoto_config_init(Ephoto *em);
void         ephoto_config_save(Ephoto *em, Eina_Bool instant);
void         ephoto_config_free(Ephoto *em);

/*Single Browser Functions/Callbacks*/
Evas_Object *ephoto_single_browser_add(Ephoto *ephoto, Evas_Object *parent);
void         ephoto_single_browser_entry_set(Evas_Object *obj, Ephoto_Entry *entry);
void         ephoto_single_browser_path_pending_set(Evas_Object *obj, const char *path);
 /* smart callbacks called:
  * "back" - the user wants to go back to the previous screen.
  * "slideshow" - the user wants to view a slideshow.
  */

/*Flow Browser Functions/Callbacks*/
Evas_Object *ephoto_flow_browser_add(Ephoto *e, Evas_Object *parent);
void         ephoto_flow_browser_entry_set(Evas_Object *obj __UNUSED__, Ephoto_Entry *entry);
/* smart callbacks called:
 * "back" - the user wants to go back to the previous screen.
 * "slideshow" - the user wants to view a slideshow.
 */

/*Slideshow Functions/Callbacks*/
Evas_Object *ephoto_slideshow_add(Ephoto *ephoto, Evas_Object *parent);
void         ephoto_slideshow_entry_set(Evas_Object *obj, Ephoto_Entry *entry);
 /* smart callbacks called:
  * "back" - the user want to go back to the previous screen.
  */

/*Main Thumb Browser Function*/
Evas_Object *ephoto_thumb_browser_add(Ephoto *ephoto, Evas_Object *parent);
void         ephoto_thumb_browser_entry_set(Evas_Object *obj, Ephoto_Entry *entry);

/* smart callbacks called:
 * "selected" - an item in the thumb browser is selected. The selected Ephoto_Entry is passed as event_info argument.
 */

/*Ephoto View*/
enum _Ephoto_State
{
  EPHOTO_STATE_THUMB,
  EPHOTO_STATE_FLOW,
  EPHOTO_STATE_SINGLE,
  EPHOTO_STATE_SLIDESHOW
};

/*Ephoto Orientation*/
enum _Ephoto_Orient
{
  EPHOTO_ORIENT_0 = 1,
  EPHOTO_ORIENT_FLIP_HORIZ = 2,
  EPHOTO_ORIENT_180 = 3,
  EPHOTO_ORIENT_FLIP_VERT = 4,
  EPHOTO_ORIENT_FLIP_VERT_90 = 5,
  EPHOTO_ORIENT_90 = 6,
  EPHOTO_ORIENT_FLIP_HORIZ_90 = 7,
  EPHOTO_ORIENT_270 = 8
};

/*Ephoto Config*/
struct _Ephoto_Config
{
   int config_version;
   int autohide_toolbar;
   int best_fit_images;
   int thumb_size;
   int thumb_gen_size;
   double slideshow_timeout;
   const char *directory;
   const char *slideshow_transition;
};

/*Ephoto Main*/
struct _Ephoto
{
   Evas_Object *win;
   Evas_Object *bg;
   Evas_Object *pager;

   Evas_Object *thumb_browser;
   Evas_Object *flow_browser;
   Evas_Object *single_browser;
   Evas_Object *slideshow;

   Eina_List *entries;
   Eina_List *thumbs;

   int thumb_gen_size;
   struct {
      Ecore_Timer *thumb_regen;
   } timer;
   struct {
      Ecore_Job *change_dir;
   } job;

   Eio_File *ls;

   Evas_Object *prefs_win;
   Ephoto_State state, prev_state;

   Ephoto_Config *config;
};

/*Ephoto Entry*/
struct _Ephoto_Entry
{
   const char *path;
   const char *basename;
   const char *label;
   Ephoto *ephoto;
   Elm_Gengrid_Item *item;
   Eina_List *free_listeners;
};

struct _Ephoto_Event_Entry_Create
{
   Ephoto_Entry *entry;
};

/*Ephoto Entry Functions/Callbacks*/
Ephoto_Entry *ephoto_entry_new(Ephoto *ephoto, const char *path, const char *label);
void          ephoto_entry_free(Ephoto_Entry *entry);
void          ephoto_entry_free_listener_add(Ephoto_Entry *entry, void (*cb)(void *data, const Ephoto_Entry *entry), const void *data);
void          ephoto_entry_free_listener_del(Ephoto_Entry *entry, void (*cb)(void *data, const Ephoto_Entry *entry), const void *data);
void          ephoto_entries_free(Ephoto *ephoto);

/*Ephoto Logging*/
extern int __log_domain;
#define DBG(...) EINA_LOG_DOM_DBG(__log_domain, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(__log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(__log_domain, __VA_ARGS__)

/*Checks to determine if a file is an image*/
static inline Eina_Bool
_ephoto_eina_file_direct_info_image_useful(const Eina_File_Direct_Info *info)
{
   const char *bname, *ext;

   bname = info->path + info->name_start;
   if (bname[0] == '.') return EINA_FALSE;
   if ((info->type != EINA_FILE_REG) && (info->type != EINA_FILE_UNKNOWN))
     return EINA_FALSE;

   ext = info->path + info->path_length - 1;
   for (; ext > bname; ext--) if (*ext == '.') break;
   if (*ext == '.')
     {
        ext++;
        if ((strcasecmp(ext, "jpg") == 0) ||
            (strcasecmp(ext, "jpeg") == 0) ||
            (strcasecmp(ext, "png") == 0) ||
            (strcasecmp(ext, "svg") == 0) ||
            (strcasecmp(ext, "svgz") == 0) ||
            (strcasecmp(ext, "bmp") == 0) ||
            (strcasecmp(ext, "dib") == 0) ||
            (strcasecmp(ext, "tiff") == 0) ||
            (strcasecmp(ext, "tif") == 0))
          return EINA_TRUE;
     }

   return EINA_FALSE;
}

/*Ephoto Event Handlers*/
extern int EPHOTO_EVENT_ENTRY_CREATE;
extern int EPHOTO_EVENT_POPULATE_START;
extern int EPHOTO_EVENT_POPULATE_END;
extern int EPHOTO_EVENT_POPULATE_ERROR;

#endif
