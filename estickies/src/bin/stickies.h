#ifndef _STICKIES_H
#define _STICKIES_H

// BASIC INCLUDES

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>

// EFL INCLUDES

#include <Elementary.h>
#include <Eina.h>
#include <Eet.h>
#include <Ecore_File.h>
#include <Ecore_X.h>
#include <Ecore_X_Atoms.h>

// GETTEXT

#include <locale.h>
#include "../../config.h"
#define _(x) gettext(x)

// PRINT MESSAGES

#define ERROR(string) fprintf(stderr, "[estickies error]: "#string"\n")
#define WARN(string) printf("[estickies warning]: "#string"\n")
#define INFO(string) printf("[estickies info]: "#string"\n")
#define DEBUG(string) printf("[estickies debug]: "#string"\n")

// USEFUL DEFINES

#define E_CONTAINS(x, y, w, h, xx, yy, ww, hh) (((xx) >= (x)) && (((x) + (w)) >= ((xx) + (ww))) && ((yy) >= (y)) && (((y) + (h)) >= ((yy) + (hh))))
#define E_INTERSECTS(x, y, w, h, xx, yy, ww, hh) (((x) < ((xx) + (ww))) && ((y) < ((yy) + (hh))) && (((x) + (w)) > (xx)) && (((y) + (h)) > (yy)))

#ifdef E_FREE
#undef E_FREE
#endif
#define E_FREE(ptr) if(ptr) { ptr = NULL; free (ptr);}

#ifdef E_NEW
#undef E_NEW
#endif
#define E_NEW(count, type) calloc(count, sizeof(type))

#define DEFAULT_THEME "default.edj"

// API

# ifdef ESAPI
#  undef ESAPI
# endif
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define ESAPI __attribute__ ((visibility("default")))
#  else
#   define ESAPI
#  endif
# else
#  define ESAPI
# endif

// STRUCTURE TYPEDEFS

typedef struct _E_Config_Sticky E_Config_Sticky;
typedef struct _E_Config_Stickies E_Config_Stickies;
typedef struct _E_Config_General E_Config_General;
typedef struct _E_Config_Version E_Config_Version;
typedef struct _E_Filedialog E_Filedialog;
typedef struct _E_Sticky E_Sticky;
typedef struct _E_Stickies E_Stickies;

// STRUCTURES THEMSELVES

// global stickies settings for EET
struct _E_Config_Stickies
{
   char *theme;
   Eina_List *stickies;
   Eina_Bool *composite;
};

// general settings for EET. TODO: USE IT SOMEWHERE.
struct _E_Config_General
{
   int nothing;
};

// version used for comparing configuration files etc for EET
struct _E_Config_Version
{
   int major;
   int minor;
   int patch;
};

// filedialog structure for exporting.
struct _E_Filedialog
{
   Evas_Object *dia;
   Evas_Object *filechooser;
   Evas_Object *export_mode;
   E_Sticky *s;
};

// XXX: sticky itself.
struct _E_Sticky
{
   // position stuff
   int x, y, w, h;
   // booleans used for sticky
   Eina_Bool stick;
   Eina_Bool locked;
   Eina_Bool list_shown;
   // booleans used for edje object
   Eina_Bool stick_toggle_state;
   Eina_Bool lock_toggle_state;
   Eina_Bool actions_toggle_state;
   // theme used for sticky and text inside.
   char *theme;
   char *text;
   // evas objects inside sticky
   Evas_Object *win;
   Evas_Object *scroller;
   Evas_Object *textentry;
   Evas_Object *list;
   Evas_Object *sticky;
   // various window stuff
   Ecore_X_Window xwin;
   Ecore_Event_Handler *evhandler;
   Ecore_X_Window_State state[2];
};

// global stickies settings
struct _E_Stickies
{
   char *theme;
   Eina_List *stickies;
   E_Config_Version *version;
   Eina_Bool composite;
};

/*
 * VARIOUS PROTOTYPES:
 *  if you'll want to call some functions
 *  always include this stickies.h file from your .c file
 *  and when you'll add new prototypes, add it here.
 *
 */

/*
 * PROTOTYPES FOR STICKIES.C
 *  prototypes for functions in stickies.c
 *  propably won't be called in function which is not in stickies.c
 *
 */
ESAPI E_Sticky *_e_sticky_new(void);
ESAPI E_Sticky *_e_sticky_new_show_append(void);

ESAPI void _e_sticky_window_add(E_Sticky *s);
ESAPI void _e_sticky_properties_set(E_Sticky *s);

ESAPI void _e_sticky_destroy(E_Sticky *s);
ESAPI void _e_sticky_delete(E_Sticky *s);
ESAPI void _e_sticky_delete_confirm(E_Sticky *s);
ESAPI void _e_sticky_show(E_Sticky *s);

ESAPI void _e_sticky_move(E_Sticky *s, int x, int y);
ESAPI void _e_sticky_resize(E_Sticky *s, int w, int h);
ESAPI void _e_sticky_move_resize(E_Sticky *s);
ESAPI void _e_sticky_lock_toggle(E_Sticky *s);

ESAPI void _e_sticky_theme_apply(E_Sticky *s, const char *theme);
ESAPI void _e_sticky_theme_apply_all(const char *theme);

ESAPI void _e_sticky_export_to(E_Sticky *s);

/*
 * PROTOTYPES FOR STICKY_GUI.C
 *  prototypes for functions in sticky_gui.c
 *  functions from this file are called inside stickies.h
 *  file is made to make functions and files shorter and easier to modify.
 *
 */
ESAPI void _e_sticky_win_add(E_Sticky *s);
ESAPI void _e_sticky_edje_add(E_Sticky *s);
ESAPI void _e_sticky_scroller_add(E_Sticky *s);
ESAPI void _e_sticky_entry_add(E_Sticky *s);

// ABOUT.C : JUST ONE PROTOTYPE
ESAPI void _e_about_show(void);

/*
 * PROTOTYPES FOR CONF.C
 *  prototypes for functions in conf.c
 *  can be called everywhere where needed.
 *
 */
ESAPI int _e_config_init(void);
ESAPI int _e_config_shutdown(void);

ESAPI E_Config_Version *_e_config_version_parse(char *version);
ESAPI int _e_config_version_compare(E_Config_Version *v1, E_Config_Version *v2);
ESAPI int _e_config_load(E_Stickies *ss);
ESAPI int _e_config_save(E_Stickies *ss);
ESAPI void _e_config_defaults_apply(E_Stickies *ss);

// CONFIG_GUI.C: JUST TWO PROTOTYPES
ESAPI void _e_theme_chooser_show(E_Sticky *s);
ESAPI Eina_Bool _e_sticky_exists(E_Sticky *s);

/*
 * PROTOTYPES FOR PLUGIN.C
 *  prototypes for functions in plugin.c
 *  it is not done and should not be used.
 *  it's my TODO.
 *
 */
ESAPI int _e_plugin_init();
ESAPI int _e_plugin_shutdown();

ESAPI int _e_plugin_load();
ESAPI int _e_plugin_unload();

// HOME VARIABLE: USED IN SEVERAL FILES, THATS WHY EXTERN.
//  useless to define in every file separately.
extern const char *home;
// EXTERN SS STRUCT BECAUSE IT IS USED IN SEVERAL FILES.
extern E_Stickies *ss;
    
#endif
