#ifndef _STICKIES_H
#define _STICKIES_H
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <Etk.h>
#include <Eet.h>
#include <Ecore_File.h>
#include <Ecore_X.h>

#include "config.h"

#define ERROR(string) \
     fprintf(stderr,"[estickies error]: "#string"\n");
#define DEBUG(string) \
   fprintf(stdout,"[estickies debug]: "#string"\n");

#define E_CONTAINS(x, y, w, h, xx, yy, ww, hh) (((xx) >= (x)) && (((x) + (w)) >= ((xx) + (ww))) && ((yy) >= (y)) && (((y) + (h)) >= ((yy) + (hh))))
#define E_INTERSECTS(x, y, w, h, xx, yy, ww, hh) (((x) < ((xx) + (ww))) && ((y) < ((yy) + (hh))) && (((x) + (w)) > (xx)) && (((y) + (h)) > (yy)))

#ifdef E_FREE
#undef E_FREE
#endif
#define E_FREE(ptr) if(ptr) { free (ptr); ptr = NULL;}

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
#include "theme_chooser.h"
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
   Etk_Widget *dia;
   Etk_Widget *filechooser;
   Etk_Widget *entry;
   Etk_Widget *export_mode;
   E_Sticky *s;
};

struct _E_Sticky
{
   int x, y, w, h;
   int r, g, b, a;
   int stick;
   int locked;
   char *theme;
   char *text;
   
   /* non-savable fields */
   Etk_Widget *win;
   Etk_Widget *textview;
   Etk_Widget *buttonbox;
   Etk_Widget *stick_toggle;
   Etk_Widget *lock_toggle;
   Etk_Widget *close_button;   
};

struct _E_Stickies
{
   char *theme;
   Eina_List *stickies;
   E_Config_Version *version;
};

E_Sticky *_e_sticky_new();
void _e_sticky_window_add(E_Sticky *s);
E_Sticky *_e_sticky_new_show_append();
void _e_sticky_destroy(E_Sticky *s);
void _e_sticky_delete(E_Sticky *s);
void _e_sticky_move(E_Sticky *s, int x, int y);
void _e_sticky_resize(E_Sticky *s, int w, int h);
Etk_Bool _e_sticky_exists(E_Sticky *s);    
void _e_sticky_show(E_Sticky *s);
void _e_sticky_move_resize(E_Sticky *s);
void _e_sticky_menu_show(E_Sticky *s);
void _e_sticky_stick_toggle(E_Sticky *s);
void _e_sticky_lock_toggle(E_Sticky *s);
void _e_sticky_load_from(E_Sticky *s);
void _e_sticky_lock_set(E_Sticky *s, Etk_Bool on);    
void _e_sticky_properties_set(E_Sticky *s);
void _e_sticky_theme_apply(E_Sticky *s, const char *theme);
void _e_sticky_theme_apply_all(const char *theme);
void _e_sticky_export_to(E_Sticky *s);
void _e_sticky_delete_confirm(E_Sticky *s);
    
#endif
