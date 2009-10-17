#ifndef _STICKIES_H
#define _STICKIES_H
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <Elementary.h>
#include <Eina.h>
#include <Eet.h>
#include <Ecore_File.h>
#include <Ecore_X.h>
#include <Ecore_X_Atoms.h>
#include <locale.h>
#include "../../config.h"
#define _(x) gettext(x)

#define ERROR(string) fprintf(stderr, "[estickies error]: "#string"\n")
#define WARN(string) printf("[estickies warning]: "#string"\n")
#define INFO(string) printf("[estickies info]: "#string"\n")
#define DEBUG(string) printf("[estickies debug]: "#string"\n")

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

typedef struct _E_Config_Sticky E_Config_Sticky;
typedef struct _E_Config_Stickies E_Config_Stickies;
typedef struct _E_Config_General E_Config_General;
typedef struct _E_Config_Version E_Config_Version;
typedef struct _E_Filedialog E_Filedialog;
typedef struct _E_Sticky E_Sticky;
typedef struct _E_Stickies E_Stickies;

#include "conf.h"
#include "config_gui.h"
#include "about.h"

struct _E_Config_Sticky
{
   int x, y, w, h;
   int r, g, b, a;
   char *text;
};

struct _E_Config_Stickies
{
   char *theme;
   Eina_List *stickies;
   Eina_Bool *composite;
};

struct _E_Config_General
{
   int nothing;
};

struct _E_Config_Version
{
   int major;
   int minor;
   int patch;
};

struct _E_Filedialog
{
   Evas_Object *dia;
   Evas_Object *filechooser;
   Evas_Object *export_mode;
   E_Sticky *s;
};

struct _E_Sticky
{
   int x, y, w, h;
   int r, g, b, a;
   Eina_Bool stick;
   Eina_Bool locked;
   Eina_Bool list_shown;
   char *theme;
   char *text;
   
   /* non-savable fields */
   Evas_Object *win;
   Evas_Object *background;
   Evas_Object *scroller;
   Evas_Object *textentry;
   Evas_Object *layout;
   Evas_Object *list;
   Evas_Object *vbox;
   Evas_Object *stick_toggle;
   Evas_Object *lock_toggle;
   Evas_Object *close_button;
   Evas_Object *actions_toggle;
   Evas_Object *layoutbox;
};

struct _E_Stickies
{
   char *theme;
   Eina_List *stickies;
   E_Config_Version *version;
   Eina_Bool composite;
};

E_Sticky *_e_sticky_new(void);
void _e_sticky_window_add(E_Sticky *s);
E_Sticky *_e_sticky_new_show_append(void);
void _e_sticky_destroy(E_Sticky *s);
void _e_sticky_delete(E_Sticky *s);
void _e_sticky_move(E_Sticky *s, int x, int y);
void _e_sticky_resize(E_Sticky *s, int w, int h);
Eina_Bool _e_sticky_exists(E_Sticky *s);    
void _e_sticky_show(E_Sticky *s);
void _e_sticky_move_resize(E_Sticky *s);
void _e_sticky_lock_toggle(E_Sticky *s);
void _e_sticky_properties_set(E_Sticky *s);
void _e_sticky_theme_apply(E_Sticky *s, const char *theme);
void _e_sticky_theme_apply_all(const char *theme);
void _e_sticky_export_to(E_Sticky *s);
void _e_sticky_delete_confirm(E_Sticky *s);

extern const char *home;
    
#endif
