#ifndef _EPHOTO_H_
#define _EPHOTO_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
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
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "config.h"

typedef struct _Ephoto_Config Ephoto_Config;
typedef struct _Ephoto Ephoto;
typedef struct _Ephoto_Entry Ephoto_Entry;

typedef enum _Ephoto_State Ephoto_State;
typedef enum _Ephoto_Orient Ephoto_Orient;

/*Main Functions*/
void ephoto_create_main_window(const char *directory, const char *image);
void ephoto_thumb_size_set(int size);
Evas_Object *ephoto_thumb_add(Evas_Object *parent, const char *path);
void         ephoto_thumb_path_set(Evas_Object *o, const char *path);


/* Configuration */
Eina_Bool ephoto_config_init(Ephoto *em);
void ephoto_config_save(Ephoto *em, Eina_Bool instant);
void ephoto_config_free(Ephoto *em);

/* Preferences */
void ephoto_show_preferences(Ephoto *em);

/*Ephoto Flow Browser*/
Evas_Object *ephoto_create_flow_browser(Evas_Object *parent);
void ephoto_flow_browser_image_set(Evas_Object *obj, const char *current_image);
 /* smart callbacks called:
  * "delete,request" - the user requested to delete the flow browser, typically called when go_back button is pressed or Escape key is typed.
  */


/*Ephoto Slideshow*/
void ephoto_create_slideshow(void);
void ephoto_show_slideshow(int view, const char *current_image);
void ephoto_hide_slideshow(void);
void ephoto_delete_slideshow(void);

/* Ephoto Directory Thumb */
Evas_Object *ephoto_directory_thumb_add(Evas_Object *parent, Ephoto_Entry *e);

/*Ephoto Thumb Browser*/
Evas_Object *ephoto_thumb_browser_add(Evas_Object *parent);
/* smart callbacks called:
 * "selected" - an item in the thumb browser is selected. The selected file is passed as event_info argument.
 * "directory,changed" - the user selected a new directory. The selected directory is passed as event_info argument.
 */

/* Enum for the state machine */
enum _Ephoto_State
{
        EPHOTO_STATE_THUMB,
        EPHOTO_STATE_FLOW,
        EPHOTO_STATE_SLIDESHOW
};

enum _Ephoto_Orient
{
        EPHOTO_ORIENT_0,
        EPHOTO_ORIENT_90,
        EPHOTO_ORIENT_180,
        EPHOTO_ORIENT_270
};

struct _Ephoto_Config
{
        int config_version;

        int thumb_size;
        int thumb_gen_size;

        int remember_directory;
        const char *directory;

        double slideshow_timeout;
        const char *slideshow_transition;

        const char *editor;
};

/*Ephoto Main Structure*/
struct _Ephoto
{
	Evas *e;
	Evas_Object *win;
	Evas_Object *bg;
	Evas_Object *layout;
	Evas_Object *flow_browser;
	Evas_Object *slideshow;
	Evas_Object *slideshow_notify;
	Evas_Object *thumb_browser;
        Eina_List *entries;

        Evas_Object *prefs_win;
        Ephoto_State state;

        Ephoto_Config *config;

        Eet_Data_Descriptor  *config_edd;
        Ecore_Timer *config_save;
};

struct _Ephoto_Entry
{
   const char *path;
   const char *basename; /* pointer inside path */
   const char *label;
   Elm_Gengrid_Item *item;
   Eina_List *dir_files; /* if dir, here contain files with preview */
   Eina_Bool dir_files_checked : 1;
   Eina_Bool is_dir : 1;
   Eina_Bool is_up : 1;
};

Ephoto_Entry *ephoto_entry_new(const char *path, const char *label);
void          ephoto_entry_free(Ephoto_Entry *entry);
void          ephoto_entries_free(Ephoto *em);

extern Ephoto *em;

extern int __log_domain;
#define DBG(...) EINA_LOG_DOM_DBG(__log_domain, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(__log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(__log_domain, __VA_ARGS__)

static inline Eina_Bool
_ephoto_eina_file_direct_info_image_useful(const Eina_File_Direct_Info *info)
{
   const char *type, *bname, *ext;

   bname = info->path + info->name_start;
   if (bname[0] == '.') return EINA_FALSE;
   if ((info->dirent->d_type != DT_REG) && (info->dirent->d_type != DT_UNKNOWN))
     return EINA_FALSE;

   ext = info->path + info->path_length - 1;
   for (; ext > bname; ext--) if (*ext == '.') break;
   if (*ext == '.')
     {
        ext++;
        if ((strcasecmp(ext, "jpg") == 0) ||
            (strcasecmp(ext, "jpeg") == 0) ||
            (strcasecmp(ext, "png") == 0))
          return EINA_TRUE;
     }

   if (!(type = efreet_mime_type_get(info->path))) return EINA_FALSE;
   return strncmp(type, "image/", sizeof("image/") - 1) == 0;
}

#endif
