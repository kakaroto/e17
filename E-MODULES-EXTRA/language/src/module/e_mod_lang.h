#ifndef E_MOD_LANG_H
#define E_MOD_LANG_H

#include <e.h>
#include "e_mod_main.h"

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKBrules.h>

typedef struct _Language		  Language;
typedef struct _Language_Predef		  Language_Predef;
typedef struct _Language_Kbd_Model	  Language_Kbd_Model;
typedef struct _Border_Language_Settings  Border_Language_Settings;

struct _Language_Kbd_Model
{
   const char *kbd_model;
   const char *kbd_model_description;
};

struct _Border_Language_Settings
{
   E_Border    *bd;
   int	       language_selector;
   const char  *language_name;
};

struct _Language
{
   unsigned int		id;
   const char		*lang_name;
   const char		*lang_shortcut;
   const char		*lang_flag;
   XkbRF_VarDefsRec	rdefs;
   XkbComponentNamesRec	cNames;
};

struct _Language_Predef
{
   const char  *lang_name;
   const char  *lang_shortcut;
   const char  *lang_flag;
   const char  *kbd_layout;
   Evas_List   *kbd_variant; // const char *
};

void	    lang_language_switch_to(Config *cfg, unsigned int n);
void	    lang_language_switch_to_next(Config *cfg);
void	    lang_language_switch_to_prev(Config *cfg);

Language    *lang_get_default_language(Config *cfg);

void	    lang_load_xfree_language_kbd_layouts(Config *cfg);
void	    lang_free_xfree_language_kbd_layouts(Config *cfg);

void	    lang_load_xfree_kbd_models(Config *cfg);
void	    lang_free_xfree_kbd_models(Config *cfg);

Language    *lang_language_copy(const Language *l);
void	    lang_language_free(Language *l);

const char  *lang_language_current_kbd_model_get();

int	    lang_language_xorg_values_get(Language *l);

/*************** border callback handlers *****************/

int lang_cb_event_desk_show(void *data, int type, void *ev);
int lang_cb_event_border_focus_in(void *data, int type, void *ev);
int lang_cb_event_border_remove(void *data, int type, void *ev);
int lang_cb_event_border_iconify(void *data, int type, void *ev);

#if 0
int lang_cb_event_border_zone_set(void *data, int type, void *event);
int lang_cb_event_border_desk_set(void *data, int type, void *event);
int lang_cb_event_border_show(void *data, int type, void *event);
int lang_cb_event_border_hide(void *data, int type, void *event);
#endif

#endif
