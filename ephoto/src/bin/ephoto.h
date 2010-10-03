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
#include <Ethumb.h>
#include <Ethumb_Client.h>
#include <Eio.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

typedef struct _Ephoto_Config Ephoto_Config;
typedef struct _Ephoto Ephoto;

typedef enum _Ephoto_State Ephoto_State;
typedef enum _Ephoto_Orient Ephoto_Orient;

/*Main Functions*/
void ephoto_create_main_window(const char *directory, const char *image);

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

/*Ephoto Thumb Browser*/
Evas_Object *ephoto_create_thumb_browser(Evas_Object *parent);
void ephoto_populate_thumbnails(Evas_Object *obj);
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

        int remember_directory;
        const char *directory;

        double slideshow_timeout;
        const char *slideshow_transition;

        const char *editor;

        int sort_images;
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
        Evas_Object *prefs_win;
	Eina_List   *images;
        Ephoto_State state;

        Ephoto_Config *config;

        Eet_Data_Descriptor  *config_edd;
        Ecore_Timer *config_save;
};

extern Ephoto *em;

extern int __log_domain;
#define DBG(...) EINA_LOG_DOM_DBG(__log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(__log_domain, __VA_ARGS__)

#endif
